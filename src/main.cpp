#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "manager.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setApplicationName("kylin-distrobox");
    app.setOrganizationName("Kylin");
    app.setWindowIcon(QIcon::fromTheme("applications-system"));

    qmlRegisterType<DistroboxManager>("Distrobox", 1, 0, "DistroboxManager");

    QQmlApplicationEngine engine;
    
    // Add local import path for fallback modules (e.g. ukui-quick from extra/)
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/imports");
    
    const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);

    return app.exec();
}
