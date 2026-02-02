#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "manager.h"

#ifdef USE_KYLIN_SDK
#include <kpushbutton.h>
using ButtonType = kdk::KPushButton;
#else
using ButtonType = QPushButton;
#endif

class ContainerCard : public QFrame {
    Q_OBJECT

public:
    explicit ContainerCard(const Container &container, QWidget *parent = nullptr);

signals:
    void enterClicked(const QString &name);
    void stopClicked(const QString &name);
    void deleteClicked(const QString &name);

private:
    void setupUi(const Container &container);
    QString getDistroIcon(const QString &iconName);
    
    Container m_container;
    QLabel *m_iconLabel;
    QLabel *m_nameLabel;
    QLabel *m_statusLabel;
    QLabel *m_imageLabel;
    ButtonType *m_enterBtn;
    ButtonType *m_stopBtn;
    ButtonType *m_deleteBtn;
};
