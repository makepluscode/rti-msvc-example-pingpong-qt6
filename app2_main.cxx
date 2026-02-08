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
 * @brief App2 클래스: Ping 수신 시 즉시 Pong으로 응답하는 챌린저
 */
class PongApp {
   public:
    PongApp(int domain_id)
        : participant_(domain_id),
          publisher_(participant_),
          subscriber_(participant_),
          ping_topic_(participant_, "Ping"),
          pong_topic_(participant_, "Pong"),
          writer_(publisher_, pong_topic_),
          reader_(subscriber_, ping_topic_) {
        // 데이터 수신 시 알림을 받기 위한 조건 설정
        read_cond_ = ReadCondition(reader_, DataState::any());
        waitset_ += read_cond_;
    }

    void run() {
        std::cout << "--- [app2] 리팩토링 버전 시작 (Ping 대기 중) ---" << std::endl;

        while (true) {
            // 무한 대기 (데이터가 올 때까지 CPU를 점유하지 않음)
            waitset_.wait();

            auto samples = reader_.take();
            for (const auto& sample : samples) {
                if (sample.info().valid()) {
                    process_ping(sample.data());
                }
            }
        }
    }

   private:
    void process_ping(const PingPongMessage& incoming) {
        int seq = incoming.sequence_num();
        std::cout << "[app2] Ping 수신 (seq: " << seq << ")" << std::endl;

        // 즉시 응답 생성
        PingPongMessage response("app2", seq);
        std::cout << "[app2] Pong 전송 (seq: " << seq << ")" << std::endl;
        writer_.write(response);
    }

    DomainParticipant participant_;
    Publisher publisher_;
    Subscriber subscriber_;
    Topic<PingPongMessage> ping_topic_;
    Topic<PingPongMessage> pong_topic_;
    DataWriter<PingPongMessage> writer_;
    DataReader<PingPongMessage> reader_;

    WaitSet waitset_;
    ReadCondition read_cond_{nullptr};
};

int main() {
    try {
        PongApp app(0);
        app.run();
    } catch (const std::exception& ex) {
        std::cerr << "에러 발생: " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
