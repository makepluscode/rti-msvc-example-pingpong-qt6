# RTI Ping-Pong 실행 스크립트

# 라이선스 파일 경로 설정
export RTI_LICENSE_FILE="C:/Program Files/rti_connext_dds-7.5.0/rti_license.dat"

echo "--- [실행] app1과 app2를 동시에 시작합니다 ---"
echo "--- (Ctrl+C를 누르면 종료됩니다) ---"

# app2를 백그라운드에서 실행 (Ping을 기다리는 쪽)
./build/Release/app2.exe &
APP2_PID=$!

# app1을 실행 (Ping을 먼저 보내는 쪽)
./build/Release/app1.exe

# app1이 종료되거나 중단되면 app2도 함께 종료
kill $APP2_PID 2>/dev/null
