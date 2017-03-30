#pragma once
#include "ListWidget.h"
#include "SignalManager.h"
class CQuickListWidget:public CListWidget
{
    Q_OBJECT
public:
    CQuickListWidget(QRect &rc, int col, QWidget *parent = 0);
    virtual ~CQuickListWidget();
public slots :
    void addItemsSlot(TListItemVector vt);
};

