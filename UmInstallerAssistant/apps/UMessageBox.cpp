#include "UMessageBox.h"
#include "Common.h"
#include <windowsx.h>
#include "QtCore"
#include "QtGui"
#include "configuration_option\GlobalConfiguration.h"
UMessageBox* UMessageBox::_instance = NULL;
UMessageBoxEx* UMessageBoxEx::_instance = NULL;
UMessageBox::UMessageBox(QWidget *parent) :
QDialog(parent, Qt::FramelessWindowHint | Qt::Tool)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    _ptOk = ui.messagebox_confirm->pos();
    _ptCancel = ui.messagebox_cancel->pos();
    QString stylesheet = Common::readStyleSheet(":/qss/umessageBox.qss");
    if (!stylesheet.isEmpty())
        setStyleSheet(stylesheet);
    
    connect(ui.messagebox_confirm, SIGNAL(clicked()), this, SLOT(confirmSlot()));
    connect(ui.messagebox_close, SIGNAL(clicked()), this, SLOT(cancelSlot()));
    connect(ui.messagebox_cancel, SIGNAL(clicked()), this, SLOT(cancelSlot()));
}

UMessageBox::~UMessageBox()
{
}

void UMessageBox::cancelSlot()
{
    hide();
    if (_waitLoop)
    {
        _waitLoop->exit(0);
        _waitLoop = NULL;
    }
}

void UMessageBox::confirmSlot()
{
    int ret = 0;
    hide();
    if (_waitLoop) {
     
        if (updateCfg()) {
            ret = 2;
        } else {
            ret = 1;
        }
        
        _waitLoop->exit(ret);
        _waitLoop = NULL;
    }
}

void UMessageBox::showNormal(QString text, unsigned int flag, QEventLoop *waitLoop)
{
    bool bOk = flag & UM_OK ? true : false;
    ui.messagebox_confirm->setVisible(bOk);
    bool bCancel = flag & UM_CANCEL ? true : false;
    ui.messagebox_cancel->setVisible(bCancel);
    bool b = flag & UM_REMEMBER ? true : false;
    ui.messagebox_ck->setVisible(b);
    ui.messagebox_ck->setChecked(false);
    if (bOk && !bCancel)
    {
        ui.messagebox_confirm->move(_ptCancel);
    }
    else
    {
        ui.messagebox_confirm->move(_ptOk);
    }
    _waitLoop = waitLoop; 
    ui.messagebox_label->setText(text);
    __super::showNormal();
}

bool UMessageBox::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);

    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message != WM_NCHITTEST)
    {
#if QT_VERSION < 0x050000  
        return QDialog::winEvent(message, result);
#else  
        return QDialog::nativeEvent("", message, result);
#endif  
    }
    return winEvent(msg, result);
}

bool UMessageBox::winEvent(MSG *message, long *result)
{
    switch (message->message)
    {
    case WM_NCHITTEST:
        //12 和21 为阴影部分去除
        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();
        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();
        if (yPos < 30 && yPos > 0)
        {
            QWidget *P = ui.centralWidget->childAt(xPos, yPos);
            if (ui.centralWidget->childAt(xPos, yPos) == NULL || ui.centralWidget->childAt(xPos, yPos) == ui.messagebox_title)
            {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
    }
    return true;
}

bool UMessageBox::updateCfg()
{
    bool b = ui.messagebox_ck->isChecked();
    QString val = !b ? "show" : "hide";
    GlobalIniCfg::getInstance()->setValue("del_package_warn_wnd", val);
    return b;
}

UMessageBoxEx::UMessageBoxEx(QWidget *parent):
QDialog(parent, Qt::FramelessWindowHint | Qt::Tool)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    QString stylesheet = Common::readStyleSheet(":/qss/umessageBoxex.qss");
    if (!stylesheet.isEmpty())
        setStyleSheet(stylesheet);
    connect(ui.messagebox_close, SIGNAL(clicked()), this, SLOT(exitSlot()));
    connect(ui.messagebox_cancel, SIGNAL(clicked()), this, SLOT(exitSlot()));
    connect(ui.continue_save, SIGNAL(clicked()), this, SLOT(exitSlot()));
}

UMessageBoxEx::~UMessageBoxEx()
{
}

void UMessageBoxEx::exitSlot()
{
    if (!_waitLoop) return;
    QPushButton *btn = static_cast<QPushButton *>(QObject::sender());
    if (btn == ui.continue_save)
    {
        _waitLoop->exit(1);
    } else if (btn == ui.messagebox_cancel || btn == ui.messagebox_cancel)
    {
        _waitLoop->exit(0);
    }
    hide();
}

bool UMessageBoxEx::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);

    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message != WM_NCHITTEST)
    {
#if QT_VERSION < 0x050000  
        return QDialog::winEvent(message, result);
#else  
        return QDialog::nativeEvent("", message, result);
#endif  
    }
    return winEvent(msg, result);
}

bool UMessageBoxEx::winEvent(MSG *message, long *result)
{
    switch (message->message)
    {
    case WM_NCHITTEST:
        //9 和4 为阴影部分去除
        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x() - 9;
        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y() - 4;
        if (yPos < 30 && yPos > 0)
        {
            QWidget *P = ui.centralWidget->childAt(xPos, yPos);
            if (ui.centralWidget->childAt(xPos, yPos) == NULL || ui.centralWidget->childAt(xPos, yPos) == ui.messagebox_title)
            {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
    }
    return true;
}

void UMessageBoxEx::showNormal(QString text1,QString text2 ,QEventLoop *waitLoop)
{
    _waitLoop = waitLoop;
    ui.messagebox_label_1->setText(text1);
    ui.messagebox_label_2->setText(text2);
    __super::showNormal();
}
