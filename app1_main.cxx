#include <dds/dds.hpp>
#include <iostream>
#include <memory>

#include "ping_pong.hpp"

using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::sub;
using namespace dds::core;
using namespace dds::core::cond;

/**
 * @brief App1 클래스: Ping 발행 및 Pong 수신 담당
 */
class PingApp {
   public:
    PingApp(int domain_id)
        : participant_(domain_id),
          publisher_(participant_),
          subscriber_(participant_),
          ping_topic_(participant_, "Ping"),
          pong_topic_(participant_, "Pong"),
          writer_(publisher_, ping_topic_),
          reader_(subscriber_, pong_topic_) {
        // 1. 읽기 조건(ReadCondition) 생성: 유효한 데이터가 오면 트리거됨
        read_cond_ = ReadCondition(reader_, DataState::any());

        // 2. WaitSet에 조건 추가: 비동기 이벤트 대기를 위해 사용
        waitset_ += read_cond_;
    }

    void run() {
        std::cout << "--- [app1] 리팩토링 버전 시작 ---" << std::endl;

        int count = 1;
        while (true) {
            send_ping(count++);
            wait_for_pong();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

   private:
    void send_ping(int seq) {
        PingPongMessage msg("app1", seq);
        std::cout << "[app1] Ping 전송 (seq: " << seq << ")" << std::endl;
        writer_.write(msg);
    }

    void wait_for_pong() {
        // WaitSet을 사용하여 Pong이 올 때까지 최대 2초간 효율적으로 대기
        auto active_conditions = waitset_.wait(Duration::from_secs(2));

        if (active_conditions.empty()) {
            std::cout << "[app1] Pong 수신 실패 (타임아웃)" << std::endl;
            return;
        }

        // 데이터 읽기
        auto samples = reader_.take();
        for (const auto& sample : samples) {
            if (sample.info().valid()) {
                const auto& data = sample.data();
                std::cout << "[app1] Pong 수신 (sender: " << data.sender_id()
                          << ", seq: " << data.sequence_num() << ")" << std::endl;
            }
        }
    }

    // DDS 엔티티 멤버 변수들
    DomainParticipant participant_;
    Publisher publisher_;
    Subscriber subscriber_;
    Topic<PingPongMessage> ping_topic_;
    Topic<PingPongMessage> pong_topic_;
    DataWriter<PingPongMessage> writer_;
    DataReader<PingPongMessage> reader_;

    // 이벤트 처리용 객체
    WaitSet waitset_;
    ReadCondition read_cond_{nullptr};
};

int main() {
    try {
        PingApp app(0);
        app.run();
    } catch (const std::exception& ex) {
        std::cerr << "에러 발생: " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
