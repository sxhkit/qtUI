#include "QueryInfoView.h"
#include "BatchListItem.h"
#include "DataManager.h"
#include "SignalManager.h"
#include "Common.h"

CQueryInfoView::CQueryInfoView(int  cols, QWidget *parent) :QWidget(parent), _cols(cols)
{
    ui.setupUi(this);
    QString styleSheet = Common::readStyleSheet(":/qss/queryView.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);

    ui.gridLayout->setMargin(12);
    ui.gridLayout->setHorizontalSpacing(7);
    ui.gridLayout->setVerticalSpacing(8);
   
    connect(CSignalManager::getInstance(), SIGNAL(changeItemSignal(QString)), this, SLOT(changeItemSlot(QString)));
    connect(CSignalManager::getInstance(), SIGNAL(completedDownloadIconSignal(QString, QString)), this, SLOT(setIconSlot(QString, QString)));
    connect(this, SIGNAL(addItemSignal(TPackageVector)), this, SLOT(addItemSlot(TPackageVector)));
    _changeHeight = 0;
}

void CQueryInfoView::changeUnique(QString unique)
{
    TPackageVector vt = CDataManager::getInstance()->getPackageVector(unique.toLongLong());
    proviewSize(vt.size(), QSize(100, 98));
}

void CQueryInfoView::proviewSize(int num,QSize sz)
{
    int row = num / _cols;
    if (num % _cols != 0)
    {
        row++;
    }
    int height = row * sz.height() + ui.gridLayout->verticalSpacing() *(row - 1) + ui.gridLayout->margin() * 2;
    int width = 0;
    if (num < _cols)
    {
        width = num *sz.height() + ui.gridLayout->horizontalSpacing() *(_cols - 1) + ui.gridLayout->margin() * 2;
    }
    else
    {
        width = _cols *sz.height() + ui.gridLayout->horizontalSpacing() *(_cols - 1) + ui.gridLayout->margin() * 2;
    }
    QSize s(width,0 );
    _changeHeight = height + ui.query_view_back->pos().y();
    setMinimumSize(s);
}

CQueryInfoView::~CQueryInfoView()
{
    delete ui.gridLayout;
    auto it = _itemList.begin();
    for (; it != _itemList.end(); it++)
    {
        delete (*it);
    }
    _itemList.clear();
}

void CQueryInfoView::addItemSlot(TPackageVector vt)
{
    auto it = vt.begin();
    for (it; it < vt.end(); it++)
    {
        RecommendSoft type = (*it)._isCooperate == "1" ? Recommended : UnRecommended;
        CQueryItem *item = new CQueryItem(type);
        _itemList.push_back(item);
        float fsize = (float)(*it)._fileSize / (1024 * 1024);
        if (!Common::isIconFileExist((*it)._icon))
        {
            QString unique = QString((*it)._name).append("_queryInofView");
            emit CSignalManager::getInstance()->addDownloadIconSignal((*it)._icon, unique);
        }
        else
        {
            item->setIcon(Common::getIconSavePath((*it)._icon));
        }
        item->setItemInfo((*it)._name, fsize);
        addItem(item);
    }
}

void CQueryInfoView::addItem(QWidget *pItem)
{
    int num = ui.gridLayout->count();
    int row = num / _cols;
    int col = num % _cols;
    ui.gridLayout->addWidget(pItem, row, col);
    resizeView(pItem->minimumWidth(), pItem->minimumHeight());
}

void CQueryInfoView::resizeView(int itemWidth ,int itemHeight)
{
    int num = ui.gridLayout->count();
    if (num == 0)
        return;
    int row = num / _cols;
    if (num % _cols != 0)
    {
        row++;
    }
    int height = row * itemHeight + ui.gridLayout->verticalSpacing() *(row - 1) + ui.gridLayout->margin() * 2;
    int width = 0;
    if (num < _cols)
    {
        width = num *itemWidth + ui.gridLayout->horizontalSpacing() *(_cols - 1) + ui.gridLayout->margin() * 2;
    }
    else
    {
        width = _cols *itemWidth + ui.gridLayout->horizontalSpacing() *(_cols - 1) + ui.gridLayout->margin() * 2;
    }

    QSize s(width,1);
    _changeHeight = height + ui.query_view_back->pos().y();
    ui.gridLayoutWidget->setFixedWidth(width);
    ui.gridLayoutWidget->setMinimumSize(s);
}

void CQueryInfoView::resize(QSize s)
{
    ui.gridLayoutWidget->setFixedHeight(s.height() - 22);
    ui.query_view_back->setFixedHeight(s.height() - 22);
    __super::resize(s);
}

void CQueryInfoView::setIconSlot(QString icon, QString unique)
{
    if (!unique.endsWith("_queryInofView"))
        return;
    QString soft_name = unique.mid(0, unique.length() - QString("_queryInofView").length());
    auto it = _itemList.begin();
    for (it; it != _itemList.end();it++)
    {
        QLabel *name = (*it)->findChild<QLabel *>("item_name");
        if (name->text() == soft_name)
        {
            ((CQueryItem*)(*it))->setIcon(icon);
            break;
        }
    }
}

void CQueryInfoView::changeItemSlot(QString unique)
{
    TPackageVector vt = CDataManager::getInstance()->getPackageVector(unique.toLongLong());
    proviewSize(vt.size(), QSize(100, 98));

    int rows = ui.gridLayout->rowCount();
    for (int i = 0; i < rows;i++)
    {
        ui.gridLayout->setRowMinimumHeight(i, 0);
    }
    auto it = _itemList.begin();
    for (; it != _itemList.end(); it++)
    {
        delete (*it);
    }
    _itemList.clear();

    emit addItemSignal(vt);
}

void CQueryInfoView::setDropDownPos(QRect r)
{
    QSize s = ui.query_view_drop_down->size();
    ui.query_view_drop_down->move(QPoint(r.x() + (r.width() - s.width()) / 2,0));
}