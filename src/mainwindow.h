#pragma once

#ifdef USE_KYLIN_SDK
#include <kwidget.h>
#include <kiconbar.h>
#include <kwindowbuttonbar.h>
#include <kpushbutton.h>
using namespace kdk;
using BaseWidget = KWidget;
#else
#include <QWidget>
using BaseWidget = QWidget;
#endif

#include "manager.h"
#include "workers.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QProgressDialog>
#include <QThread>

class MainWindow : public BaseWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void refreshContainers();
    void createContainerDialog();
    void onEnterBox(const QString &name);
    void onStopBox(const QString &name);
    void onDeleteBox(const QString &name);
    void onCreateFinished(bool success, const QString &message);
    void onInstallDistrobox();
    void onInstallFinished(bool success);

private:
    void setupUi();
    void setupHeader();
    void setupLeftPanel();
    void setupRightPanel(QWidget *rightPanel);
    void showInstallUi();
    void applyStyles();
    void onDistroIconClicked(const QString &image, const QString &name);
    
    DistroboxManager *m_manager;
    
    // UI Components
    QVBoxLayout *m_mainLayout;
    QVBoxLayout *m_cardsLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_cardsContainer;
    QPushButton *m_refreshBtn;
    QPushButton *m_createBtn;
    
    // For async operations
    QProgressDialog *m_progressDialog;
    QThread *m_workerThread;
};
