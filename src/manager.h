#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

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

    Q_INVOKABLE static bool isDistroboxInstalled();
    Q_INVOKABLE bool installDistrobox();
    Q_INVOKABLE QVariantList getContainersVariant(); // Renamed for clarity in QML, or overload
    
    // Helper to get raw list for C++ internal use if needed
    QList<Container> getContainers();

    Q_INVOKABLE void createBox(const QString &name, const QString &image, const QString &homePath, const QString &volume, bool root, const QVariantMap &extraParams = {});
    Q_INVOKABLE void cloneBox(const QString &name, const QString &newName);
    Q_INVOKABLE void upgradeBox(const QString &name);
    Q_INVOKABLE void exportApp(const QString &name, const QString &app, bool isBinary = false);
    Q_INVOKABLE bool stopBox(const QString &name);
    Q_INVOKABLE bool deleteBox(const QString &name);
    Q_INVOKABLE void enterBox(const QString &name); // Changed to void, handles process internally
    Q_INVOKABLE QString getIconName(const QString &imageName);
    Q_INVOKABLE QString getSystemArch();

signals:
    void createBoxFinished(bool success, QString message);
    void actionFinished(bool success, QString message);

private:
    QList<Container> parseTextOutput(const QString &output);
};
