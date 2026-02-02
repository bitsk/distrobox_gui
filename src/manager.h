#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>

struct Container {
    QString id;
    QString name;
    QString status;
    QString image;
};

class DistroboxManager : public QObject {
    Q_OBJECT
public:
    explicit DistroboxManager(QObject *parent = nullptr);

    static bool isDistroboxInstalled();
    bool installDistrobox();
    QList<Container> getContainers();
    bool createBox(const QString &name, const QString &image, const QString &homePath = QString(), const QString &volume = QString(), bool root = false);
    bool stopBox(const QString &name);
    bool deleteBox(const QString &name);
    QStringList enterBoxCommand(const QString &name);
    QString getIconName(const QString &imageName);

private:
    QList<Container> parseTextOutput(const QString &output);
};
