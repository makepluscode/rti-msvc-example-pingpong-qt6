/**
 * @file main.cpp
 * @brief Daemon 애플리케이션 진입점
 *
 * 터미널에서 실행되며 Ping 메시지를 주기적으로 전송하고
 * Pong 응답을 비동기 콜백으로 수신합니다.
 *
 * @note C++17 표준을 사용합니다.
 */

#include <chrono>
#include <dds/dds.hpp>
#include <iostream>
#include <memory>
#include <thread>

#include "../gen/ping_pong.hpp"


namespace rti::pingpong {

// ============================================================================
// Pong 리스너 클래스
// ============================================================================

/**
 * @class PongListener
 * @brief Pong 토픽 수신을 위한 비동기 콜백 리스너
 *
 * on_data_available 콜백을 통해 Qt GUI로부터 Pong 응답을 수신합니다.
 */
class PongListener : public dds::sub::NoOpDataReaderListener<PingPongMessage> {
   public:
    /**
     * @brief 데이터 수신 콜백
     * @param reader 데이터를 수신한 DataReader
     */
    void on_data_available(dds::sub::DataReader<PingPongMessage>& reader) override {
        // 모든 가용 샘플을 take (버퍼에서 제거)
        auto samples = reader.take();

        for (const auto& sample : samples) {
            // 유효한 데이터인지 확인 (dispose/unregister가 아닌 경우)
            if (sample.info().valid()) {
                const auto& data = sample.data();
                std::cout << "[Daemon] Received Pong <- Sequence: " << data.sequence_num()
                          << " from " << data.sender_id() << std::endl;
            }
        }
    }
};

// ============================================================================
// Daemon 애플리케이션 클래스
// ============================================================================

/**
 * @class DaemonApp
 * @brief Ping-Pong 데몬 메인 클래스
 *
 * DDS 엔티티를 초기화하고 주기적으로 Ping을 전송합니다.
 */
class DaemonApp {
   public:
    /**
     * @brief 생성자 - DDS 초기화
     * @param domain_id DDS Domain ID (기본값: 0)
     */
    explicit DaemonApp(int domain_id = 0)
        : participant_(domain_id),
          publisher_(participant_),
          subscriber_(participant_),
          ping_topic_(participant_, "Ping"),
          pong_topic_(participant_, "Pong"),
          writer_(publisher_, ping_topic_),
          reader_(subscriber_, pong_topic_) {
        // Pong 수신을 위한 리스너 설정
        listener_ = std::make_shared<PongListener>();
        reader_.listener(listener_.get(), dds::core::status::StatusMask::data_available());

        std::cout << "[Daemon] DDS Initialized. "
                  << "Topic: Ping -> Send, Pong -> Receive" << std::endl;
    }

    /**
     * @brief 메인 실행 루프
     *
     * 1초 간격으로 Ping 메시지를 전송합니다.
     * Pong 수신은 리스너 콜백에서 비동기로 처리됩니다.
     */
    void run() {
        std::cout << "[Daemon] Starting Ping loop... (Press Ctrl+C to stop)" << std::endl;

        int sequence = 1;
        while (true) {
            send_ping(sequence++);

            // 1초 대기
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

   private:
    /**
     * @brief Ping 메시지 전송
     * @param seq 시퀀스 번호
     */
    void send_ping(int seq) {
        PingPongMessage msg("Daemon", seq);

        try {
            writer_.write(msg);
            std::cout << "[Daemon] Sent Ping -> Sequence: " << seq << std::endl;
        } catch (const dds::core::Exception& e) {
            std::cerr << "[Daemon] Failed to write data: " << e.what() << std::endl;
        }
    }

    // DDS 엔티티 멤버
    dds::domain::DomainParticipant participant_;
    dds::pub::Publisher publisher_;
    dds::sub::Subscriber subscriber_;
    dds::topic::Topic<PingPongMessage> ping_topic_;
    dds::topic::Topic<PingPongMessage> pong_topic_;
    dds::pub::DataWriter<PingPongMessage> writer_;
    dds::sub::DataReader<PingPongMessage> reader_;

    std::shared_ptr<PongListener> listener_;  ///< Pong 리스너
};

}  // namespace rti::pingpong

// ============================================================================
// Main 함수
// ============================================================================

/**
 * @brief 애플리케이션 진입점
 */
int main() {
    try {
        // Daemon 애플리케이션 생성 및 실행
        rti::pingpong::DaemonApp app(0);
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "[Daemon] Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
