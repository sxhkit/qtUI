//  Copyright (C)  WSWY
//
//  File:       BatchListItem.cpp
//
//  History:    2017-Jan-1-22   Xhsund  Created
//
//--------------------------------------------------------------------------
#include "BatchListItem.h"
#include <qfile.h>
#include "QtCore"
#include "QtGui"
#include "Common.h"
#include "BatchListItem.h"
#include "SignalManager.h"
CBatchListItem::CBatchListItem(ListItemType nType, QWidget *parent)
    : QRadioButton(parent) {
    ui.setupUi(this);
    QString styleSheet = Common::readStyleSheet(":/qss/batchListItem.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
    initItemInfo(nType);
}

void CBatchListItem::initItemInfo(ListItemType nType) {
    QLabel *pLabel = findChild<QLabel *>("batch_package_type");
    if (!pLabel) return;

    if (nType == DEFAULT) {
        pLabel->setVisible(false);
    } else if (nType == HIGH_CONVERSION) {
        pLabel->setVisible(true);
        QPixmap pixmap(":/image/high_conversion");
        pLabel->setPixmap(pixmap);

    } else if (nType == HIGH_DOWNLOAD) {
        pLabel->setVisible(true);
        QPixmap pixmap(":/image/high_download");
        pLabel->setPixmap(pixmap);
    }

    pLabel = findChild<QLabel *>("batch_package_unique");
    if (!pLabel) return;
    pLabel->setVisible(false);
}

COnekeyListItem::COnekeyListItem(ListItemType nType, QWidget *parent)
    : QPushButton(parent) {
    ui.setupUi(this);
    QString styleSheet = Common::readStyleSheet(":/qss/onekeyListItem.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
    initItemInfo(nType);
}

COnekeyListItem::~COnekeyListItem() {
}

void COnekeyListItem::initItemInfo(ListItemType nType) {
    QLabel *pLabel = findChild<QLabel *>("onekey_listitem_type");
    QLabel *pLabel_desc = findChild<QLabel *>("onekey_listitem_type_desc");
    QPushButton *pDelete = findChild<QPushButton *>("onekey_listitem_delete");
    QPushButton *pModify = findChild<QPushButton *>("onekey_listitem_modify");
    QLabel *unique = findChild<QLabel *>("onekey_listitem_unique");
    if (!pLabel || !pLabel_desc || !pDelete ||!unique) return;

    unique->setVisible(false);
    if (nType == DEFAULT) {
        pLabel->setVisible(false);
        pDelete->setVisible(true);
        pLabel_desc->setVisible(false);
        pModify->setVisible(true);
    } else if (nType == HIGH_CONVERSION) {
        pLabel->setVisible(true);
        QPixmap pixmap(":/image/high_conversion");
        pLabel->setPixmap(pixmap);
        pDelete->setVisible(false);
        pLabel_desc->setVisible(true);
        pModify->setVisible(false);
    } else if (nType == HIGH_DOWNLOAD) {
        pLabel->setVisible(true);
        QPixmap pixmap(":/image/high_download");
        pLabel->setPixmap(pixmap);
        pDelete->setVisible(false);
        pLabel_desc->setVisible(true);
        pModify->setVisible(false);
    }

    QLabel *ckbox = findChild<QLabel *>("onekey_listitem_query_desc");
    if (!ckbox) return;
    ckbox->setText("展开查看");
    pLabel = findChild<QLabel *>("onekey_listitem_mask");
    if (!pLabel) return;
    pLabel->setVisible(false);
}

COnekeyListAddItem::COnekeyListAddItem(QWidget *parent) :
QPushButton(parent) {
    ui.setupUi(this);
    QString styleSheet = Common::readStyleSheet(":/qss/onekeyListAddItem.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
}

COnekeyListAddItem::~COnekeyListAddItem() {
}

CQueryItem::CQueryItem(RecommendSoft nType, QWidget *parent) :QWidget(parent) {
    ui.setupUi(this);
    QString styleSheet = Common::readStyleSheet(":/qss/queryItem.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
}

void CQueryItem::setIcon(QString icon) {
    QPixmap pix(icon);
    bool isSetIcon = true;
    if (pix.isNull())
    {
        QString format = Common::getImageFormat(icon);
        if (format == "ERR") return;
        if (!pix.load(icon, format.toStdString().c_str()))
            isSetIcon = false;
        else
            isSetIcon = true;

    }
    else {
        isSetIcon = true;
    }
    if (isSetIcon)
    {
        pix = pix.scaled(ui.item_icon->size(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui.item_icon->setPixmap(pix);
    }
}

void CQueryItem::setItemInfo(QString name, float fsize) {
    ui.item_name->setText(name);
    QString size = QString("%0MB").arg(QString::number(fsize, 'f', 2));
    ui.item_size->setText(size);
}

CQueryItem::~CQueryItem() {
}


CTaskListItem::CTaskListItem(QWidget *parent) :QWidget(parent) {
    ui.setupUi(this);
    QString styleSheet = Common::readStyleSheet(":/qss/taskListItem.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
    initItem();
    connectSignal2Slot();
}

CTaskListItem::~CTaskListItem() {
}

void CTaskListItem::initItem() {
    QLabel *pLabel = findChild<QLabel *>("TaskItem_Name");
    if (!pLabel) return;
    pLabel->setText(QString(""));

    pLabel = findChild<QLabel *>("TaskItem_Size");
    if (!pLabel) return;
    pLabel->setText("31.2MB");

    pLabel = findChild<QLabel *>("TaskItem_value");
    if (!pLabel) return;
    pLabel->setText("");

    pLabel = findChild<QLabel *>("TaskItem_State_icon");
    if (!pLabel) return;
    QPixmap pic(":/image/task_waitting_state");
    pLabel->setPixmap(pic);

    pLabel = findChild<QLabel *>("TaskItem_State_text");
    if (!pLabel) return;
    pLabel->setText("等待下载");

    pLabel = findChild<QLabel *>("TaskItem_Error");
    if (!pLabel) return;
    pLabel->setVisible(false);

    QPushButton *pBtn = findChild<QPushButton *>("TaskItem_TryBtn");
    pBtn->setText("重试");
    pBtn->setVisible(false);
    setProgressValueSlot(0);
}

void CTaskListItem::setItemStateSlot(SOFT_STATE nstate) {
    QLabel *pLabel = findChild<QLabel *>("TaskItem_State_icon");
    if (!pLabel) return;

    QPixmap pic(GetStateIcon(nstate));
    pLabel->setPixmap(pic);

    pLabel = findChild<QLabel *>("TaskItem_State_text");
    if (!pLabel) return;
    pLabel->setText(GetStateString(nstate));

    QProgressBar *pProgress = findChild<QProgressBar *>("TaskItem_Progress");
    if (!pProgress) return;
    if (nstate == DOWNLOADFAILED || nstate == INSTALLFAILED) {
        pLabel->setStyleSheet("font-size:14px; font-family:\
                              \"Microsoft YaHei UI\"; color:#ffff1438;\
                              background:transparent; ");
        pProgress->setStyleSheet("");
        pProgress->setStyleSheet("QProgressBar#TaskItem_Progress { \
        background-color:#ffffffff;\
        border:1px solid #ff559ae5;}\
        QProgressBar#TaskItem_Progress::chunk{\
        background-color:#ff589aeb;}\
        ");

        QPushButton *pBtn = findChild<QPushButton *>("TaskItem_TryBtn");
        if (!pBtn) return;
        pBtn->setVisible(true);
    } else {
        pLabel->setStyleSheet("font-size:14px;font-family:\
        \"Microsoft YaHei UI\";color:#ff0D5BB1;\
        background:transparent;");
        if (!pProgress) return;
        pProgress->setStyleSheet("QProgressBar#TaskItem_Progress { \
        background-color:#ffffffff;\
        border:1px solid #ff559ae5;}\
        QProgressBar#TaskItem_Progress::chunk{\
        background-color:#ff589aeb;}");
        QPushButton *pBtn = findChild<QPushButton *>("TaskItem_TryBtn");
        if (!pBtn) return;
        pBtn->setVisible(false);
    }

    if (nstate == DOWNLOADSUCCESS) {
        setProgressValueSlot(100);
    }
    QLabel *moive = findChild<QLabel *>("TaskItem_Gif");
    if (!moive) return;
    if (nstate == INSTALLING) {
        QLabel *moive = findChild<QLabel *>("TaskItem_Gif");
        moive->setVisible(true);
        QMovie *mv = new QMovie(":/image/task_install_movie");
        moive->setMovie(mv);
        mv->start();
    } else {
        moive->setVisible(false);
    }

    if (nstate != INSTALLFAILED) {
        QLabel *pLabel = findChild<QLabel *>("TaskItem_Error");
        if (!pLabel) return;
        pLabel->setVisible(false);
    }
}

QString CTaskListItem::GetStateIcon(int state) {
    QString path;
    switch (state) {
    case WAIT_DOWNLOAD: {
        path = ":/image/task_waitting_state";
    }break;
    case DOWNLOADFAILED: {
        path = ":/image/task_failed_state";
    }break;
    case DOWNLOADING: {
        path = ":/image/task_download_state";
    }break;
    case DOWNLOADSUCCESS:
    case INSTALLING:
    case INSTALLSUCCESS: {
        path = ":/image/task_install_state";
    }break;
    case INSTALLFAILED: {
        path = ":/image/task_failed_state";
    }break;
    default:
        path = "";
        break;
    }
    return path;
}

QString CTaskListItem::GetStateString(int nState) {
    QString ret = "";
    switch (nState) {
    case WAIT_DOWNLOAD: {
        ret = "等待下载";
    }break;
    case DOWNLOADFAILED: {
        ret = "下载失败";
    }break;
    case DOWNLOADING: {
        ret = "正在下载";
    }break;
    case DOWNLOADSUCCESS: {
        ret = "等待安装";
    }break;
    case INSTALLING: {
        ret = "正在安装";
    }break;
    case INSTALLSUCCESS: {
        ret = "安装成功";
    }break;
    case INSTALLFAILED: {
        ret = "安装失败";
    }break;
    default:
        break;
    }
    return ret;
}

void CTaskListItem::setItemRelevantInfoSlot(QString name , float fsize) {
    QLabel *pLabel = findChild<QLabel *>("TaskItem_Name");
    if (!pLabel) return;
    pLabel->setText(name);

    pLabel = findChild<QLabel *>("TaskItem_Size");
    if (!pLabel) return;
    QString t = QString("%1M").arg(QString::number(fsize, 'f', 2));
    pLabel->setText(t);
}

void CTaskListItem::setProgressValueSlot(int val) {
    QString v = QString("%0%").arg(val);
    QLabel *pLabel = findChild<QLabel *>("TaskItem_value");
    if (pLabel) {
        pLabel->setText(v);
    }
    QProgressBar *pProgress = findChild<QProgressBar *>("TaskItem_Progress");
    pProgress->setValue(val);
}

void CTaskListItem::setErrorInfoSlot(QString err) {
    QLabel *pLabel = findChild<QLabel *>("TaskItem_Error");
    if (pLabel) {
        pLabel->setVisible(true);
        pLabel->setText(err);
    }
}

void CTaskListItem::setIconSlot(QString icon) {
    QLabel *pLabel = findChild<QLabel *>("TaskItem_Icon");
    if (!pLabel) return;

    QPixmap pix(icon);
    bool isSetIcon = true;
    if (pix.isNull())
    {
        QString format = Common::getImageFormat(icon);
        if (format == "ERR") return;
        if (!pix.load(icon, format.toStdString().c_str()))
            isSetIcon = false;
        else
            isSetIcon = true;
 
    } else {
        isSetIcon = true;
    }
    if (isSetIcon)
    {
        pix = pix.scaled(pLabel->size(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        pLabel->setPixmap(pix);
    }
}

void CTaskListItem::connectSignal2Slot() {
    connect(this, SIGNAL(setProgressValueSignal(int)),
        this, SLOT(setProgressValueSlot(int)));
    connect(this, SIGNAL(setItemRelevantInfoSignal(QString, float)),
        this, SLOT(setItemRelevantInfoSlot(QString, float)));

    connect(this, SIGNAL(setErrorInfoSignal(QString)),
        this, SLOT(setErrorInfoSlot(QString)));

    connect(this, SIGNAL(setItemStateSignal(SOFT_STATE)),
        this, SLOT(setItemStateSlot(SOFT_STATE)));

    connect(this, SIGNAL(setIconSignal(QString)),
        this, SLOT(setIconSlot(QString)));
}

CQuickListItem::CQuickListItem(QWidget *parent):QPushButton(parent)
{
    ui.setupUi(this);
    
    initItem();
}

CQuickListItem::~CQuickListItem()
{

}

void CQuickListItem::initItem()
{
    setVisible(true);
    emit setStateSlot(RESTOREINSTALL);
    connectSignal2Slot();
}

void CQuickListItem::connectSignal2Slot() {
    connect(this, SIGNAL(setIconSignal(QString)),
        this, SLOT(setIconSlot(QString)));
    connect(this, SIGNAL(setStateSignal(SOFT_STATE)),
        this, SLOT(setStateSlot(SOFT_STATE)));
    connect(this, SIGNAL(setItemInfoSignal(QString,QString,bool)),
        this, SLOT(setItemInfoSlot(QString,QString ,bool)));
}

void CQuickListItem::setIconSlot(QString icon)
{
    QLabel *pLabel = findChild<QLabel *>("quick_icon");
    if (!pLabel) return;

    QPixmap pix(icon);
    bool isSetIcon = true;
    if (pix.isNull())
    {
        QString format = Common::getImageFormat(icon);
        if (format == "ERR") return;
        if (!pix.load(icon, format.toStdString().c_str()))
            isSetIcon = false;
        else
            isSetIcon = true;

    }
    else {
        isSetIcon = true;
    }
    if (isSetIcon)
    {
        pix = pix.scaled(pLabel->size(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        pLabel->setPixmap(pix);
    }
}

void CQuickListItem::setStateSlot(SOFT_STATE state)
{
    QString iState;
    QString style = "font-size:14px;font-family:\"Microsoft YaHei UI\";background:transparent;text-align:center;";
    //1安装 2下载中 3安装中 4重试 5已安装
    if (state == WAIT_DOWNLOAD || DOWNLOADING == state)
    {
        iState = "下载中";
        style.append("background-image: url(:/image/quickitem_state_download);color:#ffffffff");
    }
    else if (state == DOWNLOADSUCCESS || INSTALLING == state)
    {
        iState = "安装中";
        style.append("background-image: url(:/image/quickitem_state_installing);color:#ffffffff");
    }
    else if (state == DOWNLOADFAILED || INSTALLFAILED == state)
    {
        iState = "重试";
        style.append("background-image: url(:/image/quickitem_state_normal);color:#ff1d6bec");
    }
    else if (state == INSTALLSUCCESS)
    {
        iState = "已安装";
        style.append("background-image: url(:/image/quickitem_state_installed);color:#ff000000");
    }
    else if (state == RESTOREINSTALL)
    {
        iState = "安装";
        style.append("background-image: url(:/image/quickitem_state_normal);color:#ff000000");
    }
    ui.quick_state->setText(iState);
    ui.quick_state->setStyleSheet(style);
}

void CQuickListItem::setItemInfoSlot(QString name, QString detail, bool brec)
{
    QFontMetrics elidfont(ui.quick_name->font());
    ui.quick_name->setText(elidfont.elidedText(name,
        Qt::ElideRight, ui.quick_name->width()));
    ui.quick_detail->setText(detail);
    if (brec) {
        ui.quick_rec->setVisible(true);
    } else {
        ui.quick_rec->setVisible(false);
    }
}

CustomWndItem::CustomWndItem(QWidget *parent):QCheckBox(parent)
{
    ui.setupUi(this);
    ui.custom_check_icon->setStyleSheet("image: url(:/image/custom_checkicon_normal);");
    connect(this, SIGNAL(setIconSignal(QString)),
        this, SLOT(setIconSlot(QString)));
    connect(this, SIGNAL(setItemInfoSignal(QString, bool)),
        this, SLOT(setItemInfoSlot(QString, bool)));
    connect(this, SIGNAL(toggled(bool)), this, SLOT(setCheckedSlot(bool)));
}

CustomWndItem::~CustomWndItem()
{

}

void CustomWndItem::setCheckedSlot(bool bChecked)
{
    
    if (!bChecked)
    {
        ui.custom_check_icon->setStyleSheet("image: url(:/image/custom_checkicon_normal);");
    } else {
        ui.custom_check_icon->setStyleSheet("image: url(:/image/custom_checkicon_selected);");
    }

}

void CustomWndItem::setItemInfoSlot(QString name, bool brec)
{
    QFontMetrics elidfont(ui.custom_item_name->font());
    ui.custom_item_name->setText(elidfont.elidedText(name,
        Qt::ElideRight, ui.custom_item_name->width() - 5));
    if (brec) {
        ui.custom_item_type->setVisible(true);
    } else {
        ui.custom_item_type->setVisible(false);
    }
}

void CustomWndItem::setIconSlot(QString icon)
{
    QLabel *pLabel = findChild<QLabel *>("custom_item_icon");
    if (!pLabel) return;

    QPixmap pix(icon);
    bool isSetIcon = true;
    if (pix.isNull())
    {
        QString format = Common::getImageFormat(icon);
        if (format == "ERR") return;
        if (!pix.load(icon, format.toStdString().c_str()))
            isSetIcon = false;
        else
            isSetIcon = true;

    }
    else {
        isSetIcon = true;
    }
    if (isSetIcon)
    {
        pix = pix.scaled(pLabel->size(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        pLabel->setPixmap(pix);
    }
}

