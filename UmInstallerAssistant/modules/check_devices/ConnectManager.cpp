#include "ConnectManager.h"
#include "configuration.h"
#include "Common.h"
#include "MoblieItem.h"
#include "SignalManager.h"
#include "qmessagebox.h"

ConnectManager::ConnectManager()
{
}

ConnectManager::~ConnectManager()
{
//     不知道为啥不能释放，否则关闭客户端会崩溃，再次不释放交给系统释放
//     auto it = _listConnect.begin();
//     for (it; it != _listConnect.end(); it++){
//         LinkInfo *info = &(*it);
//         delete  info->pItem;
//     }
}

void ConnectManager::connectSignal2Slot()
{

}

void ConnectManager::setMobileModelSlot(QString serial, QString model) {
    LinkInfo *lk = getLinkInfoBySerial(serial);
    if (!lk) return;
    lk->model = model;
    CMoblieItem *pItem = static_cast<CMoblieItem *>(lk->pItem);
    if (!pItem) return;
    emit pItem->setItemMobileModelSignal(model);
    QString single_ser = getOnekeySerial(false);
    if (single_ser == serial) {
        emit CSignalManager::getInstance()->setSingleConnectStateSignal(serial, model, lk->nState);
    }
}

void ConnectManager::mobileStatemChangeSlot(const QString& serial, DeviceState nstate, struct device_info info)
{
    
    
        
}

bool ConnectManager::isExistConnectedMobile()
{
    bool res = false;
    auto it = _listConnect.begin();
    for (; it != _listConnect.end();it++)
    {
        if ((*it).nState == CONNECTED)
        {
            res = true;
            break;
        }
    }
    return res;
}

LinkInfo * ConnectManager::updateLinkInfo(const QString &serial, DeviceState nstate, const QString &model)
{
    LinkInfo *lk = NULL;
    if (isExistLink(serial)) {
        lk = getValidLinkInfo(serial);
    }
    else {
        lk = getNewLinkInfo();
    }
    if (!lk) return  NULL;

    //保存更新连接相关信息
    lk->serNum = serial;
    lk->nState = nstate;
    lk->model = model;

    if (nstate == UNCONNECT)
    {
        lk->connectTime = 0;
        lk->serNum = "";
        lk->nState = UNCONNECT;
        lk->model = "";
    }
    return lk;
}

bool ConnectManager::isExistLink(const QString &serial)
{
    bool bExist = false;
    auto it = _listConnect.begin();
    for (; it != _listConnect.end(); it++)
    {
        if (serial == it->serNum)
        {
            bExist = true;
            break;
        }
    }
    return bExist;
}

LinkInfo *ConnectManager::getValidLinkInfo(const QString &serial)
{
    LinkInfo *lk = NULL;
    auto it = _listConnect.begin();
    for (; it != _listConnect.end(); it++)
    {
        if (it->serNum == serial)
        {
            lk = &(*it);
            break;
        }
    }
    return lk;
}

LinkInfo *ConnectManager::getNewLinkInfo()
{
    LinkInfo *lk = NULL;
    auto it = _listConnect.begin();
    for (; it != _listConnect.end(); it++)
    {
        if (it->serNum.isEmpty())
        {
            lk = &(*it);
            QDateTime now = QDateTime::currentDateTime();
            qint64 unique = now.toMSecsSinceEpoch();
            lk->connectTime = unique;
            break;
        }
    }
    return lk;
}

void ConnectManager::earseLink(const QString &serial)
{
    auto it = _listConnect.begin();
    for (; it != _listConnect.end(); it++)
    {
        if (it->serNum == serial)
        {
            it->serNum = "";
            it->model = "";
            it->nState = UNCONNECT;
            it->connectTime = 0;
        }
        break;
    }
}

bool ConnectManager::getMobiles(TMobileVector *vt)
{
    auto it = _listConnect.begin();
    for (it; it != _listConnect.end(); it++)
    {
        if (it->model.isEmpty() || it->nState != CONNECTED
            || it->pItem == NULL || it->serNum.isEmpty())
            continue;
        else
            vt->push_back(it->serNum);
    }
    return true;
}

void ConnectManager::insert(QWidget *pItem)
{
    LinkInfo info;
    info.pItem = pItem;
    _listConnect.push_back(info);
}

QString ConnectManager::getOnekeySerial(bool isNeedConnectSuccess)
{
    __int64 min = 0;
    QString ser;
    if (_listConnect.size() > 0)
    {
        min = _listConnect[0].connectTime;
        if (isNeedConnectSuccess)
        {
            if (_listConnect[0].nState == CONNECTED)
                ser = _listConnect[0].serNum;
        }else
            ser = _listConnect[0].serNum;
        
        
    }
   
    auto it = _listConnect.begin();
    for (it; it != _listConnect.end();it++)
    {
        if (it->connectTime != 0 )
        {
            if (min == 0 || it->connectTime < min)
            {
                if (isNeedConnectSuccess)
                {
                    if (it->nState == CONNECTED)
                        ser = it->serNum;
                }else
                    ser = it->serNum;
                
                min = it->connectTime;
            }
        }
    }
    return ser;
}

LinkInfo *ConnectManager::getLinkInfoBySerial(QString &serial)
{
    LinkInfo *lk = NULL;
    auto it = _listConnect.begin();
    for (it; it != _listConnect.end(); it++)
    {
        if ((*it).serNum == serial)
        {
            lk = &(*it);
            break;
        }
    }
    return lk;
}