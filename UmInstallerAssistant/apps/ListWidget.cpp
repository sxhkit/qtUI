#include <QLayout>
#include "ListWidget.h"
#include "qscrollarea.h"
#include "qevent.h"
#include "qpushbutton.h"
#include "qmap.h"
#include "BatchListItem.h"
#include "Common.h"
#include <QScrollBar> 
CListWidget::CListWidget(QRect &r, int col, QWidget *parent) :
QWidget(parent), _cols(col), _horzspace(0), _vertspace(0), _margin(0), _maxHeight(0)
{
    init(r,parent);
}

void CListWidget::init(QRect &r,QWidget *parent)
{
    //视图区
    setGeometry(r);
    _widget = new QWidget(this);
    _scrollArea = new QScrollArea(this);
    _scrollArea->resize(r.width(), r.height());
    //垂直滚动条不可见，只能通过鼠标滑动
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //窗体1，其父窗体为滚动区域
    //设置滚动区域的窗体
    _widget->resize(r.width(), 0);
    _widget->setStyleSheet("background-color:#ffffff;border:0px;");
    _scrollArea->setWidget(_widget);
    _nextPos.setX(_margin);
    _nextPos.setY(_margin);
    QString styleSheet_s = Common::readStyleSheet(":/qss/vert_scroll.qss");
    _scrollArea->verticalScrollBar()->setStyleSheet(styleSheet_s);
}

CListWidget::~CListWidget()
{

}

void CListWidget::addItem(QWidget *pItem)
{
    push(pItem);
    int index = getItemIndex(pItem);
    auto it = _itemList.begin();
    for (int i = 0; i < _itemList.size(); it++,i++)
    {
        if (i>= index)
        {
            adjustPos(*it);
        }
    }
    resizeHeight();
}

void CListWidget::adjustPos(QWidget *pItem)
{
    QSize s = pItem->size();
    if (s.width() + _nextPos.x() > _widget->width() - _margin)
    {
        int dist = _nextPos.y() + pItem->height() + _vertspace + _margin;
        int iAdd = 0;
        if (_maxHeight - _nextPos.y() > pItem->height() + _margin)
        {
            iAdd = _maxHeight - _nextPos.y();
        }
        else
        {
            iAdd = pItem->height();
        }
        QPoint pos(_margin, _nextPos.y() + _vertspace + iAdd);
        pItem->move(pos);

        _nextPos.setX(_margin + _horzspace + pItem->width());
        _nextPos.setY(pos.y());
        if (dist > _maxHeight)
        {
            _maxHeight = dist;
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

void CListWidget::deleteItem(QWidget *pItem)
{
    _nextPos.setX(pItem->pos().x());
    _nextPos.setY(pItem->pos().y());
    _maxHeight = 0;
    auto it = _itemList.begin();
    int index = getItemIndex(pItem); 
    int i = 0;
    for (; it != _itemList.end();it++)
    {
        if (i > index)
        {
            adjustPos(*it);
        }
        i++;
    }
    earseQlistItem(pItem);
    delete pItem;
    resizeHeight();
}

void CListWidget::resizeHeight()
{
    QWidget *p = _itemList.back();
    QPoint pos = p->pos();
    _maxHeight = pos.y() + p->height() + _margin;
    _nextPos.setX(pos.x() + _horzspace + p->width());
    _nextPos.setY(pos.y());
    _widget->setFixedHeight(_maxHeight);
}

void CListWidget::push(QWidget *pItem)
{
    _itemList.push_back(pItem);
}

void CListWidget::addItemSlot(QWidget *pItem)
{
    addItem(pItem);
}

void CListWidget::deleteItemSlot(QWidget *pItem)
{
    deleteItem(pItem);
}

void CListWidget::earseQlistItem(QWidget *pItem)
{
    auto it = _itemList.begin();
    for (it; it != _itemList.end();it++)
    {
        if (*it == pItem)
        {
            _itemList.erase(it);
            break;
        }
    }
}

void CListWidget::setHorizontalSpacing(int sep)
{
    _horzspace = sep;
}

void CListWidget::setVerticalSpacing(int sep)
{
    _vertspace = sep;
}

void CListWidget::setSpacing(int sep)
{
    _vertspace = sep;
    _horzspace = sep;
}
void CListWidget::setMargin(int margin)
{
    _margin = margin;
    
    _nextPos.setX(_margin);
    _nextPos.setY(_margin);
}

QWidget *CListWidget::getItemAt(int index)
{
    QWidget *pItem = NULL;
    if (index > _itemList.size() || index < 0)  return NULL;
    auto it = _itemList.begin();
    for (int i = 0; i < _itemList.size(); i++,it++)
    {
        if (i == index)
        {
            pItem = *it;
            break;
        }
    }
    return pItem;
}

int CListWidget::getItemIndex(QWidget *pItem)
{
    auto it = _itemList.begin();
    int index = 0;
    bool bfind = false;
    for (it; it < _itemList.end(); it++, index++)
    {
        if (*it == pItem)
        {
            bfind = true;
            pItem = *it;
            break;
        }
    }
    if (!bfind)
    {
        index = -1;
    }
    return index;
}

int CListWidget::getColCount()
{
    return _cols;
}
