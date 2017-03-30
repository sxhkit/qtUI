#include "Common.h"
#include "BatchListWidget.h"
#include "qlabel.h"
#include "SignalManager.h"
#include "qstring.h"
#include "DataManager.h"

CBatchListWidget::CBatchListWidget(QRect &r, int col, QWidget *parent) :CListWidget(r,col,parent) 
{
    connect(CSignalManager::getInstance(), SIGNAL(setBatchItemUniqueSignal(QWidget *, QString)),
        this, SLOT(setUniqueSlot(QWidget *, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(setBatchPackageInfoSignal(QWidget *, QString, float, int)), 
        this, SLOT(setBatchPackageInfoSlot(QWidget *, QString, float, int)));
    connect(CSignalManager::getInstance(), SIGNAL(setBatchPackageInfoByUniqueSignal(QString,QString)), 
        this, SLOT(setBatchPackageInfoByUniqueSlot(QString, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(addBatchItemSingal(QWidget *)),
        this, SLOT(addItemSlot(QWidget *)));
}

CBatchListWidget::CBatchListWidget(QRect &r, QWidget *parent) : CListWidget(r, 1, parent) 
{
}

CBatchListWidget::~CBatchListWidget()
{

}

void CBatchListWidget::addItem(QWidget *pItem)
{
    pItem->show();
    __super::addItem(pItem);
}

void CBatchListWidget::adjustPos(QWidget *pItem)
{
    __super::adjustPos(pItem);
}

void CBatchListWidget::resizeHeight()
{
    __super::resizeHeight();
}

QWidget *CBatchListWidget::getItemByUnique(QString unique)
{
    if (unique.isEmpty()) return NULL;
    auto it = _itemList.begin();
    QWidget* pWidget = NULL;
    for (it; it < _itemList.end(); it++) {
        QLabel *pLabel = (*it)->findChild<QLabel *>("batch_package_unique");
        if (!pLabel) continue;
        if (unique == pLabel->text()) {
            pWidget = *it;
            break;
        }
    }
    return pWidget;
}

void CBatchListWidget::deleteBatchListItemSlot(QString unique)
{
    QWidget *pWidget = getItemByUnique(unique);
    if (!pWidget) return;
    deleteItemSlot(pWidget);
}

void CBatchListWidget::setUniqueSlot(QWidget *pWidget, QString unique)
{
    QLabel *pLabel = pWidget->findChild<QLabel *>("batch_package_unique");
    if (!pLabel) return;
    pLabel->setText(unique);
}

void CBatchListWidget::setBatchPackageInfoSlot(QWidget *pWidget, QString packageName, float fsize, int num)
{
    if (!pWidget) return;
    QLabel *pLabel = pWidget->findChild<QLabel *>("batch_package_size");
    if (!pLabel) return;
    QString t = QString("共%1款软件,共%2M").arg(num).arg(QString::number(fsize, 'f', 2));
    pLabel->setText(t);
    if (!pLabel) return;
    pLabel = pWidget->findChild<QLabel *>("batch_package_name");
    pLabel->setText(packageName);
}

void CBatchListWidget::setBatchPackageInfoByUniqueSlot(QString unique,QString packageName)
{
    QWidget *pItem = getItemByUnique(unique);
    float size = CDataManager::getInstance()->getPackageSize(unique.toLongLong());
    int num = CDataManager::getInstance()->getPackItemNum(unique.toLongLong());
    QLabel * pLabel = pItem->findChild<QLabel *>("batch_package_name");
    if (pLabel && packageName == "请输入套餐名")
    {
        packageName = pLabel->text();
    }
    setBatchPackageInfoSlot(pItem, packageName,size, num);
}
