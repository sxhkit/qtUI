#ifndef CLISTWIDGET_H
#define CLISTWIDGET_H

#include <QWidget>
#include "qgridlayout.h"
#include "qscrollarea.h"
#include "qevent.h"
#include <QProgressBar>
#include "QueryInfoView.h"
#include "qtimer.h"
#define MAX_NUM(X,Y)  ((X) > (Y)?(X):(Y))
class CListWidget : public QWidget
{
#define  MAX_ROW  4096
    Q_OBJECT
public:
    CListWidget(QRect &rc,int col, QWidget *parent = 0);
    ~CListWidget();
    //overwrite
public:
    void setSpacing(int sep);
    void setHorizontalSpacing(int sep);
    void setVerticalSpacing(int sep);
    void setMargin(int);
    int getItemIndex(QWidget *pItem);
    int getColCount();
    QWidget *getItemAt(int index);
    QWidget *getContainer() {
        return _widget;
    }

public slots :
    void addItemSlot(QWidget *pItem);
    void deleteItemSlot(QWidget *item);

private:
    void init(QRect &rc,QWidget *parent);

protected:
    //移除列表中子项
    void earseQlistItem(QWidget *pItem);
    
protected:
    virtual void adjustPos(QWidget *pItem);

    virtual void resizeHeight();

    virtual void deleteItem(QWidget *pItem);

    virtual void push(QWidget *pItem);

    virtual void addItem(QWidget *pItem);
protected:
    int _cols;
    int _queryrow;
    int _horzspace;
    int _vertspace;
    int _margin;
    int _rowCount;
    int _maxHeight;
    QPoint _nextPos;
    QScrollArea *_scrollArea;
    QWidget *_widget;
    QList<QWidget *> _itemList;
};
#endif // CLISTWIDGET_H
