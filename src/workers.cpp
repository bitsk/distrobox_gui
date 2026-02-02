#include "workers.h"
#include <QDebug>

// CreateBoxWorker implementation
CreateBoxWorker::CreateBoxWorker(DistroboxManager *manager,
                                 const QString &name,
                                 const QString &image,
                                 const QString &homePath,
                                 const QString &volume,
                                 bool root)
    : QObject(nullptr),
      m_manager(manager),
      m_name(name),
      m_image(image),
      m_homePath(homePath),
      m_volume(volume),
      m_root(root)
{
}

void CreateBoxWorker::run() {
    qDebug() << "CreateBoxWorker: Starting container creation in background thread";
    
    try {
        bool success = m_manager->createBox(m_name, m_image, m_homePath, m_volume, m_root);
        
        if (success) {
            emit finished(true, QString("容器 '%1' 创建成功！").arg(m_name));
        } else {
            emit finished(false, "创建容器失败。\n请检查镜像名称是否正确。");
        }
    } catch (...) {
        emit finished(false, "创建容器时发生未知错误。");
    }
    
    qDebug() << "CreateBoxWorker: Finished";
}

// InstallWorker implementation
InstallWorker::InstallWorker(DistroboxManager *manager)
    : QObject(nullptr),
      m_manager(manager)
{
}

void InstallWorker::run() {
    qDebug() << "InstallWorker: Starting installation in background thread";
    
    bool success = m_manager->installDistrobox();
    
    qDebug() << "InstallWorker: Installation" << (success ? "succeeded" : "failed");
    emit finished(success);
}
