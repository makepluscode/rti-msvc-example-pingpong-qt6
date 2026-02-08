# RTI Connext DDS Ping-Pong with Qt6 QML

RTI Connext DDS 7.5.0과 Qt6 QML을 사용한 **Daemon-GUI 통신 예제**입니다.

## 주요 특징

- **Modern C++17**: 최신 C++ 표준 및 RTI DDS C++11 API 사용
- **Qt6 QML GUI**: 카드 기반의 깔끔한 UI로 통신 상태 실시간 모니터링
- **비동기 콜백**: `DataReaderListener`를 활용한 이벤트 기반 비동기 데이터 수신
- **연결 감지**: `on_subscription_matched` 콜백으로 Daemon 연결/해제 실시간 감지
- **린트 적용**: MSVC `/W4` 경고 레벨로 코드 품질 확보

## 기술 스택

| 항목 | 버전/설명 |
|------|----------|
| **언어** | C++17 |
| **DDS** | RTI Connext DDS 7.5.0 |
| **GUI** | Qt 6.10.2 (QML/Quick) |
| **컴파일러** | MSVC (Visual Studio 2022) |
| **빌드** | CMake 3.16+ |

## 프로젝트 구조

```
rti-qt6-msvc-pingpong/
├── CMakeLists.txt              # 빌드 설정 (RTI DDS + Qt6)
├── ping_pong.idl               # DDS 데이터 타입 정의
├── USER_QOS_PROFILES.xml       # QoS 설정 (Reliable 통신)
├── daemon/
│   └── main.cpp                # 터미널 데몬 (Ping 발신, Pong 수신)
├── gui/
│   ├── main.cpp                # Qt 앱 진입점
│   ├── DdsManager.hpp/cpp      # DDS-Qt 브리지 클래스
│   └── Main.qml                # QML UI
├── gen/                        # rtiddsgen 생성 코드
├── build.ps1                   # 빌드 스크립트
└── run.ps1                     # 실행 스크립트
```

## 빌드 방법

### 사전 요구사항

- RTI Connext DDS 7.5.0 (`C:\Program Files\rti_connext_dds-7.5.0`)
- Qt 6.10.2 MSVC (`C:\Qt\6.10.2\msvc2022_64`)
- Visual Studio 2022
- CMake 3.16+

### 빌드

```powershell
# IDL 코드 생성 (최초 1회)
mkdir gen
& "C:/Program Files/rti_connext_dds-7.5.0/bin/rtiddsgen.bat" -language C++11 -ppDisable -d gen ping_pong.idl

# 빌드
./build.ps1
```

## 실행 방법

```powershell
./run.ps1
```

두 개의 애플리케이션이 실행됩니다:

1. **PingDaemon** (터미널): Ping 메시지를 1초 간격으로 전송
2. **PingGui** (Qt 창): Ping 수신 시 화면에 표시하고 Pong 자동 응답

## 통신 흐름

```
[Daemon]                      [Qt GUI]
    |                             |
    |--- Ping (seq: 1) -------->  |
    |                             |--- 화면에 표시
    |<-- Pong (seq: 1) ---------|
    |                             |
    |--- Ping (seq: 2) -------->  |
    ...
```

## 연결 상태 감지

- **Daemon 실행 시**: GUI에 "Connected (1 writer)" 표시
- **Daemon 종료 시**: GUI에 "Daemon disconnected" 표시

## 상세 설계

[ARCHITECTURE.md](./ARCHITECTURE.md) 참조.

## 라이선스

이 프로젝트는 RTI Connext DDS 라이선스 정책을 따릅니다.
