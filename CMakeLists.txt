cmake_minimum_required(VERSION 3.15)
project(rti_ping_pong)

# C++17 표준 사용 (PRD 권장사항)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC에서 한글 깨짐 방지 및 정적 분석(Lint) 적용
if(MSVC)
    add_compile_options(/utf-8 /W4 /analyze)
endif()

# RTI Connext DDS 설치 경로 설정 (Windows 경로 주의: 슬래시 사용 권장)
set(CONNEXTDDS_DIR "C:/Program Files/rti_connext_dds-7.5.0")
list(APPEND CMAKE_MODULE_PATH "${CONNEXTDDS_DIR}/resource/cmake")

# RTI Connext DDS 패키지 찾기
set(CONNEXTDDS_ARCH "x64Win64VS2017")
find_package(RTIConnextDDS REQUIRED)

# IDL에서 생성된 소스 코드 파일들
set(GEN_SOURCES 
    gen/ping_pong.cxx
    gen/ping_pongPlugin.cxx
)

# 생성된 헤더 파일(.hpp)을 찾을 수 있도록 디렉토리 추가
include_directories(gen)

# -----------------------------------------------------------------------------
# app1 빌드 설정 (Ping 발행, Pong 구독)
# -----------------------------------------------------------------------------
add_executable(app1 app1_main.cxx ${GEN_SOURCES})
target_link_libraries(app1 RTIConnextDDS::cpp2_api)

# -----------------------------------------------------------------------------
# app2 빌드 설정 (Ping 구독, Pong 발행)
# -----------------------------------------------------------------------------
add_executable(app2 app2_main.cxx ${GEN_SOURCES})
target_link_libraries(app2 RTIConnextDDS::cpp2_api)
