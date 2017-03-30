#include "Common.h"
#include "OneKeyListWidget.h"
#include "qlabel.h"
#include "SignalManager.h"
#include "qpushbutton.h"
#include "DataManager.h"
#include "configuration_option\GlobalConfiguration.h"
#include "UMessageBox.h"
#include "BatchListItem.h"

COnekeyListWidget::COnekeyListWidget(QRect &r, int col, QWidget *parent) :CListWidget(r, col, parent), _recPackageNum(0), _maxCustomNum(0)
{
    _queryView = new  CQueryInfoView(7,_widget);
    _queryView->setVisible(false);
    _queryView->resize(QSize(r.width(),0));
    _queryView->move(0, 0);
    _pShowTimer = new QTimer;
    _pHideTime = new QTimer;
    _showFinishTime = 100;
    _showInterval = 10;
    _hideInterval = 10;
    _hideFinishTime = 100;
    _showIndex = 0;
    _normal_StyleSheet = Common::readStyleSheet(":/qss/onekeyListItem.qss");
    _select_StyleSheet = Common::readStyleSheet(":/qss/onekeyListItem_select.qss");
    connectSignal2Slot();
    _recPackageNum = GlobalIniCfg::getInstance()->getRecPackageNum();
    QString numStr = GlobalIniCfg::getInstance()->queryGlobalValue("max_package_num");
    int num = 0;
    if (numStr.isEmpty())
    {
        num = DEFAULT_PACKAGE_NUM;
    }
    else
        num = numStr.toInt();
    _maxCustomNum = num;
}

COnekeyListWidget::COnekeyListWidget(QRect &r, QWidget *parent) : CListWidget(r, 1, parent), _recPackageNum(0), _maxCustomNum(0)
{
    _queryView = NULL;
    _queryView->setVisible(false);
    _queryView->resize(QSize(r.width(), 0));
    _queryView->move(0, 0);

    _pShowTimer = new QTimer;
    _pHideTime = new QTimer;
    _showFinishTime = 100;
    _showInterval = 10;
    _hideInterval = 10;
    _hideFinishTime = 100;
    connectSignal2Slot();
    _recPackageNum = GlobalIniCfg::getInstance()->getRecPackageNum();
}

