/**
 * @file DdsManager.hpp
 * @brief Qt-DDS 브릿지 클래스 정의
 *
 * RTI Connext DDS와 Qt QML UI 간의 통신을 담당하는 클래스입니다.
 * DDS DataReader 콜백을 Qt 시그널로 변환하여 스레드 안전한 UI 업데이트를 제공합니다.
 */

#ifndef DDSMANAGER_HPP
#define DDSMANAGER_HPP

#include <QObject>
#include <QStringListModel>
#include <dds/dds.hpp>
#include <memory>
#include <mutex>

#include "../gen/ping_pong.hpp"

/**
 * @class DdsManager
 * @brief DDS와 Qt QML 간의 브릿지 클래스
 *
 * 이 클래스는 다음 기능을 제공합니다:
 * - DDS DomainParticipant 초기화 및 관리
 * - Ping 토픽 구독 (DataReader)
 * - Pong 토픽 발행 (DataWriter)
 * - 연결 상태 모니터링 (subscription_matched 콜백)
 * - QML UI에 상태 및 메시지 노출 (Q_PROPERTY)
 */
class DdsManager : public QObject {
    Q_OBJECT

    // QML에서 접근 가능한 프로퍼티
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)

   public:
    /**
     * @brief 생성자
     * @param parent Qt 부모 객체 (메모리 관리용)
     */
    explicit DdsManager(QObject* parent = nullptr);

    /**
     * @brief 소멸자 - DDS 엔티티 정리
     */
    ~DdsManager();

    // Getter 함수들
    [[nodiscard]] QString status() const { return m_status; }
    [[nodiscard]] QStringList messages() const { return m_messages; }

   public slots:
    /**
     * @brief DDS 초기화 및 시작
     *
     * DomainParticipant, Topic, DataWriter, DataReader를 생성하고
     * 리스너를 등록합니다.
     */
    void start();

    /**
     * @brief Pong 메시지 전송
     * @param sequence 응답할 시퀀스 번호
     */
    void sendPong(int sequence);

    /**
     * @brief UI 로그 메시지 추가
     * @param msg 표시할 메시지
     */
    void addLogMessage(const QString& msg);

   signals:
    /** @brief 연결 상태 변경 시그널 */
    void statusChanged();

    /** @brief 메시지 목록 변경 시그널 */
    void messagesChanged();

    /** @brief Ping 수신 시그널 */
    void pingReceived(int sequence);

   private:
    /**
     * @class PingListener
     * @brief DDS DataReader 리스너 (내부 클래스)
     *
     * DDS 콜백을 받아 Qt 시그널로 변환합니다.
     * - on_data_available: 새 데이터 수신 시 호출
     * - on_subscription_matched: 상대 Writer 연결/해제 시 호출
     */
    class PingListener : public dds::sub::NoOpDataReaderListener<PingPongMessage> {
       public:
        explicit PingListener(DdsManager* manager) : m_parent(manager) {}

        /**
         * @brief 데이터 수신 콜백
         * @param reader 데이터를 수신한 DataReader
         */
        void on_data_available(dds::sub::DataReader<PingPongMessage>& reader) override {
            auto samples = reader.take();
            for (const auto& sample : samples) {
                if (sample.info().valid()) {
                    const auto& data = sample.data();
                    // Qt UI 스레드로 안전하게 전달
                    QMetaObject::invokeMethod(
                        m_parent, "handleIncomingPing", Qt::QueuedConnection,
                        Q_ARG(int, data.sequence_num()),
                        Q_ARG(QString, QString::fromStdString(data.sender_id())));
                }
            }
        }

        /**
         * @brief 구독 매칭 콜백 - Writer 연결/해제 감지
         * @param reader DataReader 참조
         * @param status 현재 매칭 상태
         */
        void on_subscription_matched(
            dds::sub::DataReader<PingPongMessage>& reader,
            const dds::core::status::SubscriptionMatchedStatus& status) override {
            (void)reader;  // 미사용 파라미터
            int count = status.current_count();
            int change = status.current_count_change();
            qDebug() << "[DDS] subscription_matched: count=" << count << ", change=" << change;
            QMetaObject::invokeMethod(m_parent, "updateConnectionStatus", Qt::QueuedConnection,
                                      Q_ARG(int, count));
        }

       private:
        DdsManager* m_parent;  ///< 부모 DdsManager 포인터
    };

    // Private slots - DDS 스레드에서 Qt UI 스레드로 전달
    Q_INVOKABLE void handleIncomingPing(int seq, const QString& sender);
    Q_INVOKABLE void updateConnectionStatus(int writerCount);

    // DDS 엔티티 (스마트 포인터로 관리)
    std::unique_ptr<dds::domain::DomainParticipant> m_participant;
    std::unique_ptr<dds::pub::Publisher> m_publisher;
    std::unique_ptr<dds::sub::Subscriber> m_subscriber;
    std::unique_ptr<dds::topic::Topic<PingPongMessage>> m_pingTopic;
    std::unique_ptr<dds::topic::Topic<PingPongMessage>> m_pongTopic;
    std::unique_ptr<dds::pub::DataWriter<PingPongMessage>> m_writer;
    std::unique_ptr<dds::sub::DataReader<PingPongMessage>> m_reader;

    std::shared_ptr<PingListener> m_listener;  ///< DDS 리스너

    // UI 상태
    QString m_status{"Disconnected"};  ///< 현재 연결 상태
    QStringList m_messages;            ///< 로그 메시지 목록
    std::mutex m_mutex;                ///< 스레드 동기화
    int m_connectedWriters{0};         ///< 연결된 Writer 수
};

#endif  // DDSMANAGER_HPP
