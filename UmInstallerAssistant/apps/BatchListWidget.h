#pragma once
#include "Common.h"
#include "ListWidget.h"
#include "configuration.h"

class CBatchListWidget : public CListWidget {
    Q_OBJECT
public:
    CBatchListWidget(QRect &rc, QWidget *parent = 0);
    CBatchListWidget(QRect &rc, int col, QWidget *parent = 0);
   virtual  ~CBatchListWidget();
public:
    QWidget *getItemByUnique(QString unique);
    virtual void adjustPos(QWidget *pItem);
    virtual void resizeHeight();
    virtual void addItem(QWidget *pItem);

private slots:
    void deleteBatchListItemSlot(QString unique);
    void setUniqueSlot(QWidget *pWidget, QString unique);
    void setBatchPackageInfoSlot(QWidget *pWidget, QString packageName, float fsize, int num);
    void setBatchPackageInfoByUniqueSlot(QString unique, QString packageName);
};
