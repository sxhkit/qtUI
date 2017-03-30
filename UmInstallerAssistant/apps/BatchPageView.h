//  Copyright (C)  WSWY
//
//  File:       BatchPageView.h
//
//  History:    2017-Jan-1-22   Xhsund  Created
//
//--------------------------------------------------------------------------
#ifndef UMINSTALLERASSISTANT_APPS_BATCHPAGEVIEW_H_
#define UMINSTALLERASSISTANT_APPS_BATCHPAGEVIEW_H_
#include "Common.h"
#include "ui_BatchPage.h"
#include <qwidget.h>
#include <qmap.h>
#include "CheckDevices.h"
#include "ListWidget.h"
#include "BatchListItem.h"
#include "BatchListWidget.h"
#include <qbuttongroup.h>
class CBatchPageView : public QWidget {
    Q_OBJECT

 public:
    explicit CBatchPageView(QWidget *parent = Q_NULLPTR);
    virtual ~CBatchPageView();

    QButtonGroup *getRadionGroup();
    QWidget *getListContainer(){
        return _pBatchList->getContainer();
    }
 private:
    Ui::BatchPageUIClass ui;

 private slots:
    void startBatchInstallSlot();
    void finishedBatchPackageInstallSlot(QString serial);
    void restoreInstallButtonSlot();
    void setInstallProgressSlot(QString serial,
        int num, int success, int finished);
    void setBatchInstallBtnEnableSlot(bool bEnable);

 private:
    CBatchListWidget *_pBatchList;
    QButtonGroup *_pRadioGroup;
    QString _package_unique;

 private:
    void connectSignal2Slot();
    void initBatchMobileList();
    QString getSelectedPackageUnique();
};
#endif  // UMINSTALLERASSISTANT_APPS_BATCHPAGEVIEW_H_
