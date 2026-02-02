#include "mainwindow.h"
#include "containercard.h"
#include "createdialog.h"
#include "workers.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include <QDebug>
#include <QLayout>
#include <QApplication>
#include <QIcon>
#include <QInputDialog>
#include <QScrollArea>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent) 
    : BaseWidget(parent), 
      m_manager(new DistroboxManager(this)),
      m_progressDialog(nullptr),
      m_workerThread(nullptr) {
    
#ifdef USE_KYLIN_SDK
    // KWidget setup
    setWidgetName("Kylin Distrobox 管理器");
    setIcon("applications-system"); // Use a valid system icon name
    
    // Configure window button bar (min/max/close/menu)
    if (auto *bar = windowButtonBar()) {
        // You can customize the bar here if needed
        // For example, hide menu button if not used
        // bar->menuButton()->setVisible(false);
    }
#else
    setWindowTitle("Kylin Distrobox 管理器");
#endif
    
    setupUi();
    
    // Refresh list after a short delay to let window render first
    QTimer::singleShot(100, this, &MainWindow::refreshContainers);
}

void MainWindow::setupUi() {
    resize(1000, 600);
    setMinimumSize(900, 550);
    applyStyles();

#ifdef USE_KYLIN_SDK
    // === OpenKylin SDK Layout Strategy ===
    // KWidget has built-in sideBar() (left) and baseBar() (right/center)
    
    // 1. Setup Side Bar (Left Panel - Running Containers)
    QWidget *leftPanel = sideBar();
    // Default sidebar might need layout
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(15, 20, 15, 20);
    
    // Title & Refresh for Sidebar
    auto *sideHeader = new QHBoxLayout();
    auto *leftTitle = new QLabel("正在运行", leftPanel);
    leftTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    sideHeader->addWidget(leftTitle);
    
    // Refresh button (small icon style)
    m_refreshBtn = new QPushButton(leftPanel);
    m_refreshBtn->setIcon(QIcon::fromTheme("view-refresh"));
    m_refreshBtn->setToolTip("刷新列表");
    m_refreshBtn->setFixedSize(30, 30);
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setProperty("isIconBtn", true); // Custom property for styling
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshContainers);
    sideHeader->addWidget(m_refreshBtn);
    
    leftLayout->addLayout(sideHeader);
    
    // Setup the scroll area and container logic
    setupLeftPanel();
    leftLayout->addWidget(m_scrollArea);
    
    // 2. Setup Base Bar (Right Panel - New Environments)
    QWidget *rightPanel = baseBar();
    setupRightPanel(rightPanel);
    
#else
    // === Standard Qt Layout Strategy ===
    setWindowTitle("Kylin Distrobox 管理器");
    
    // Create main layout
    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(20);
    
    // Header with refresh button
    setupHeader();
    
    // Main content area - split into left and right panels
    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(20);
    
    // Left panel: Running containers
    auto *leftPanel = new QWidget(this);
    auto *leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanelLayout->setSpacing(15);
    leftPanelLayout->setContentsMargins(0, 0, 0, 0);
    
    auto *leftTitle = new QLabel("正在运行的环境", this);
    leftTitle->setStyleSheet("font-size: 16px; font-weight: bold;");
    leftPanelLayout->addWidget(leftTitle);
    
    setupLeftPanel();
    leftPanelLayout->addWidget(m_scrollArea);
    
    leftPanel->setMinimumWidth(350);
    leftPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    
    // Right panel: New environment icons  
    auto *rightPanel = new QWidget(this);
    rightPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setupRightPanel(rightPanel);
    
    // Add panels with stretch factors (1:2 ratio)
    contentLayout->addWidget(leftPanel, 1);
    contentLayout->addWidget(rightPanel, 2);
    
    m_mainLayout->addLayout(contentLayout);
    
    // Set layout
    if (layout() == nullptr) {
        setLayout(m_mainLayout);
    } else {
        QWidget *centralWidget = new QWidget(this);
        centralWidget->setLayout(m_mainLayout);
        if (QVBoxLayout *existingLayout = qobject_cast<QVBoxLayout*>(layout())) {
            existingLayout->setContentsMargins(0, 0, 0, 0);
            existingLayout->addWidget(centralWidget);
        } else if (QLayout *existingLayout = layout()) {
            existingLayout->addWidget(centralWidget);
        }
    }
