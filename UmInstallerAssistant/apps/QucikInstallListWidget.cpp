#include "QucikInstallListWidget.h"
#include "SignalManager.h"

CQuickListWidget::CQuickListWidget(QRect &r, int col, QWidget *parent):
CListWidget(r, col, parent)
{
    connect(CSignalManager::getInstance(), SIGNAL(addQuickItemsSingal(TListItemVector)),
        this, SLOT(addItemsSlot(TListItemVector)));
}

CQuickListWidget::~CQuickListWidget()
{
}

void CQuickListWidget::addItemsSlot(TListItemVector vt)
{
    auto it = vt.begin();
    for (it; it != vt.end(); it++)
    {
        (*it)->show();
        addItem(*it);
    }
}