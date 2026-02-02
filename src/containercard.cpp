#include "containercard.h"
#include <QGraphicsDropShadowEffect>

ContainerCard::ContainerCard(const Container &container, QWidget *parent)
    : QFrame(parent), m_container(container) {
    setObjectName("ContainerCard");
    setupUi(container);
    
    // Add shadow effect for depth
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setXOffset(0);
    shadow->setYOffset(2);
    shadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(shadow);
}

QString ContainerCard::getDistroIcon(const QString &iconName) {
    // Unicode symbols for different distros
    if (iconName.contains("ubuntu")) return "🐧";
    if (iconName.contains("fedora")) return "🎩";
    if (iconName.contains("arch")) return "🏔️";
    if (iconName.contains("debian")) return "🌀";
    if (iconName.contains("alpine")) return "⛰️";
    if (iconName.contains("suse")) return "🦎";
    if (iconName.contains("kali")) return "🐉";
    if (iconName.contains("openkylin") || iconName.contains("kylin")) return "🔷";
    return "🐧"; // Default Linux penguin
}

void ContainerCard::setupUi(const Container &container) {
    // Determine if running
    bool isRunning = container.status.toLower().contains("up") || 
                     container.status.toLower().contains("running");
    
#ifdef USE_KYLIN_SDK
    // 在KylinSDK下使用绿色/红色，但不硬编码具体值
    QString statusColor = isRunning ? "#4CAF50" : "#F44336"; // Material Design colors
#else
    QString statusColor = isRunning ? "#98c379" : "#e06c75"; // One Dark colors
#endif
    
    QString statusText = isRunning ? "运行中" : "已停止";
    
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(15);
    
    // 1. Distro Icon
    m_iconLabel = new QLabel(this);
    QString iconText = getDistroIcon(container.image);
    m_iconLabel->setText(iconText);
    m_iconLabel->setStyleSheet("font-size: 48px;");
    m_iconLabel->setFixedSize(60, 60);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_iconLabel);
    
    // 2. Info Section
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);
    
    // Name
    m_nameLabel = new QLabel(container.name, this);
#ifdef USE_KYLIN_SDK
    m_nameLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
#else
    m_nameLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
#endif
    infoLayout->addWidget(m_nameLabel);
    
    // Details row (Status + Image)
    auto *detailsLayout = new QHBoxLayout();
    detailsLayout->setSpacing(10);
    
    // Status badge
    m_statusLabel = new QLabel(statusText, this);
    m_statusLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(statusColor));
    detailsLayout->addWidget(m_statusLabel);
    
    // Separator
    auto *sep = new QLabel("|", this);
#ifdef USE_KYLIN_SDK
    sep->setStyleSheet("color: palette(mid);");
#else
    sep->setStyleSheet("color: #5c6370;");
#endif
    detailsLayout->addWidget(sep);
    
    // Image name
    m_imageLabel = new QLabel(container.image, this);
#ifdef USE_KYLIN_SDK
    m_imageLabel->setStyleSheet("font-size: 13px;");
#else
    m_imageLabel->setStyleSheet("font-size: 13px; color: #abb2bf;");
#endif
    detailsLayout->addWidget(m_imageLabel);
    
    detailsLayout->addStretch();
    infoLayout->addLayout(detailsLayout);
    
    layout->addLayout(infoLayout);
    layout->addStretch();
    
    // 3. Actions Section
    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(8);
    
    int btnHeight = 38;
    
    // Enter button
    m_enterBtn = new ButtonType("进入终端", this);
    m_enterBtn->setFixedHeight(btnHeight);
    m_enterBtn->setMinimumWidth(100);
    m_enterBtn->setCursor(Qt::PointingHandCursor);
    connect(m_enterBtn, &QPushButton::clicked, [this]() {
        emit enterClicked(m_container.name);
    });
    
#ifdef USE_KYLIN_SDK
    // KPushButton styling
    // m_enterBtn->setBackgroundColorHighlight(true); // If this method exists and you want primary color
    // Use properties or stylesheets if SDK methods are not perfectly clear from truncated docs
    // The guide mentioned setBackgroundColorHighlight(bool)
    m_enterBtn->setBackgroundColorHighlight(true);
#else
    m_enterBtn->setObjectName("CreateBtn");
#endif
    actionsLayout->addWidget(m_enterBtn);
    
    // Stop button
    m_stopBtn = new ButtonType(this);
    m_stopBtn->setText("⏹");
    m_stopBtn->setToolTip("停止");
    m_stopBtn->setFixedSize(btnHeight, btnHeight);
    m_stopBtn->setCursor(Qt::PointingHandCursor);
    connect(m_stopBtn, &QPushButton::clicked, [this]() {
        emit stopClicked(m_container.name);
    });

#ifdef USE_KYLIN_SDK
    // m_stopBtn->setButtonType(kdk::ButtonType::CircleType); // Optional
    // m_stopBtn->setTranslucent(true);
#else
    m_stopBtn->setStyleSheet(
        "border-radius: 6px; "
        "background-color: rgba(255,255,255,0.05); "
        "border: 1px solid rgba(255,255,255,0.1); "
        "font-size: 16px;"
    );
#endif
    actionsLayout->addWidget(m_stopBtn);
    
    // Delete button
    m_deleteBtn = new ButtonType(this);
    m_deleteBtn->setText("🗑");
    m_deleteBtn->setToolTip("删除");
    m_deleteBtn->setFixedSize(btnHeight, btnHeight);
    m_deleteBtn->setCursor(Qt::PointingHandCursor);
    connect(m_deleteBtn, &QPushButton::clicked, [this]() {
        emit deleteClicked(m_container.name);
    });
    
#ifdef USE_KYLIN_SDK
    // m_deleteBtn->setButtonType(kdk::ButtonType::CircleType);
    // m_deleteBtn->setTranslucent(true);
#else
    m_deleteBtn->setStyleSheet(
        "border-radius: 6px; "
        "background-color: rgba(255,255,255,0.05); "
        "border: 1px solid rgba(255,255,255,0.1); "
        "font-size: 16px;"
    );
#endif
    actionsLayout->addWidget(m_deleteBtn);
    
    layout->addLayout(actionsLayout);
}