#endif
}

void MainWindow::setupLeftPanel() {
    // Scrollable container list
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QScrollArea::NoFrame);
    m_scrollArea->setStyleSheet("QScrollArea { background-color: transparent; border: none; }");
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_cardsContainer = new QWidget();
    m_cardsContainer->setStyleSheet("background-color: transparent;");
    m_cardsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_cardsLayout = new QVBoxLayout(m_cardsContainer);
    m_cardsLayout->setAlignment(Qt::AlignTop);
    m_cardsLayout->setSpacing(15);
    m_cardsLayout->setContentsMargins(0, 0, 10, 0);
    
    m_scrollArea->setWidget(m_cardsContainer);
}

void MainWindow::setupRightPanel(QWidget *rightPanel) {
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(15);
#ifdef USE_KYLIN_SDK
    rightLayout->setContentsMargins(20, 20, 20, 20);
#else
    rightLayout->setContentsMargins(0, 0, 0, 0);
#endif
    
    // Title
    auto *rightTitle = new QLabel("新建开发环境", rightPanel);
    rightTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #ff8c42;");
    rightLayout->addWidget(rightTitle);
    
    // Scrollable distro grid
    auto *scrollArea = new QScrollArea(rightPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QScrollArea::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; border: none; }");
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    auto *gridContainer = new QWidget();
    gridContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    auto *gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(20);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    gridLayout->setContentsMargins(0, 0, 10, 0);
    
    // Distro list with icons
    struct DistroInfo {
        QString name;
        QString image;
        QString icon;
    };
    
    QList<DistroInfo> distros = {
        {"Ubuntu", "ubuntu:latest", "🐧"},
        {"Fedora", "fedora:latest", "🎩"},
        {"Arch", "archlinux:latest", "🏔️"},
        {"Debian", "debian:stable", "🌀"},
        {"openKylin", "openkylin/openkylin:2.0", "🔷"},
        {"Alpine", "alpine:latest", "⛰️"},
        {"Kali", "kalilinux/kali-rolling", "🐉"},
        {"Ubuntu 22.04", "ubuntu:22.04", "🐧"}
    };
    
    int row = 0, col = 0;
    const int columns = 4;
    
    for (const auto &distro : distros) {
        auto *btn = new QPushButton(gridContainer);
        btn->setFixedSize(100, 120);
        btn->setCursor(Qt::PointingHandCursor);
        
        auto *btnLayout = new QVBoxLayout(btn);
        btnLayout->setAlignment(Qt::AlignCenter);
        btnLayout->setSpacing(8);
        
        // Icon
        auto *iconLabel = new QLabel(distro.icon, btn);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setStyleSheet("font-size: 40px; background: transparent;");
        btnLayout->addWidget(iconLabel);
        
        // Name
        auto *nameLabel = new QLabel(distro.name, btn);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setWordWrap(true);
        nameLabel->setStyleSheet("background: transparent; font-size: 11px;");
        btnLayout->addWidget(nameLabel);
        
        btn->setStyleSheet(
            "QPushButton {"
            "   background-color: transparent;"
            "   border: 3px solid #ff8c42;"
            "   border-radius: 12px;"
            "}"
            "QPushButton:hover {"
            "   border-color: #ff6b1a;"
            "   background-color: rgba(255, 140, 66, 0.1);"
            "}"
        );
        
        connect(btn, &QPushButton::clicked, [this, distro]() {
            onDistroIconClicked(distro.image, distro.name);
        });
        
        gridLayout->addWidget(btn, row, col);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    gridContainer->setLayout(gridLayout);
    scrollArea->setWidget(gridContainer);
    rightLayout->addWidget(scrollArea);
}

void MainWindow::setupHeader() {
    auto *headerLayout = new QHBoxLayout();
    
    // Title section with icon
    auto *iconLabel = new QLabel(this);
    iconLabel->setText("📦");
    iconLabel->setStyleSheet("font-size: 28px;");
    headerLayout->addWidget(iconLabel);
    
    auto *titleLabel = new QLabel("Distrobox 管理器", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-left: 8px;");
    headerLayout->addWidget(titleLabel);
    
    headerLayout->addStretch();
    
    // Refresh button
    m_refreshBtn = new QPushButton("🔄 刷新", this);
    m_refreshBtn->setFixedHeight(36);
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #3e4451; "
        "   color: white; "
        "   border-radius: 6px; "
        "   padding: 8px 20px; "
        "   font-weight: bold; "
        "   border: none;"
        "} "
        "QPushButton:hover {"
        "   background-color: #4e5461;"
        "}"
    );
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshContainers);
    headerLayout->addWidget(m_refreshBtn);
    
    m_mainLayout->addLayout(headerLayout);
}

