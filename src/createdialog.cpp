#include "createdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QScrollArea>
#include <QButtonGroup>

// DistroButton implementation
DistroButton::DistroButton(const DistroInfo &info, QWidget *parent)
    : QPushButton(parent), m_image(info.image) {
    
    setCheckable(true);
    setFixedSize(120, 140);
    setCursor(Qt::PointingHandCursor);
    
    // Create layout for icon and text
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignCenter);
    
    // Icon label
    auto *iconLabel = new QLabel(info.icon, this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 48px; background: transparent;");
    layout->addWidget(iconLabel);
    
    // Name label
    auto *nameLabel = new QLabel(info.displayName, this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    nameLabel->setStyleSheet("background: transparent; font-size: 12px;");
    layout->addWidget(nameLabel);
    
    // Hexagon-like styling
    setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: 3px solid #ff8c42;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   border-color: #ff6b1a;"
        "   background-color: rgba(255, 140, 66, 0.1);"
        "}"
        "QPushButton:checked {"
        "   border-color: #ff6b1a;"
        "   background-color: rgba(255, 140, 66, 0.2);"
        "}"
    );
}

CreateContainerDialog::CreateContainerDialog(QWidget *parent) : BaseDialog(parent) {
    setWindowTitle("新建 Distrobox 容器");
    setMinimumWidth(700);
    setMinimumHeight(600);
    setupUi();
}

QList<DistroInfo> CreateContainerDialog::getDistroList() {
    QList<DistroInfo> distros;
    
    distros << DistroInfo{"Ubuntu", "ubuntu:latest", "🐧", "Ubuntu"};
    distros << DistroInfo{"Ubuntu 22.04", "ubuntu:22.04", "🐧", "Ubuntu\n22.04"};
    distros << DistroInfo{"Fedora", "fedora:latest", "🎩", "Fedora"};
    distros << DistroInfo{"Arch Linux", "archlinux:latest", "🏔️", "Arch\nLinux"};
    distros << DistroInfo{"Debian", "debian:stable", "🌀", "Debian"};
    distros << DistroInfo{"openKylin", "openkylin/openkylin:2.0", "🔷", "openKylin\n2.0"};
    distros << DistroInfo{"Alpine", "alpine:latest", "⛰️", "Alpine"};
    distros << DistroInfo{"Kali Linux", "kalilinux/kali-rolling", "🐉", "Kali\nLinux"};
    
    return distros;
}