void COnekeyListWidget::connectSignal2Slot()
{
    connect(_pShowTimer, SIGNAL(timeout()), this, SLOT(showTimerTimeout()));
    connect(_pHideTime, SIGNAL(timeout()), this, SLOT(hideTimerTimeout()));
    connect(CSignalManager::getInstance(), SIGNAL(setOnekeyItemUniqueSignal(QWidget *, QString)),
        this, SLOT(setUniqueSlot(QWidget *, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(setOnekeyPackageInfoSignal(QWidget*, QString, float, int)),
        this, SLOT(setOnekeyPackageInfoSlot(QWidget *, QString, float, int)));
    connect(CSignalManager::getInstance(), SIGNAL(setOnekeyPackageInfoByUniqueSignal(QString, QString)),
        this, SLOT(setOnekeyPackageInfoByUniqueSlot(QString, QString)));
}

COnekeyListWidget::~COnekeyListWidget()
{

}

void COnekeyListWidget::addItem(QWidget *pItem)
{
    int num = _maxCustomNum;
    if (isHasAddButton())
    {
        QRect pos = getLastItemPos();
        QPoint p(pos.left(), pos.top());
        _nextPos = p; 
        if (_itemList.size() >= num + _recPackageNum)
        {
            deleteAddItem(_itemList.back());
        }
    }
    else
    {
        if (_itemList.size() >= num + _recPackageNum)
        {
            return;
        }
    }
    pItem->show();
    __super::addItem(pItem);
}

void COnekeyListWidget::adjustPos(QWidget *pItem)
{
    QSize s = pItem->size();
    if (s.width() + _nextPos.x() > _widget->width() - _margin)
    {
        QRect r(_margin, _nextPos.y()  + MAX_NUM((_maxHeight - _nextPos.y()),
            (pItem->height() + _margin)) ,pItem->width() ,pItem->height());
        QPoint dist_pos(r.left(), r.top() + _vertspace);
        if (_queryView->isVisible())
        {
            QPoint pos = _queryView->pos();
            if (r.contains(pos))
            {
                dist_pos.setX(_margin);
                dist_pos.setY(pos.y() + _queryView->height() + _vertspace);
                pItem->move(dist_pos);
            }
            else
                pItem->move(dist_pos);
        }else
            pItem->move(dist_pos);
        _nextPos.setX(_margin + _horzspace + pItem->width());
        _nextPos.setY(dist_pos.y());

        int dist_height = dist_pos.y() + pItem->height() + _margin;
        if (dist_height > _maxHeight)
        {
            _maxHeight = dist_height;
        }
    }
    else
    {
        int dist = _nextPos.y() + pItem->height() + _margin;
        pItem->move(_nextPos.x(), _nextPos.y());
        _nextPos.setX(_nextPos.x() + pItem->width() + _horzspace);
        if (dist > _maxHeight)
        {
            _maxHeight = dist;
        }
    }
}

void COnekeyListWidget::resizeHeight()
{
    if (_queryView->isVisible())
    {
        QPoint queryPos = _queryView->pos();
        QSize querySz = _queryView->size();
        QWidget *p = _itemList.back();
        QPoint pos = p->pos();
        if (pos.y() > queryPos.y())
        {
            _maxHeight = pos.y() + p->height() + _margin;
        }
        else
        {
            _maxHeight = queryPos.y() + querySz.height() + _margin;
        }
        _nextPos.setX(pos.x() + _horzspace + p->width());
        _nextPos.setY(pos.y());
    }
    else
    {
        QWidget *p = _itemList.back();
        QPoint pos = p->pos();
        _maxHeight = pos.y() + p->height() + _margin;
        _nextPos.setX(pos.x() + _horzspace + p->width());
        _nextPos.setY(pos.y());
    }
    _widget->setFixedHeight(_maxHeight);
}

void COnekeyListWidget::showLineItemSlot(QWidget * pItem, QString unique)
{
    if (!_queryView) return;
    if (_queryView->isVisible())
    {
        hideRowItemSlot(true);
    }
    QPoint dist_pos(_margin,pItem->pos().y()  + pItem->height());
    _queryView->changeUnique(unique);
    _queryView->move(dist_pos);
    _queryView->setVisible(true);
    _queryView->resize(QSize(width(),0));
    _nextPos.setX(pItem->pos().x());
    _nextPos.setY(pItem->pos().y());
    _maxHeight = pItem->pos().y() + _margin + pItem->height();
    auto it = _itemList.begin();
    int index = getItemIndex(pItem);
    int i = 0;

    for (; it != _itemList.end(); it++)
    {
        if (i >= index)
        {
            adjustPos(*it);
        }
        i++;
    }
    resizeHeight();
    _pHideTime->stop();
    _pShowTimer->start(_showInterval);
    emit CSignalManager::getInstance()->changeItemSignal(unique);
    ModifyItemStyleSheet(pItem, true);
    _showIndex = getItemIndex(pItem);
    _queryView->setDropDownPos(pItem->geometry());
}

void COnekeyListWidget::ModifyItemStyleSheet(QWidget*pItem ,bool bSelected)
{
    if (!pItem) return;
    if (bSelected)
    {
        pItem->setStyleSheet(_select_StyleSheet);
    }
    else
    {
        pItem->setStyleSheet(_normal_StyleSheet);
    }
}

void COnekeyListWidget::deleteItem(QWidget *pItem)
{
    bool bHideView = false;
    if (_showIndex >= getInsertIndex() - 1)
    {
        bHideView = true;
        _queryView->setVisible(false);
    }
    if (_queryView->isVisible())
    {
        QString unique = getNextItemUnique(pItem);
        if (!unique.isEmpty())
        {
            emit _queryView->changeUnique(unique);
        }
        
    }

    QString numStr = GlobalIniCfg::getInstance()->queryGlobalValue("max_package_num");
    int num = 0;
    if (numStr.isEmpty())
    {
        num = DEFAULT_PACKAGE_NUM;
    }
    else
        num = numStr.toInt();
    QWidget *parentWidget = (QWidget *)pItem->parent();
    __super::deleteItem(pItem);
    if (!isHasAddButton())
    {
        if (_itemList.size() == num + _recPackageNum - 1)
        {
            QWidget *p = new COnekeyListAddItem(parentWidget);
            addItem(p);
            connect(p, SIGNAL(clicked()), parent(), SLOT(OnekeyAddPackageClickSlot()));
        }
    }
}

void COnekeyListWidget::deleteAddItem(QWidget *pItem)
{
    bool bHideView = false;
    int index = getItemIndex(pItem);

    if (_showIndex >= getInsertIndex() - 1)
    {
        bHideView = true;
        _queryView->setVisible(false);
    }
    if (_queryView->isVisible())
    {
        QString unique = getNextItemUnique(pItem);
        if (!unique.isEmpty())
        {
            emit _queryView->changeUnique(unique);
        }

    }
    __super::deleteItem(pItem);
}

QString COnekeyListWidget::getNextItemUnique(QWidget *pItem)
{
    QString unique = "";
    auto it = _itemList.begin();
    for (it; it != _itemList.end();it++)
    {
        if (*it == pItem)
        {
            it++;
            if (it!=_itemList.end())
            {
                QLabel *p = (*it)->findChild<QLabel *>("onekey_listitem_unique");
                if (p)
                    unique = p->text();
            }
            break;
        }
    }
    return unique;
}

void COnekeyListWidget::hideRowItemSlot(bool bClosed)
{
    if (_queryView)
    {
        if (bClosed)
        {
            _queryView->setVisible(false);
            resetNextPos();
            for (auto it = _itemList.begin(); it != _itemList.end(); it++)
            {
                QPoint item_pos = (*it)->pos();
                if (item_pos.y() > _queryView->pos().y())
                {
                    adjustPos(*it);
                }
            }
            resizeHeight();
        }
        else
        {
            if (_queryView->isVisible())
            {
                _pShowTimer->stop();
                _pHideTime->start(_hideInterval);
            }
        }
    }
}

void COnekeyListWidget::resetNextPos()
{
    QPoint tmp;
    QWidget *pt;
    for (auto it = _itemList.begin(); it != _itemList.end();it++)
    {
        if ((*it)->pos().y() < _queryView->pos().y())
        {
            tmp = (*it)->pos();
            pt = (*it);
        }
    }
    int x = tmp.x() + pt->width() + _horzspace;
    int y = tmp.y();
    _maxHeight = y + pt->height();
    _nextPos = QPoint(x, y);
}

void COnekeyListWidget::adjustPosDueView()
{
    auto it = _itemList.begin();
    resetNextPos();
    for (; it != _itemList.end(); it++)
    {
        QPoint item_pos = (*it)->pos();
        if (item_pos.y() > _queryView->pos().y())
        {
            adjustPos(*it);
        }
    }
    resizeHeight();
}

void COnekeyListWidget::showTimerTimeout()
{
    if (_queryView)
    {
        int height =_queryView->getDistHeight();
        QSize s = _queryView->size();
        if (s.height() >= height)
        {
            _pShowTimer->stop();
        }
        else
        {
            int dist = (height * _showInterval / _showFinishTime + s.height()) > height ? height : (height * _showInterval / _showFinishTime + s.height());
            _queryView->resize(QSize(s.width(), dist));
        }
        adjustPosDueView();
    }
}

void COnekeyListWidget::hideTimerTimeout()
{
    if (_queryView)
    {
        int height = _queryView->getDistHeight();
        QSize s = _queryView->size();
        if (s.height() == 0)
        {
            _queryView->setVisible(false);
            _pHideTime->stop();
        }
        else
        {
            int dist = (s.height() - height * _showInterval / _showFinishTime) < 0 ? 0 : (s.height() - height * _showInterval / _showFinishTime);
            _queryView->resize(QSize(s.width(), dist));
            
        }
        adjustPosDueView();
    }
}

void COnekeyListWidget::setUniqueSlot(QWidget *pWidget, QString unique)
{
    QLabel *pLabel = pWidget->findChild<QLabel *>("onekey_listitem_unique");
    if (!pLabel) return;
    pLabel->setText(unique);
}

void COnekeyListWidget::setOnekeyPackageInfoSlot(QWidget *pWidget, QString packageName, float fsize, int num)
{
    if (!pWidget) return;
    QLabel *pLabel = pWidget->findChild<QLabel *>("onekey_listitem_size");
    if (!pLabel) return;
    QString t = QString("共%1款软件,共%2M").arg(num).arg(QString::number(fsize, 'f', 2));
    pLabel->setText(t);
    pLabel = pWidget->findChild<QLabel *>("onekey_listitem_name");
    pLabel->setText(packageName);
}

void COnekeyListWidget::setOnekeyPackageInfoByUniqueSlot(QString unique, QString packageName)
{
    QWidget *pItem = getItemByUnique(unique);
    float size = CDataManager::getInstance()->getPackageSize(unique.toLongLong());
    int num = CDataManager::getInstance()->getPackItemNum(unique.toLongLong());
    QLabel * pLabel = pItem->findChild<QLabel *>("onekey_listitem_name");
    if (pLabel && packageName == "请输入套餐名")
    {
        packageName = pLabel->text();
    }
    setOnekeyPackageInfoSlot(pItem, packageName, size, num);
}

QWidget *COnekeyListWidget::getItemByUnique(QString unique)
{
    if (unique.isEmpty()) return NULL;
    auto it = _itemList.begin();
    QWidget* pWidget = NULL;
    for (it; it < _itemList.end(); it++)
    {
        QLabel *pLabel = (*it)->findChild<QLabel *>("onekey_listitem_unique");
        if (!pLabel) continue;
        if (unique == pLabel->text()) {
            pWidget = *it;
            break;
        }
    }
    return pWidget;
}

void COnekeyListWidget::push(QWidget *pItem)
{
    bool bAtEnd = true;
    if (_itemList.empty())
    {
        _itemList.push_back(pItem);
        return;
    }
    int index = _itemList.size() - 1;
    QWidget *back = _itemList.back();
    if (back)
    {
        QWidget *p = back->findChild<QLabel *>("onekey_listitem_unique");
        if (!p)
        {
            bAtEnd = false;
        }
    }
    if (bAtEnd)
    {
        _itemList.push_back(pItem);
    }
    else
        _itemList.insert(getInsertIndex(), pItem);
}

int  COnekeyListWidget::getInsertIndex()
{
    int index = _itemList.size() - 1;
    QWidget *back = _itemList.back();
    bool badd = false;
    if (back)
    {
        QWidget *p = back->findChild<QLabel *>("onekey_listitem_unique");
        while (!p)
        {
            badd = true;
            if (index > 0)
            {
                back = getItemAt(--index);
                p = back->findChild<QLabel *>("onekey_listitem_unique");
            }
            else {
                badd = false;
                break;
            }
        }
        index = badd?index + 1:index;
    }
    return  index;
}

QRect COnekeyListWidget::getLastItemPos()
{
    int index = getInsertIndex();
    QRect rc = getItemAt(index)->geometry();
    return rc;
}

bool COnekeyListWidget::isHasAddButton()
{
    bool bhas = false;
    if (_itemList.isEmpty())
    {
        bhas = false;
        return bhas;
    }
    QWidget *back = _itemList.back();
    if (back)
    {
        QWidget *p = back->findChild<QLabel *>("onekey_listitem_unique");
        if (!p)
        {
            bhas = true;
        }
    }
    return bhas;
}

void COnekeyListWidget::refreshQueryInfoView(QString unique)
{
    if (!_queryView) return;
    if (_queryView->isVisible())
    {
        _queryView->changeUnique(unique);
        _queryView->resize(QSize(width(), _queryView->getDistHeight()));

        QWidget *pItem = getItemByUnique(unique);
        _nextPos.setX(pItem->pos().x());
        _nextPos.setY(pItem->pos().y());
        _maxHeight = pItem->pos().y() + _margin + pItem->height();
        auto it = _itemList.begin();
        int index = getItemIndex(pItem);
        int i = 0;

        for (; it != _itemList.end(); it++)
        {
            if (i >= index)
            {
                adjustPos(*it);
            }
            i++;
        }
        resizeHeight();
        emit CSignalManager::getInstance()->changeItemSignal(unique);
        ModifyItemStyleSheet(pItem, true);
        _showIndex = getItemIndex(pItem);
        _queryView->setDropDownPos(pItem->geometry());
    }
    
}