void MainWindow::applyStyles() {
#ifdef USE_KYLIN_SDK
    // Use minimal styles, let KWidget handle most things
    QString minimalStyle = R"(
        /* Refresh Button in Sidebar */
        QPushButton[isIconBtn="true"] {
            border: 1px solid palette(mid);
            border-radius: 4px;
            background: transparent;
        }
        QPushButton[isIconBtn="true"]:hover {
            background: palette(midlight);
        }
    )";
    setStyleSheet(minimalStyle);
#else
    // Standard Qt dark theme
    QString mainStyle = R"(
        * { font-family: "Noto Sans CJK SC", "Microsoft YaHei", sans-serif; }
        QWidget { background-color: #282c34; color: #abb2bf; }
        QFrame#ContainerCard { background-color: #2c313c; border-radius: 12px; border: 1px solid #3e4451; }
        QFrame#ContainerCard:hover { border: 1px solid #61afef; background-color: #323842; }
        /* ... more dark theme styles ... */
    )";
    setStyleSheet(mainStyle);
#endif
}

void MainWindow::refreshContainers() {
    qDebug() << "Refreshing container list...";
    
    if (!m_cardsLayout) return;
    
    // Clear existing cards
    while (m_cardsLayout->count()) {
        QLayoutItem *item = m_cardsLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    // Check if distrobox is installed
    if (!m_manager->isDistroboxInstalled()) {
        showInstallUi();
        return;
    }
    
    // Get containers
    QList<Container> containers = m_manager->getContainers();
    qDebug() << "Found" << containers.size() << "containers";
    
    if (containers.isEmpty()) {
        auto *emptyLabel = new QLabel("暂无运行的容器", this);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #888; font-size: 13px; margin-top: 20px;");
        m_cardsLayout->addWidget(emptyLabel);
        return;
    }
    
    // Create cards for each container
    for (const Container &container : containers) {
        auto *card = new ContainerCard(container, this);
        
        connect(card, &ContainerCard::enterClicked, this, &MainWindow::onEnterBox);
        connect(card, &ContainerCard::stopClicked, this, &MainWindow::onStopBox);
        connect(card, &ContainerCard::deleteClicked, this, &MainWindow::onDeleteBox);
        
        m_cardsLayout->addWidget(card);
    }
}

void MainWindow::onDistroIconClicked(const QString &image, const QString &name) {
    qDebug() << "Distro icon clicked:" << name << image;
    
    bool ok;
    QString containerName = QInputDialog::getText(this, "创建容器",
        QString("为 %1 容器输入名称:").arg(name),
        QLineEdit::Normal,
        QString("my-%1").arg(name.toLower().replace(" ", "-")), &ok);
    
    if (!ok || containerName.isEmpty()) {
        return;
    }
    
    // Show progress dialog
    m_progressDialog = new QProgressDialog(
        QString("正在创建容器 '%1'...\n这可能需要一些时间来拉取镜像。").arg(containerName),
        QString(), 0, 0, this);
    m_progressDialog->setWindowTitle("处理中");
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->show();
    
    // Create worker and thread for async operation
    m_workerThread = new QThread(this);
    CreateBoxWorker *worker = new CreateBoxWorker(
        m_manager, containerName, image, QString(), QString(), false);
    worker->moveToThread(m_workerThread);
    
    // Connect signals
    connect(m_workerThread, &QThread::started, worker, &CreateBoxWorker::run);
    connect(worker, &CreateBoxWorker::finished, this, &MainWindow::onCreateFinished);
    connect(worker, &CreateBoxWorker::finished, m_workerThread, &QThread::quit);
    connect(worker, &CreateBoxWorker::finished, worker, &CreateBoxWorker::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QThread::deleteLater);
    
    // Start thread
    m_workerThread->start();
}

void MainWindow::showInstallUi() {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    
    auto *icon = new QLabel("⚠️", this);
    icon->setStyleSheet("font-size: 64px;");
    icon->setAlignment(Qt::AlignCenter);
    layout->addWidget(icon);
    
    auto *label = new QLabel("未检测到 Distrobox", this);
#ifdef USE_KYLIN_SDK
    label->setStyleSheet("font-size: 20px; font-weight: bold;");
#else
    label->setStyleSheet("font-size: 20px; font-weight: bold; color: #e06c75;");
#endif
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    
    auto *desc = new QLabel(
        "Distrobox 尚未安装。\n它可以让您在终端中运行任何 Linux 发行版。", this);
#ifdef USE_KYLIN_SDK
    desc->setStyleSheet("font-size: 14px;");
#else
    desc->setStyleSheet("color: #abb2bf; font-size: 14px;");
#endif
    desc->setAlignment(Qt::AlignCenter);
    layout->addWidget(desc);
    
    auto *btn = new QPushButton("安装 Distrobox (~/.local)", this);
    btn->setObjectName("CreateBtn");
#ifndef USE_KYLIN_SDK
    btn->setStyleSheet("background-color: #98c379; border: none;");
#endif
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedWidth(250);
    btn->setFixedHeight(40);
    
    // Connect install signal
    connect(btn, &QPushButton::clicked, this, &MainWindow::onInstallDistrobox);
    
    layout->addWidget(btn);
    
    m_cardsLayout->addWidget(container);
}

void MainWindow::createContainerDialog() {
    CreateContainerDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        CreateDialogData data = dialog.getData();
        
        qDebug() << "Creating container:" << data.name << "with image:" << data.image;
        
        // Show progress dialog
        m_progressDialog = new QProgressDialog(
            QString("正在创建容器 '%1'...\n这可能需要一些时间来拉取镜像。").arg(data.name),
            QString(), 0, 0, this);
        m_progressDialog->setWindowTitle("处理中");
        m_progressDialog->setWindowModality(Qt::WindowModal);
        m_progressDialog->setMinimumDuration(0);
        m_progressDialog->show();
        
        // Create worker and thread for async operation
        m_workerThread = new QThread(this);
        CreateBoxWorker *worker = new CreateBoxWorker(
            m_manager, data.name, data.image, data.homePath, data.volume, data.root);
        worker->moveToThread(m_workerThread);
        
        // Connect signals
        connect(m_workerThread, &QThread::started, worker, &CreateBoxWorker::run);
        connect(worker, &CreateBoxWorker::finished, this, &MainWindow::onCreateFinished);
        connect(worker, &CreateBoxWorker::finished, m_workerThread, &QThread::quit);
        connect(worker, &CreateBoxWorker::finished, worker, &CreateBoxWorker::deleteLater);
        connect(m_workerThread, &QThread::finished, m_workerThread, &QThread::deleteLater);
        
        // Start thread
        m_workerThread->start();
    }
}

