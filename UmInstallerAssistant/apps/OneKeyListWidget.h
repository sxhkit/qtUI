#pragma once
#include "Common.h"
#include "ListWidget.h"
#include "CustomWnd.h"

class COnekeyListWidget : public CListWidget
{
    Q_OBJECT
public:
    COnekeyListWidget(QRect &rc, QWidget *parent = 0);
    COnekeyListWidget(QRect &rc, int col, QWidget *parent = 0);
    ~COnekeyListWidget();

    void ModifyItemStyleSheet(QWidget*pItem,bool bSelected);
    QWidget *getItemByUnique(QString unique);
    QString getNextItemUnique(QWidget *pItem);

    void refreshQueryInfoView(QString unique);
protected:
    void setShowTimerInfo(int msec = 10, int finishTime = 100);
    void setHideTimerInfo(int msec = 10, int finishTime = 100);
    
private slots:
    void showLineItemSlot(QWidget* pItem, QString unique);
    void hideRowItemSlot(bool bClosed = false);
    void showTimerTimeout();
    void hideTimerTimeout();
    void setUniqueSlot(QWidget *pWidget, QString unique);
    void setOnekeyPackageInfoSlot(QWidget *pWidget, QString packageName, float fsize, int num);
    void setOnekeyPackageInfoByUniqueSlot(QString, QString);

private:
    CQueryInfoView *_queryView;

    QTimer *_pShowTimer;
    QTimer *_pHideTime;
    int _showFinishTime;
    int _hideFinishTime;
    int _showInterval;
    int _hideInterval;
    QString _normal_StyleSheet;
    QString _select_StyleSheet;
    int _showIndex;
    int _recPackageNum;
    int _maxCustomNum;
private:
    void connectSignal2Slot();
    void adjustPosDueView();
    void resetNextPos();
    // 不包含添加自定义套餐按钮
    QRect getLastItemPos();
    
    int  getInsertIndex();
    bool isHasAddButton();
    void deleteAddItem(QWidget *pItem);
    virtual void adjustPos(QWidget *pItem);
    virtual void resizeHeight();
    virtual void deleteItem(QWidget *pItem);
    virtual void push(QWidget *pItem);
    virtual void addItem(QWidget *pItem);
    
};
