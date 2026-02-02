#pragma once

#ifdef USE_KYLIN_SDK
#include <kdialog.h>
using BaseDialog = kdk::KDialog;
#else
#include <QDialog>
using BaseDialog = QDialog;
#endif

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>

struct CreateDialogData {
    QString name;
    QString image;
    QString homePath;
    QString volume;
    bool root;
};

struct DistroInfo {
    QString name;
    QString image;
    QString icon;
    QString displayName;
};

class DistroButton : public QPushButton {
    Q_OBJECT
public:
    explicit DistroButton(const DistroInfo &info, QWidget *parent = nullptr);
    QString getImage() const { return m_image; }
    
private:
    QString m_image;
};

class CreateContainerDialog : public BaseDialog {
    Q_OBJECT

public:
    explicit CreateContainerDialog(QWidget *parent = nullptr);
    CreateDialogData getData() const;

private slots:
    void validateAndAccept();
    void browseHome();
    void onDistroSelected(const QString &image);

private:
    void setupUi();
    void setupDistroGrid();
    void addField(QVBoxLayout *layout, const QString &labelText, QWidget *widget, QHBoxLayout *customLayout = nullptr);
    QList<DistroInfo> getDistroList();
    
    QString m_selectedImage;
    QLineEdit *m_nameInput;
    QLineEdit *m_homeInput;
    QLineEdit *m_volumeInput;
    QCheckBox *m_rootCheck;
    QDialogButtonBox *m_buttonBox;
};
