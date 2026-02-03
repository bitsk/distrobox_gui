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

QVariantList DistroboxManager::getContainersVariant() {
    QList<Container> containers = getContainers();
    QVariantList list;
    for (const auto &c : containers) {
        QVariantMap map;
        map["id"] = c.id;
        map["name"] = c.name;
        map["status"] = c.status;
        map["image"] = c.image;
        map["isRunning"] = c.status.toLower().contains("up") || c.status.toLower().contains("running");
        list.append(map);
    }
    return list;
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

#include <QThread>

// ... (existing code)

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

void DistroboxManager::createBox(const QString &name, const QString &image, const QString &homePath, const QString &volume, bool root, const QVariantMap &extraParams) {
    QThread *thread = QThread::create([=]() {
        QStringList args;
        args << "create" << "-n" << name << "-i" << image << "-Y";
        if (!homePath.isEmpty()) args << "--home" << homePath;
        if (!volume.isEmpty()) args << "--volume" << volume;
        if (root) args << "--root";
        
        // Handle extra params
        if (extraParams.value("init").toBool()) args << "--init";
        if (extraParams.value("nvidia").toBool()) args << "--nvidia";
        
        QString platform = extraParams.value("platform").toString();
        if (!platform.isEmpty()) args << "--platform" << platform;
        
        QString packages = extraParams.value("packages").toString();
        if (!packages.isEmpty()) args << "--additional-packages" << packages;
        
        QString flags = extraParams.value("flags").toString();
        if (!flags.isEmpty()) {
            // Split flags by space but respect quotes? Simple split for now.
            // Distrobox expects one --additional-flags argument or multiple?
            // "additional flags to pass to the container manager command"
            // Usually passed as one string: --additional-flags "--env foo=bar"
            args << "--additional-flags" << flags;
        }

        QProcess process;
        process.start("distrobox", args);
        process.waitForFinished(-1); // Wait indefinitely

        bool success = (process.exitCode() == 0);
        QString message = success ? "Creation successful" : QString::fromUtf8(process.readAllStandardError());
        
        emit createBoxFinished(success, message);
    });
    
    // Auto-delete thread when finished
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void DistroboxManager::cloneBox(const QString &name, const QString &newName) {
    QThread *thread = QThread::create([=]() {
        QStringList args;
        args << "create" << "--clone" << name << "--name" << newName << "-Y";

        QProcess process;
        process.start("distrobox", args);
        process.waitForFinished(-1);

        bool success = (process.exitCode() == 0);
        QString message = success ? "Clone successful" : QString::fromUtf8(process.readAllStandardError());
        
        emit actionFinished(success, message);
    });
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

// Re-implementing upgradeBox to launch terminal
/* 
   Wait, if I change the signature in .h to just launch terminal, it returns void. 
   But I already declared it. Let's make it launch terminal.
*/
void DistroboxManager::upgradeBox(const QString &name) {
    // Similar to enterBox but runs upgrade
    // List of terminals to try (reuse code or refactor? For now duplicate for safety/speed)
    QStringList terminals = {
        "kylin-terminal", "mate-terminal", "gnome-terminal", "xfce4-terminal", 
        "terminator", "deepin-terminal", "qterminal",
        "tilix", "konsole", "alacritty", "xterm", "kitty", "foot"
    };
    
    QString foundTerminal;
    for (const QString &term : terminals) {
        QProcess which;
        which.start("which", QStringList() << term);
        which.waitForFinished();
        if (which.exitCode() == 0) {
            foundTerminal = term;
            break;
        }
    }
    
    if (foundTerminal.isEmpty()) return;
    
    QStringList cmd;
    cmd << "distrobox" << "upgrade" << name; // This runs upgrade script
    QString flatCmd = cmd.join(" ");
    QString shellCmd = QString("%1; echo 'Upgrade finished.'; read -p 'Press Enter to close...' var;").arg(flatCmd);
    
    QStringList finalCmd;
    finalCmd << foundTerminal;
    
    if (foundTerminal.contains("gnome") || foundTerminal.contains("mate") || 
        foundTerminal.contains("xfce") || foundTerminal == "tilix") {
        finalCmd << "--";
    } else if (foundTerminal == "konsole" || foundTerminal == "alacritty" || foundTerminal == "xterm") {
        finalCmd << "-e";
    }
    
    finalCmd << "bash" << "-c" << shellCmd;
    QProcess::startDetached(finalCmd.first(), finalCmd.mid(1));
}

void DistroboxManager::exportApp(const QString &name, const QString &app, bool isBinary) {
    QThread *thread = QThread::create([=]() {
        // Command: distrobox enter name -- distrobox-export --app/--bin app
        QStringList args;
        args << "enter" << name << "--" << "distrobox-export";
        if (isBinary) {
            args << "--bin" << app << "--export-path" << QDir::homePath() + "/.local/bin";
        } else {
            args << "--app" << app;
        }

        QProcess process;
        process.start("distrobox", args);
        process.waitForFinished(-1);

        bool success = (process.exitCode() == 0);
        // Capture stdout/stderr
        QString output = QString::fromUtf8(process.readAllStandardOutput());
        QString err = QString::fromUtf8(process.readAllStandardError());
        QString message = success ? ("Export successful: " + output) : ("Export failed: " + err);
        
        emit actionFinished(success, message);
    });
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

bool DistroboxManager::stopBox(const QString &name) {
    return QProcess::execute("distrobox", QStringList() << "stop" << name << "-Y") == 0;
}

bool DistroboxManager::deleteBox(const QString &name) {
    return QProcess::execute("distrobox", QStringList() << "rm" << name << "-Y") == 0;
}

void DistroboxManager::enterBox(const QString &name) {
    // List of terminals to try
    QStringList terminals = {
        "kylin-terminal", "mate-terminal", "gnome-terminal", "xfce4-terminal", 
        "terminator", "deepin-terminal", "qterminal",
        "tilix", "konsole", "alacritty", "xterm", "kitty", "foot"
    };
    
    QString foundTerminal;
    for (const QString &term : terminals) {
        QProcess which;
        which.start("which", QStringList() << term);
        which.waitForFinished();
        if (which.exitCode() == 0) {
            foundTerminal = term;
            break;
        }
    }
    
    if (foundTerminal.isEmpty()) {
        qDebug() << "No supported terminal found.";
        return;
    }
    
    // Build command
    QStringList cmd;
    cmd << "distrobox" << "enter" << name;
    QString flatCmd = cmd.join(" ");
    QString shellCmd = QString("%1 || { echo 'Command failed.'; read -p 'Press Enter to close...' var; }").arg(flatCmd);
    
    QStringList finalCmd;
    finalCmd << foundTerminal;
    
    // Add terminal-specific flags
    if (foundTerminal.contains("gnome") || foundTerminal.contains("mate") || 
        foundTerminal.contains("xfce") || foundTerminal == "tilix") {
        finalCmd << "--";
    } else if (foundTerminal == "konsole" || foundTerminal == "alacritty" || foundTerminal == "xterm") {
        finalCmd << "-e";
    }
    
    finalCmd << "bash" << "-c" << shellCmd;
    
    qDebug() << "Launching terminal:" << finalCmd;
    QProcess::startDetached(finalCmd.first(), finalCmd.mid(1));
}

QString DistroboxManager::getSystemArch() {
    return QSysInfo::currentCpuArchitecture();
}
