/**
 * @file DdsManager.cpp
 * @brief DdsManager 클래스 구현
 *
 * RTI Connext DDS 초기화, 메시지 송수신, 연결 상태 관리를 담당합니다.
 */

#include "DdsManager.hpp"

#include <QDateTime>
#include <QDebug>

// ============================================================================
// 생성자 / 소멸자
// ============================================================================

DdsManager::DdsManager(QObject* parent) : QObject(parent) {}

DdsManager::~DdsManager() {
    // DDS 엔티티는 unique_ptr에 의해 자동 정리됨
}

// ============================================================================
// DDS 초기화
// ============================================================================

void DdsManager::start() {
    try {
        // 1. DomainParticipant 생성 (Domain ID: 0)
        m_participant = std::make_unique<dds::domain::DomainParticipant>(0);

        // 2. Publisher/Subscriber 생성
        m_publisher = std::make_unique<dds::pub::Publisher>(*m_participant);
        m_subscriber = std::make_unique<dds::sub::Subscriber>(*m_participant);

        // 3. Topic 생성 (IDL에서 정의된 PingPongMessage 타입 사용)
        m_pingTopic = std::make_unique<dds::topic::Topic<PingPongMessage>>(*m_participant, "Ping");
        m_pongTopic = std::make_unique<dds::topic::Topic<PingPongMessage>>(*m_participant, "Pong");

        // 4. DataWriter/DataReader 생성
        m_writer =
            std::make_unique<dds::pub::DataWriter<PingPongMessage>>(*m_publisher, *m_pongTopic);
        m_reader =
            std::make_unique<dds::sub::DataReader<PingPongMessage>>(*m_subscriber, *m_pingTopic);

        // 5. 리스너 등록 (data_available + subscription_matched)
        m_listener = std::make_shared<PingListener>(this);
        auto mask = dds::core::status::StatusMask::data_available() |
                    dds::core::status::StatusMask::subscription_matched();
        m_reader->listener(m_listener.get(), mask);

        // 6. UI 상태 업데이트
        m_status = "Waiting for Daemon...";
        emit statusChanged();
        addLogMessage("DDS Started. Waiting for Daemon...");

    } catch (const std::exception& e) {
        // 초기화 실패 시 에러 처리
        m_status = QString("Error: %1").arg(e.what());
        emit statusChanged();
        qWarning() << "DDS Init Error:" << e.what();
    }
}

// ============================================================================
// 메시지 처리
// ============================================================================

void DdsManager::handleIncomingPing(int seq, const QString& sender) {
    // Ping 수신 로그 기록
    QString log = QString("[DDS] Received Ping (%1) from %2").arg(seq).arg(sender);
    addLogMessage(log);

    // 자동으로 Pong 응답 전송
    sendPong(seq);
}

void DdsManager::sendPong(int sequence) {
    if (!m_writer) {
        return;  // Writer가 없으면 무시
    }

    try {
        // Pong 메시지 생성 및 전송
        PingPongMessage msg("QtApp", sequence);
        m_writer->write(msg);
        addLogMessage(QString("[DDS] Sent Pong (%1) back").arg(sequence));
    } catch (const std::exception& e) {
        addLogMessage(QString("[Error] Failed to send Pong: %1").arg(e.what()));
    }
}

// ============================================================================
// 연결 상태 관리
// ============================================================================

void DdsManager::updateConnectionStatus(int writerCount) {
    m_connectedWriters = writerCount;

    if (writerCount > 0) {
        // Daemon 연결됨
        m_status = QString("Connected (%1 writer)").arg(writerCount);
        addLogMessage(QString("Daemon connected! (%1 writer)").arg(writerCount));
    } else {
        // Daemon 연결 해제됨
        m_status = "Daemon disconnected";
        addLogMessage("Daemon disconnected!");
    }

    emit statusChanged();
}

// ============================================================================
// UI 로깅
// ============================================================================

void DdsManager::addLogMessage(const QString& msg) {
    // 스레드 안전한 메시지 추가
    std::lock_guard<std::mutex> lock(m_mutex);

    // 타임스탬프와 함께 메시지 추가 (최신 메시지가 맨 위)
    m_messages.prepend(QDateTime::currentDateTime().toString("hh:mm:ss") + " - " + msg);

    // 메시지 목록 크기 제한 (최대 50개)
    if (m_messages.size() > 50) {
        m_messages.removeLast();
    }

    emit messagesChanged();
}