void MainWindow::onEnterBox(const QString &name) {
    qDebug() << "Entering container:" << name;
    
    // List of terminals to try
    QStringList terminals = {
        "gnome-terminal", "mate-terminal", "xfce4-terminal", 
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
        QMessageBox::warning(this, "错误", "未检测到支持的终端模拟器。");
        return;
    }
    
    // Build command
    QStringList cmd = m_manager->enterBoxCommand(name);
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
    
    // Refresh after 3 seconds
    QTimer::singleShot(3000, this, &MainWindow::refreshContainers);
}

void MainWindow::onStopBox(const QString &name) {
    auto reply = QMessageBox::question(this, "确认", 
        QString("确定要停止容器 '%1' 吗？").arg(name),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        qDebug() << "Stopping container:" << name;
        m_manager->stopBox(name);
        QTimer::singleShot(1000, this, &MainWindow::refreshContainers);
    }
}

void MainWindow::onDeleteBox(const QString &name) {
    auto reply = QMessageBox::question(this, "确认", 
        QString("确定要删除容器 '%1' 吗？\n此操作不可撤销！").arg(name),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        qDebug() << "Deleting container:" << name;
        m_manager->deleteBox(name);
        QTimer::singleShot(1000, this, &MainWindow::refreshContainers);
    }
}