void CreateContainerDialog::setupUi() {
#ifdef USE_KYLIN_SDK
    QWidget *target = mainWidget();
#else
    QWidget *target = this;
#endif

    auto *mainLayout = new QVBoxLayout(target);
    mainLayout->setSpacing(20);
    
    // Title
    auto *titleLabel = new QLabel("新建开发环境", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #ff8c42;");
    mainLayout->addWidget(titleLabel);
    
    // Distro selection grid
    setupDistroGrid();
    mainLayout->addWidget(new QLabel("选择发行版:", this));
    
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumHeight(300);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    auto *gridContainer = new QWidget();
    auto *gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(15);
    
    auto *buttonGroup = new QButtonGroup(this);
    
    QList<DistroInfo> distros = getDistroList();
    int row = 0, col = 0;
    const int columns = 4;
    
    for (const auto &distro : distros) {
        auto *btn = new DistroButton(distro, this);
        buttonGroup->addButton(btn);
        gridLayout->addWidget(btn, row, col);
        
        connect(btn, &QPushButton::clicked, [this, distro]() {
            onDistroSelected(distro.image);
        });
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    gridContainer->setLayout(gridLayout);
    scrollArea->setWidget(gridContainer);
    mainLayout->addWidget(scrollArea);
    
    // Name input
    m_nameInput = new QLineEdit(this);
    m_nameInput->setPlaceholderText("例如: my-ubuntu-dev");
    addField(mainLayout, "容器名称:", m_nameInput);
    
    // Advanced options separator
    auto *sepLabel = new QLabel("高级选项", this);
    sepLabel->setStyleSheet("font-weight: bold; color: #61afef; margin-top: 10px; font-size: 14px;");
    mainLayout->addWidget(sepLabel);
    
    // Home directory with Browse button
    m_homeInput = new QLineEdit(this);
    m_homeInput->setPlaceholderText("/path/to/custom/home");
    
    auto *homeLayout = new QHBoxLayout();
    homeLayout->addWidget(m_homeInput);
    
    auto *btnBrowse = new QPushButton("浏览...", this);
    connect(btnBrowse, &QPushButton::clicked, this, &CreateContainerDialog::browseHome);
    homeLayout->addWidget(btnBrowse);
    
    addField(mainLayout, "自定义 Home (可选):", nullptr, homeLayout);
    
    // Volume mount
    m_volumeInput = new QLineEdit(this);
    m_volumeInput->setPlaceholderText("宿主路径:容器路径");
    addField(mainLayout, "挂载卷 (可选):", m_volumeInput);
    
    // Root checkbox
    m_rootCheck = new QCheckBox("使用 Root 权限 (Rootful)", this);
    mainLayout->addWidget(m_rootCheck);
    
    // Dialog buttons
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_buttonBox->button(QDialogButtonBox::Ok)->setText("创建");
    m_buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &CreateContainerDialog::validateAndAccept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    mainLayout->addWidget(m_buttonBox);
}

void CreateContainerDialog::setupDistroGrid() {
    // Grid is now set up in setupUi
}

void CreateContainerDialog::onDistroSelected(const QString &image) {
    m_selectedImage = image;
    
    // Auto-generate name suggestion
    QString suggestedName;
    if (image.contains("ubuntu")) {
        suggestedName = "my-ubuntu";
    } else if (image.contains("fedora")) {
        suggestedName = "my-fedora";
    } else if (image.contains("arch")) {
        suggestedName = "my-arch";
    } else if (image.contains("debian")) {
        suggestedName = "my-debian";
    } else if (image.contains("openkylin")) {
        suggestedName = "my-openkylin";
    } else if (image.contains("alpine")) {
        suggestedName = "my-alpine";
    } else if (image.contains("kali")) {
        suggestedName = "my-kali";
    }
    
    if (!suggestedName.isEmpty() && m_nameInput->text().isEmpty()) {
        m_nameInput->setText(suggestedName);
    }
}

void CreateContainerDialog::addField(QVBoxLayout *layout, const QString &labelText, 
                                      QWidget *widget, QHBoxLayout *customLayout) {
    auto *fieldLayout = new QVBoxLayout();
    fieldLayout->setSpacing(5);
    
    auto *label = new QLabel(labelText, this);
    label->setStyleSheet("font-weight: bold;");
    fieldLayout->addWidget(label);
    
    if (widget) {
        fieldLayout->addWidget(widget);
    } else if (customLayout) {
        fieldLayout->addLayout(customLayout);
    }
    
    layout->addLayout(fieldLayout);
}

void CreateContainerDialog::browseHome() {
    QString directory = QFileDialog::getExistingDirectory(this, "选择 Home 目录");
    if (!directory.isEmpty()) {
        m_homeInput->setText(directory);
    }
}

void CreateContainerDialog::validateAndAccept() {
    QString name = m_nameInput->text().trimmed();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "验证错误", "请输入容器名称。");
        return;
    }
    
    if (m_selectedImage.isEmpty()) {
        QMessageBox::warning(this, "验证错误", "请选择一个发行版。");
        return;
    }
    
    accept();
}

CreateDialogData CreateContainerDialog::getData() const {
    CreateDialogData data;
    data.name = m_nameInput->text().trimmed();
    data.image = m_selectedImage;
    data.homePath = m_homeInput->text().trimmed();
    data.volume = m_volumeInput->text().trimmed();
    data.root = m_rootCheck->isChecked();
    return data;
}