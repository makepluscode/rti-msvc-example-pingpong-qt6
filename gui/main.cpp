/**
 * @file main.cpp
 * @brief Qt GUI 애플리케이션 진입점
 *
 * QML 엔진을 초기화하고 DdsManager를 QML 컨텍스트에 등록합니다.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "DdsManager.hpp"

/**
 * @brief 애플리케이션 진입점
 * @param argc 명령줄 인자 개수
 * @param argv 명령줄 인자 배열
 * @return 종료 코드
 */
int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("RTI Ping-Pong Qt GUI");
    app.setOrganizationName("MakePlusCode");

    QQmlApplicationEngine engine;

    // DDS Manager 생성 및 QML 컨텍스트에 등록
    DdsManager ddsManager;
    engine.rootContext()->setContextProperty("ddsManager", &ddsManager);

    // QML 파일 로드
    engine.load(QUrl(QStringLiteral("qrc:/pingpong/gui/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML";
        return -1;
    }

    // DDS 초기화 시작
    ddsManager.start();

    return app.exec();
}
