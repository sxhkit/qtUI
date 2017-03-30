#pragma once
#include "qdialog.h"
#include "ui_UMessageBox.h"
#include "ui_UMessageBoxEx.h"
#include "Common.h"
#include "UmInstallerAssistant.h"
#include "qmutex.h"
extern QWidget *g_main_frame;
enum UMessageBoxFlag
{
    UM_OK = 1,
    UM_CANCEL = 2,
    UM_REMEMBER = 4,
};

class UMessageBox: public QDialog
{
public:
    Q_OBJECT
   UMessageBox(QWidget *parent = Q_NULLPTR);
public:
    static UMessageBox*getInstance()
    {
        if (!_instance)
        {
            _instance = new UMessageBox(g_main_frame);
            _instance->setModal(true);
        }
        return _instance;
    }
    virtual ~UMessageBox();
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    bool winEvent(MSG *message, long *result);
    void showNormal(QString text, unsigned int flag, QEventLoop *waitLoop = NULL);
private slots:
    void cancelSlot();
    void confirmSlot();

private:
    bool updateCfg();

private:
    Ui::UMessageBoxClass ui;
    QPoint _ptOk;
    QPoint _ptCancel;
    QEventLoop *_waitLoop;
    static UMessageBox *_instance;
};

class UMessageBoxEx:public QDialog
{
    Q_OBJECT
    UMessageBoxEx(QWidget *parent = Q_NULLPTR);
public:
    static UMessageBoxEx*getInstance()
    {
        if (!_instance)
        {
            _instance = new UMessageBoxEx(g_main_frame);
            _instance->setModal(true);
        }
        
        return _instance;
    }
    ~UMessageBoxEx();
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    bool winEvent(MSG *message, long *result);
    void showNormal(QString text1, QString text2, QEventLoop *waitLoop);
private slots:
    void exitSlot();
private:
    Ui::UMessageBoxExClass ui;
    QEventLoop *_waitLoop;
    static UMessageBoxEx *_instance;
};
