#!/bin/bash
# RTI Ping-Pong 빌드 스크립트

echo "--- [빌드 시작] IDL 소스 코드 생성 ---"
# -ppDisable: 전처리기를 찾지 못하는 경우를 대비해 비활성화
# -language C++11: 최신 C++ API 사용
"C:/Program Files/rti_connext_dds-7.5.0/bin/rtiddsgen.bat" -language C++11 -ppDisable -d gen ping_pong.idl

echo "--- [빌드 시작] CMake 설정 및 빌드 ---"
# build 디렉토리 생성 및 CMake 실행
mkdir -p build
cd build

# MSVC를 사용해 빌드 (Release 모드)
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

echo "--- [빌드 완료] ---"
