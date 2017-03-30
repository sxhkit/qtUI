#pragma once
#include "Common.h"
#include "qwidget.h"
#include "ui_OneKayPage.h"
#include "ListWidget.h"
#include "qradiobutton.h"
#include "qmessagebox.h"
#include "OneKeyListWidget.h"
#include "CustomWnd.h"
enum GroupType {
    DELETE_BUTTON = 0,
    MODIFY_BUTTON,
    INSTALL_BUTTON,
    QUERY_CHECKBOX,
    MASK_CLOSE_BUTTON,
};
class COnekeyPageView : public QWidget
{
    Q_OBJECT
public:
    COnekeyPageView(QWidget *parent = Q_NULLPTR);
    virtual ~COnekeyPageView();
    QButtonGroup *getButtonGroup(GroupType nType);
    QWidget *getListContainer(){
        return _pOnekeyList->getContainer();
    }
    QWidget *getNextItem(QWidget *cur);
private slots:
    void radioClick(QAbstractButton *pRadio,bool bchecked);
    void deleteItemSlot(QAbstractButton *item);
    void ModifyItemSlot(QAbstractButton *item);
    void InstallItemSlot(QAbstractButton *item);
    void modifyPackageSlot(QString unique);
    void restoreInstallButtonSlot();
    void maskCloseSlot(QAbstractButton *close);
    void showMaskSlot(int num, int success);
    void setDeleteEnableSlot(QString unique, bool isEnable);
    void refreshQueryInfoViewSlot(QString curUnique, QString changeUnique);
public slots:
    void OnekeyAddPackageClickSlot();
private:
    Ui::OneKayPageUIClass ui;
    COnekeyListWidget *_pOnekeyList;
    QButtonGroup *_pListCheckboxGroup;
    QButtonGroup *_pDeleteGroup;
    QButtonGroup *_pModifyGroup;
    QButtonGroup *_pInstallGroup;
    QButtonGroup *_pMaskCloseGroup;
    QGridLayout *_pItemLayout;
    QCheckBox *_checkbox;
    CustomWnd *_customWnd;
    bool _bShowDel;

private:
    void MuteCheckBox(QAbstractButton *pRadio);
    //连接信号和槽
    void connectSignal2Slot();
};