void MainWindow::onCreateFinished(bool success, const QString &message) {
    if (m_progressDialog) {
        m_progressDialog->close();
        delete m_progressDialog;
        m_progressDialog = nullptr;
    }
    
    if (success) {
        QMessageBox::information(this, "成功", message);
        refreshContainers();
    } else {
        QMessageBox::critical(this, "错误", message);
    }
}

void MainWindow::onInstallDistrobox() {
    qDebug() << "User requested distrobox installation";
    
    auto reply = QMessageBox::question(this, "确认安装",
        "将从官方源安装 Distrobox 到 ~/.local 目录。\n\n"
        "这可能需要几分钟时间。\n\n"
        "是否继续？",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // Show progress dialog
    m_progressDialog = new QProgressDialog(
        "正在安装 Distrobox...\n请稍候，这可能需要几分钟。",
        QString(), 0, 0, this);
    m_progressDialog->setWindowTitle("安装中");
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setMinimumDuration(0);
    m_progressDialog->setCancelButton(nullptr);
    m_progressDialog->show();
    
    // Create worker and thread for async installation
    m_workerThread = new QThread(this);
    InstallWorker *worker = new InstallWorker(m_manager);
    worker->moveToThread(m_workerThread);
    
    // Connect signals
    connect(m_workerThread, &QThread::started, worker, &InstallWorker::run);
    connect(worker, &InstallWorker::finished, this, &MainWindow::onInstallFinished);
    connect(worker, &InstallWorker::finished, m_workerThread, &QThread::quit);
    connect(worker, &InstallWorker::finished, worker, &InstallWorker::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QThread::deleteLater);
    
    // Start thread
    m_workerThread->start();
}

void MainWindow::onInstallFinished(bool success) {
    if (m_progressDialog) {
        m_progressDialog->close();
        delete m_progressDialog;
        m_progressDialog = nullptr;
    }
    
    if (success) {
        QMessageBox::information(this, "安装成功",
            "Distrobox 已成功安装到 ~/.local 目录！\n\n"
            "请确保 ~/.local/bin 在您的 PATH 环境变量中。\n"
            "您可能需要重新打开终端或重新登录。\n\n"
            "现在刷新列表...");
        
        // Refresh to check if distrobox is now available
        QTimer::singleShot(500, this, &MainWindow::refreshContainers);
    } else {
        QMessageBox::critical(this, "安装失败",
            "Distrobox 安装失败。\n\n"
            "可能的原因：\n"
            "- 网络连接问题\n"
            "- 缺少必要的依赖（如 curl）\n"
            "- 权限不足\n\n"
            "请检查终端输出获取更多信息。",
            QMessageBox::Ok);
    }
}