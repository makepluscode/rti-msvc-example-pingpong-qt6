# Product Requirements Document (PRD): RTI Ping-Pong Example

## 1. 개요

- **목적**: RTI Connext DDS를 사용해 두 도메인 참여자(app1, app2) 간 **Ping**과 **Pong** 메시지를 주고받는 예제 제공.
- **참조**: [rticonnextdds-examples – Publish/Subscribe C++11](https://github.com/makepluscode/rticonnextdds-examples/tree/release/7.5.0/tutorials/publish_subscribe/c%2B%2B11) 튜토리얼을 최대한 활용.

---

## 2. 요구사항

### 2.1 도메인 참여자

- **app1**: Ping을 발행(Publish), Pong을 구독(Subscribe).
- **app2**: Ping을 구독(Subscribe), Pong을 발행(Publish).
- 동작: app1이 Ping 전송 → app2가 수신 후 Pong 전송 → app1이 Pong 수신. 터미널 로그로 Ping/Pong 흐름 확인 가능해야 함.

### 2.2 빌드 및 실행

- **빌드**: `build.sh` 실행 시 전체 프로젝트 빌드.
- **실행**: `run.sh` 실행 시 app1과 app2를 모두 실행하여, 한 터미널에서 두 앱 로그를 함께 확인 가능.

### 2.3 기술 스택

- **RTI Connext DDS**: 7.5.0 (설치 경로: `C:\Program Files\rti_connext_dds-7.5.0\`).
- **언어**: C++11 이상 (C++17 권장).
- **컴파일러**: MSVC.
- **코드**: 모든 코드에 이해하기 쉬운 주석 포함.

### 2.4 품질

- Lint 적용 유지.
- 요구사항을 반드시 준수하며, 다른 방식으로 대체(폴백)하지 않음.

---

## 3. 산출물

| 산출물 | 설명 |
|--------|------|
| `PRD.md` | 본 문서. |
| `ping_pong.idl` | Ping/Pong 메시지 타입 정의. |
| `app1_main.cxx` | app1: Ping 발행, Pong 구독, 터미널 로그 출력. |
| `app2_main.cxx` | app2: Ping 구독, Pong 발행, 터미널 로그 출력. |
| `CMakeLists.txt` | RTI 연동 및 app1/app2 빌드 설정. |
| `build.sh` | IDL 코드 생성 + CMake 설정 및 빌드. |
| `run.sh` | app1과 app2 동시 실행. |
| `USER_QOS_PROFILES.xml` | DDS QoS 프로파일(필요 시). |

---

## 4. 검증

- `build.sh` 실행 시 오류 없이 빌드 완료.
- `run.sh` 실행 시 app1 터미널 로그에 **Ping 전송** 및 **Pong 수신** 메시지, app2 터미널 로그에 **Ping 수신** 및 **Pong 전송** 메시지가 순서대로 출력되는지 확인.
