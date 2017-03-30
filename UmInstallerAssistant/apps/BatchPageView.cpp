//  Copyright (C)  WSWY
//
//  File:       BatchPageView.cpp
//
//  History:    2017-Jan-1-22   Xhsund  Created
//
//--------------------------------------------------------------------------
#include <qfile.h>
#include <qdesktopservices.h>
#include <qdatetime.h>
#include <configuration.h>
#include "Common.h"
#include "./apps/BatchListItem.h"
#include "MoblieItem.h"
#include "UmInstallerAssistant.h"
#include "BatchListItem.h"
#include "SignalManager.h"
#include "BatchListWidget.h"
#include "ConnectManager.h"
#include "DataManager.h"
#include "PackageStateManager.h"
#include "UMessageBox.h"


CBatchPageView::CBatchPageView(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
    ui.verticalLayoutWidget->setGeometry(QRect(14, 63, 302, 433));
    ui.batch_bottom_horzlayout->insertStretch(0);
    ui.batch_version_btn->setVisible(false);
    ui.batch_install_btton->setEnabled(false);
    _pBatchList = new CBatchListWidget(QRect(353, 63, 438, 450), 2, this);
    _pBatchList->setHorizontalSpacing(14);
    _pBatchList->setVerticalSpacing(16);
    _pBatchList->setStyleSheet("background-color:#ffffff;border:0px;");
    _pRadioGroup = new QButtonGroup;
    QString styleSheet = Common::readStyleSheet(":/qss/batchPage.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);

    QString text = QString("版本").append(SOFTWARE_VERSION).append("(");
    ui.batch_version->setText(text);
    ui.batch_version_btn->setVisible(false);

    connectSignal2Slot();
    initBatchMobileList();
}


void CBatchPageView::connectSignal2Slot() {
    connect(CSignalManager::getInstance(), SIGNAL(
        deleteBatchItemSignal(QString)), _pBatchList,
        SLOT(deleteBatchListItemSlot(QString)));
    connect(ui.batch_install_btton, SIGNAL(clicked()),
        this, SLOT(startBatchInstallSlot()));
    connect(CSignalManager::getInstance(),
        SIGNAL(finishedBatchPackageInstallSignal(QString)),
        this, SLOT(finishedBatchPackageInstallSlot(QString)));
    connect(CSignalManager::getInstance(),
        SIGNAL(finishedAllBatchPackageInstallSignal()),
        this, SLOT(restoreInstallButtonSlot()));

    connect(CSignalManager::getInstance(),
        SIGNAL(batchInstallProgressSignal(QString, int, int, int)),
        this, SLOT(setInstallProgressSlot(QString, int, int, int)));
    connect(CSignalManager::getInstance(),
        SIGNAL(setBatchInstallBtnEnableSignal(bool)),
        this, SLOT(setBatchInstallBtnEnableSlot(bool)));
}

void CBatchPageView::initBatchMobileList() {
    for (int i = 0; i < MAX_CONNECT_MOBILE_NUM; i++) {
        CMoblieItem *pItem = new CMoblieItem(ui.verticalLayoutWidget);
        ui.connectLayout->addWidget(pItem);
        ConnectManager::getInstance()->insert(pItem);
    }
}

CBatchPageView::~CBatchPageView() {
    delete _pRadioGroup;
    delete _pBatchList;
}

void CBatchPageView::startBatchInstallSlot() {
    if (!Common::isNetConnected()) {
        UMessageBox::getInstance()->showNormal("无网络连接,请联网重试！", UM_OK);
        return;
    }

    TMobileVector mobileVt;
    ConnectManager::getInstance()->getMobiles(&mobileVt);

    if (mobileVt.size() < 1) {
        // 提示窗口 手机连接不能为空
        UMessageBox::getInstance()->showNormal(
            "手机未连接，请先连接手机！", UM_OK);
        return;
    }

    QString unique = getSelectedPackageUnique();
    if (unique.isEmpty()) {
        // 提示窗口 套餐不能为空
        UMessageBox::getInstance()->showNormal("套餐未选择，请先选择套餐！", UM_OK);
        return;
    }

    __int64 unique_key = unique.toLongLong();
    TPackageVector vt =
        CDataManager::getInstance()->getPackageVector(unique_key, false);
    if (vt.size() == 0) {
        //获取套餐信息错误
        UMessageBox::getInstance()->showNormal("无网络连接,请联网重试！", UM_OK);
        return;
    }
    QString key;
    if (unique == "1" || unique == "2") {
        key = "znzjtc";
    } else {
        key = "zdytc";
    }

    if (PackageStateMan::getInstance()->startInstallTask(
        vt, mobileVt, key, BATCH_TASK)) {
        bool isNeedSynQuickState = false;
        for (auto it = mobileVt.begin(); it != mobileVt.end(); it++) {
            LinkInfo *lk =
                ConnectManager::getInstance()->getLinkInfoBySerial(*it);
            emit static_cast<CMoblieItem *>(lk->pItem)->
                setItemInstallProgressSignal(vt.size(), 0, 0);
            if (ConnectManager::getInstance()->getOnekeySerial() == *it) {
                isNeedSynQuickState = true;
            }
            
        }
        if (isNeedSynQuickState) {
            QString packageName;
            for (auto it_p = vt.begin(); it_p != vt.end();it_p++) {
                CSignalManager::getInstance()->quickInstallChangeSoftStatusSignal(it_p->_packageName,WAIT_DOWNLOAD);
            }
            
        }
        emit CSignalManager::getInstance()->startInstallSingal(vt, mobileVt);
        QPushButton *btn = static_cast<QPushButton *>(QObject::sender());
        emit CSignalManager::getInstance()->sendDisableDeletePackageSignal(
            unique, false);
        _package_unique = unique;
        btn->setText("安装中");
    }
}

QButtonGroup *CBatchPageView::getRadionGroup() {
    return _pRadioGroup;
}

QString CBatchPageView::getSelectedPackageUnique() {
    QList<QAbstractButton *> list = _pRadioGroup->buttons();
    auto it = list.begin();
    bool bchecked = false;
    QString unique;
    for (it; it != list.end(); it++) {
        if ((*it)->isChecked()) {
            bchecked = true;
            QLabel *pLabel = (*it)->findChild<QLabel *>("batch_package_unique");
            unique = pLabel->text();
            break;
        }
    }
    if (!bchecked)
        unique = "";
    return unique;
}

void CBatchPageView::finishedBatchPackageInstallSlot(QString serial) {
    LinkInfo *lk = ConnectManager::getInstance()->getLinkInfoBySerial(serial);
    if (lk && lk->pItem) {
        static_cast<CMoblieItem *>(lk->pItem)->setInstallFinishInfo();
    }
}

void CBatchPageView::setInstallProgressSlot(
    QString serial, int num, int success, int finished) {
    LinkInfo *lk = ConnectManager::getInstance()->getLinkInfoBySerial(serial);
    if (lk) {
        static_cast<CMoblieItem *>(lk->pItem)->setItemInstallProgressSignal(
            num, success, finished);
    }
}

void CBatchPageView::restoreInstallButtonSlot() {
    emit CSignalManager::getInstance()->sendDisableDeletePackageSignal(
        _package_unique, true);

    _package_unique = "";
    ui.batch_install_btton->setEnabled(true);
    ui.batch_install_btton->setText("开始安装");
}

void CBatchPageView::setBatchInstallBtnEnableSlot(bool bEnable) {
    ui.batch_install_btton->setEnabled(bEnable);
}
