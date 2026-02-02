#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[]) {
    // Enable high DPI scaling
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QApplication app(argc, argv);
    app.setApplicationName("kylin-distrobox");
    app.setApplicationDisplayName("Kylin Distrobox 管理器");
    app.setDesktopFileName("kylin-distrobox");

    qDebug() << "Starting Kylin Distrobox application";
    
#ifdef USE_KYLIN_SDK
    qDebug() << "Using OpenKylin SDK (KWidget)";
#else
    qDebug() << "Using standard Qt5 widgets";
#endif

    // Translation loading logic (suggested by OpenKylin SDK docs)
    QTranslator trans;
    QString locale = QLocale::system().name();
    if(locale == "zh_CN") {
        if(trans.load(":/translations/gui_zh_CN.qm")) {
            app.installTranslator(&trans);
        }
    }

    MainWindow window;
    qDebug() << "MainWindow created, showing window...";
    window.show();
    qDebug() << "Window shown, entering event loop";

    return app.exec();
}
