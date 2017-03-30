#ifndef EVENT_DRIVER_H
#define EVENT_DRIVER_H

#include <stdint.h>
#include <iostream>
#include <queue>
#include <qmutex.h>
#include <qobject.h>
#include <qthread.h>
#include <qeventloop.h>

typedef bool (*eventCallback)(void *);
typedef void (*timemoutCallback)(void *);

#define EVENT_NORMAL           0
#define EVENT_PENDING          1
#define EVENT_WAIT             2

enum EVENT_STATE
{
    UNPROCESSED,
    PROCESSING,
    PROCESSED
};

struct CommonEvent{
    //事件id，用来唯一标志该事件，当需要唯一标识该事件的时候对其赋值
    uint32_t _id;
    uint32_t _type;
    uint32_t _priority;
    eventCallback _action;
    void *_actionArgs;
    int32_t _timeout;
    timemoutCallback _timeoutAction;
    void *_timeoutArgs;
    QString _outputInfo;

    CommonEvent()
    {
        _id = 0;
        _type = 0;
        _priority = 0;
        _action = NULL;
        _actionArgs = NULL;
        _timeout = 0;
        _timeoutAction = NULL;
        _timeoutArgs = NULL;
    }

    bool operator<(const struct CommonEvent& e) const
    {
        if ( _priority < e._priority ) {
            return true;
        }
        return false;
    }

    struct CommonEvent operator=(struct CommonEvent& e)
    {
        if ( &e == this ) {
            return *this;
        }
        _type = e._type;
        _priority = e._priority;
        _action = e._action;
        _actionArgs = e._actionArgs;
        _timeout = e._timeout;
        _timeoutAction = e._timeoutAction;
        _timeoutArgs = e._timeoutArgs;
        return *this;
    }
};

struct WrapedEvent{
    EVENT_STATE _state;
    struct CommonEvent _event;

    WrapedEvent()
    {
        _state = UNPROCESSED;
    }

    bool operator< (const struct WrapedEvent& e) const
    {
        return _event < e._event;
    }

    struct WrapedEvent operator=(struct WrapedEvent& e)
    {
        if ( this == &e ) {
            return *this;
        }
        _state = e._state;
        _event = e._event;
        return *this;
    }
};


template <typename T>
class ThreadSafePriorityQueue: protected std::priority_queue<T> {
public:
    typedef std::priority_queue<T> super;
    
    bool pop(T& e)
    {
        _mutex.lock();
        if ( super::size() ) {
            T& ele = super::top();
            e = ele;
            super::pop();
            _mutex.unlock();
            return true;
        }
        _mutex.unlock();
        return false;
    }

    bool push(T& e)
    {
        _mutex.lock();
        super::push(e);
        _mutex.unlock();
        return true;
    }

    void clear()
    {
        _mutex.lock();
        while ( !__super::empty() ) {
            super::pop();
        }
        _mutex.unlock();
    }

    bool empty()
    {
        _mutex.lock();
        bool ret = __super::empty();
        _mutex.unlock();
        return ret;
    }

private:
    QMutex _mutex;
};
   
typedef ThreadSafePriorityQueue<struct WrapedEvent> quene_param_type;

class Worker: public QObject {
    Q_OBJECT

public:
    Worker();
    bool isLoopFinished();

public slots:
    void eventLoop(void* queue);

signals:
    void completedAll();
    void completedOne(struct WrapedEvent ev, bool res);
        
private:
    bool _isFinished;
    QMutex _mutex;
};

class WorkerThread: public QThread {
    Q_OBJECT

public:
    void run() Q_DECL_OVERRIDE
    {
        
    }

private:

};

class EventListenerThread: public QThread {
    Q_OBJECT

public:
    void run() Q_DECL_OVERRIDE
    {
        Worker worker;
        connect(this, SIGNAL(begin(void*)), &worker, SLOT(eventLoop(void*)));
        connect(&worker, SIGNAL(completedOne(struct WrapedEvent, bool)), this, SLOT(getCompletedOne(struct WrapedEvent, bool)));
        connect(&worker, SIGNAL(completedAll()), this, SLOT(getCompletedAll()));
        
    }
private:

};

class EventListener: public QThread {
    Q_OBJECT

public:
    EventListener();
    ~EventListener();

    void run() Q_DECL_OVERRIDE
    {
        Worker worker;
        connect(this, SIGNAL(begin(void*)), &worker, SLOT(eventLoop(void*)));
        connect(&worker, SIGNAL(completedOne(struct WrapedEvent, bool)), this, SLOT(getCompletedOne(struct WrapedEvent, bool)));
        connect(&worker, SIGNAL(completedAll()), this, SLOT(getCompletedAll()));
        QEventLoop waitLoop;
        connect(this, SIGNAL(exit()), &waitLoop, SLOT(quit()));
        waitLoop.exec();
//        emit quit();
    }

    static bool installEvent(struct CommonEvent& ev, quene_param_type& queue);
    bool installEventQueue(quene_param_type& queue);
    bool removeEvent(struct WrapedEvent& ev);
    bool clearEvent();

    bool startLoop();
    bool suspendLoop();
    bool resumeLoop();
public slots:
    void getCompletedOne(struct WrapedEvent ev, bool res)
    {
        emit completedOne(ev._event, res);
    }
    void getCompletedAll()
    {
        emit completedAll();
    }

signals:
    void begin(void* queue);
    void completedAll();
    void completedOne(struct CommonEvent ev, bool res);
    void exit();
    void quit();

private:
    QMutex _mutex;
    quene_param_type* _queue;
    QThread _thread;
    Worker _worker;
};

#endif