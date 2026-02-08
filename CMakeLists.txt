cmake_minimum_required(VERSION 3.16)
project(rti_pingpong_qt6)

# C++17 표준 사용 (PRD 권장사항)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC에서 한글 깨짐 방지 및 정적 분석(Lint) 적용
if(MSVC)
    add_compile_options(/utf-8 /W4)
endif()

# RTI Connext DDS 설치 경로 설정 (Windows 경로 주의: 슬래시 사용 권장)
set(CONNEXTDDS_DIR "C:/Program Files/rti_connext_dds-7.5.0")
list(APPEND CMAKE_MODULE_PATH "${CONNEXTDDS_DIR}/resource/cmake")

# RTI Connext DDS 패키지 찾기
set(CONNEXTDDS_ARCH "x64Win64VS2017")
find_package(RTIConnextDDS REQUIRED)

# Qt6 Setup
find_package(Qt6 REQUIRED COMPONENTS Gui Qml Quick)
qt_standard_project_setup()

# IDL에서 생성된 소스 코드 파일들
set(GEN_SOURCES
    gen/ping_pong.cxx
    gen/ping_pongPlugin.cxx
)

# 생성된 헤더 파일(.hpp)을 찾을 수 있도록 디렉토리 추가
include_directories(gen)

# -----------------------------------------------------------------------------
# Daemon 빌드 설정 (Ping 발행, Pong 구독) - 터미널 앱
# -----------------------------------------------------------------------------
add_executable(PingDaemon daemon/main.cpp ${GEN_SOURCES})
target_link_libraries(PingDaemon RTIConnextDDS::cpp2_api)

# -----------------------------------------------------------------------------
# Qt GUI 빌드 설정 (Ping 구독, Pong 발행) - QML 앱
# -----------------------------------------------------------------------------
qt_add_executable(PingGui 
    gui/main.cpp 
    gui/DdsManager.cpp 
    gui/DdsManager.hpp
    ${GEN_SOURCES}
)

qt_add_qml_module(PingGui
    URI pingpong
    VERSION 1.0
    QML_FILES gui/Main.qml
)

target_link_libraries(PingGui PRIVATE 
    Qt6::Gui 
    Qt6::Qml 
    Qt6::Quick
    RTIConnextDDS::cpp2_api
)

# Copy QoS profile to output directories
file(COPY USER_QOS_PROFILES.xml DESTINATION ${CMAKE_BINARY_DIR})
