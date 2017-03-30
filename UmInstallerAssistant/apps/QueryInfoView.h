#pragma  once
#include "Common.h"
#include "qwidget.h"
#include "qgridlayout.h"
#include "configuration.h"
#include "ui_QueryView.h"
class CQueryInfoView:public QWidget
{
    Q_OBJECT
public:
    CQueryInfoView(int cols,QWidget *parent = 0);
    ~CQueryInfoView();
    void addItem(QWidget *pItem);
    void changeUnique(QString unique);
    void setDropDownPos(QRect r);
    virtual void resize(QSize s);
signals:
    void addItemSignal(TPackageVector vt);
private slots:
    void setIconSlot(QString icon, QString unique);
    void addItemSlot(TPackageVector vt);
    void changeItemSlot(QString unique);
private:
    QList<QWidget *>_itemList;
    int _cols;
    Ui::QueryViewClass ui;
private:
    void resizeView(int itemWidth, int itemHeight);
    void proviewSize(int num, QSize sz);
    int _changeHeight;
public:
    int getDistHeight()
    {
        return 200;
    }
};
