#pragma once
#include "Common.h"
#include "qdialog.h"
#include "ui_CustomPackageWnd.h"
#include "qwebenginepage.h"
#include "qwebengineview.h"
#include "qwebchannel.h"
#include "qtimer.h"
#include "QucikInstallListWidget.h"
#include "BatchListItem.h"

class CustomWnd : public QDialog
{
    Q_OBJECT
public:
    CustomWnd(QWidget *parent = Q_NULLPTR);
    virtual ~CustomWnd();
    void showNormal(QString unique);
signals:
    void hideSignal();
private slots:
    void setIconSlot(QString icon, QString unique);
    void checkSlot(QAbstractButton *item, bool bchecked);
    void createQuickItemSlot(TPackageVector vt);
    void cooperateCheckSlot(bool bChecked);
    void selectAllCheckSlot(bool bChecked);
    void closeSlot();
    void timeoutSlot();
    void savePackageSlot();
    void NoNetSlot(bool bVisible);
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    bool winEvent(MSG *message, long *result);
    void selectItem(bool bSelected, bool isCooperateCheck);
    bool isSelected(bool isCooperate = false);
    QString generateJsonStr(QString name, QVector<TKuInstallItem>& vec);
    bool isExistSoftByType(bool isCoorprate);
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::CustomPackageWndClass ui;
    CQuickListWidget *_pListWidget;
    QString _itemStyleSheet;
    QString _unique_key;
    QMap<CustomWndItem *, TKuInstallItem> _mapInfo;
    QMap<QString, QWidget *> _map;
    QButtonGroup *_pCheckGroup;
    QTimer _timer;
};
