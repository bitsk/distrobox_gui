#pragma once

#include <QObject>
#include <QString>
#include "manager.h"

// Worker for creating containers in background thread
class CreateBoxWorker : public QObject {
    Q_OBJECT

public:
    explicit CreateBoxWorker(DistroboxManager *manager,
                            const QString &name,
                            const QString &image,
                            const QString &homePath = QString(),
                            const QString &volume = QString(),
                            bool root = false);

public slots:
    void run();

signals:
    void finished(bool success, const QString &message);

private:
    DistroboxManager *m_manager;
    QString m_name;
    QString m_image;
    QString m_homePath;
    QString m_volume;
    bool m_root;
};

// Worker for installing distrobox in background thread
class InstallWorker : public QObject {
    Q_OBJECT

public:
    explicit InstallWorker(DistroboxManager *manager);

public slots:
    void run();

signals:
    void finished(bool success);

private:
    DistroboxManager *m_manager;
};
