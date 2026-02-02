#include "manager.h"
#include <QProcess>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QDir>
#include <QFile>

DistroboxManager::DistroboxManager(QObject *parent) : QObject(parent) {}

bool DistroboxManager::isDistroboxInstalled() {
    return !QStandardPaths::findExecutable("distrobox").isEmpty();
}

bool DistroboxManager::installDistrobox() {
    qDebug() << "Installing distrobox to ~/.local...";
    
    // First check if curl is available
    QProcess curlCheck;
    curlCheck.start("which", QStringList() << "curl");
    curlCheck.waitForFinished();
    
    if (curlCheck.exitCode() != 0) {
        qDebug() << "ERROR: curl is not installed!";
        return false;
    }
    
    qDebug() << "curl is available, proceeding with installation...";
    
    // Download the installation script first
    QProcess download;
    download.start("curl", QStringList() 
        << "-L" 
        << "-o" 
        << "/tmp/distrobox-install.sh"
        << "https://raw.githubusercontent.com/89luca89/distrobox/main/install");
    
    if (!download.waitForFinished(60000)) {
        qDebug() << "Download timeout";
        return false;
    }
    
    if (download.exitCode() != 0) {
        qDebug() << "Download failed:" << download.readAllStandardError();
        return false;
    }
    
    qDebug() << "Download completed, running installation script...";
    
    // Make script executable and run it
    QProcess chmod;
    chmod.start("chmod", QStringList() << "+x" << "/tmp/distrobox-install.sh");
    chmod.waitForFinished();
    
    // Run the installation script with proper output capture
    QProcess install;
    install.setProcessChannelMode(QProcess::MergedChannels);
    
    // Use sh to expand ~
    QString homeDir = QDir::homePath();
    install.start("sh", QStringList() 
        << "/tmp/distrobox-install.sh"
        << "--prefix"
        << homeDir + "/.local");
    
    qDebug() << "Waiting for installation to complete (may take 1-2 minutes)...";
    
    if (!install.waitForFinished(180000)) { // 3 minutes timeout
        qDebug() << "Installation timeout";
        return false;
    }
    
    int exitCode = install.exitCode();
    QString output = install.readAll();
    
    qDebug() << "Install exit code:" << exitCode;
    qDebug() << "Install output length:" << output.length();
    
    if (!output.isEmpty()) {
        qDebug() << "Install output (first 500 chars):" << output.left(500);
    }
    
    // Cleanup
    QFile::remove("/tmp/distrobox-install.sh");
    
    // Verify installation by checking if distrobox binary exists
    QString distroboxPath = homeDir + "/.local/bin/distrobox";
    bool installed = QFile::exists(distroboxPath);
    
    qDebug() << "Distrobox binary exists at" << distroboxPath << ":" << installed;
    
    if (!installed) {
        qDebug() << "ERROR: Installation reported success but binary not found!";
        qDebug() << "Full output:" << output;
    }
    
    return installed;
}

QList<Container> DistroboxManager::getContainers() {
    if (!isDistroboxInstalled()) {
        return {};
    }

    QProcess process;
    process.start("distrobox", QStringList() << "list" << "--json");
    process.waitForFinished();

    QList<Container> containers;
    if (process.exitCode() == 0) {
        QByteArray output = process.readAllStandardOutput();
        if (!output.trimmed().isEmpty()) {
             QJsonDocument doc = QJsonDocument::fromJson(output);
             if (doc.isArray()) {
                 QJsonArray array = doc.array();
                 for (const QJsonValue &val : array) {
                     QJsonObject obj = val.toObject();
                     Container c;
                     c.id = obj["ID"].toString();
                     c.name = obj["NAME"].toString();
                     c.status = obj["STATUS"].toString();
                     c.image = obj["IMAGE"].toString();
                     containers.append(c);
                 }
                 return containers;
             }
        }
    }

    // Fallback to text parsing
    process.start("distrobox", QStringList() << "list" << "--no-color");
    process.waitForFinished();
    if (process.exitCode() == 0) {
        return parseTextOutput(process.readAllStandardOutput());
    }

    return {};
}

QList<Container> DistroboxManager::parseTextOutput(const QString &output) {
    QList<Container> containers;
    QStringList lines = output.trimmed().split('\n');
    if (lines.size() < 2) return containers;

    // Skip header
    for (int i = 1; i < lines.size(); ++i) {
        QStringList parts = lines[i].split('|');
        if (parts.size() >= 3) {
            Container c;
            c.id = parts[0].trimmed();
            c.name = parts[1].trimmed();
            c.status = parts[2].trimmed();
            c.image = (parts.size() > 3) ? parts[3].trimmed() : "unknown";
            containers.append(c);
        }
    }
    return containers;
}

QString DistroboxManager::getIconName(const QString &imageName) {
    QString lowerName = imageName.toLower();
    if (lowerName.contains("openkylin") || lowerName.contains("kylin")) return "openkylin";
    if (lowerName.contains("ubuntu")) return "ubuntu";
    if (lowerName.contains("fedora")) return "fedora";
    if (lowerName.contains("arch")) return "archlinux";
    if (lowerName.contains("debian")) return "debian";
    if (lowerName.contains("alpine")) return "alpine";
    if (lowerName.contains("suse")) return "opensuse";
    if (lowerName.contains("kali")) return "kali-linux";
    return "linux-generic";
}

bool DistroboxManager::createBox(const QString &name, const QString &image, const QString &homePath, const QString &volume, bool root) {
    QStringList args;
    args << "create" << "-n" << name << "-i" << image << "-Y";
    if (!homePath.isEmpty()) args << "--home" << homePath;
    if (!volume.isEmpty()) args << "--volume" << volume;
    if (root) args << "--root";

    return QProcess::execute("distrobox", args) == 0;
}

bool DistroboxManager::stopBox(const QString &name) {
    return QProcess::execute("distrobox", QStringList() << "stop" << name << "-Y") == 0;
}

bool DistroboxManager::deleteBox(const QString &name) {
    return QProcess::execute("distrobox", QStringList() << "rm" << name << "-Y") == 0;
}

QStringList DistroboxManager::enterBoxCommand(const QString &name) {
    return QStringList() << "distrobox" << "enter" << name;
}
