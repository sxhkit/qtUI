#include "EventDriver.h"
#include <QTime>
#include <qdebug.h>

EventListener::EventListener()
{
    qRegisterMetaType<struct WrapedEvent>("struct WrapedEvent");
}

EventListener::~EventListener()
{
    _queue->clear();
    emit exit();
    quit();
    wait();
}

bool EventListener::installEvent(struct CommonEvent& ev, quene_param_type& queue)
{
    struct WrapedEvent we;
    we._state = UNPROCESSED;
    we._event = ev;
    queue.push(we);
    return true;
}
    
bool EventListener::installEventQueue(quene_param_type& queue)
{
    _queue = &queue;
    return true;
}

bool EventListener::removeEvent(struct WrapedEvent& ev)
{
    return true;
}

bool EventListener::clearEvent()
{
    _queue->clear();
    return _queue->empty();
}

bool EventListener::startLoop()
{
    emit begin((void*)_queue);
    return true;
}

bool EventListener::suspendLoop()
{
    return true;
}

bool EventListener::resumeLoop()
{
    return true;
}
    
bool Worker::isLoopFinished()
{
    _mutex.lock();
    bool ret = _isFinished;
    _mutex.unlock();
    return ret;
}

Worker::Worker()
{
    qRegisterMetaType<struct WrapedEvent>("struct WrapedEvent");
    _isFinished = true;
}

void Worker::eventLoop(void* quene)
{
    quene_param_type *q = (quene_param_type*)quene;
    _mutex.lock();
    _isFinished = false;
    _mutex.unlock();
    while ( !q->empty() ) {
        bool res = false;
        struct WrapedEvent ev;
        if ( !q->pop(ev) ) {
            continue;
        }
        eventCallback func = ev._event._action;
        void* args = ev._event._actionArgs;
            
        QTime qtime;
        if ( ev._event._timeout != 0 ) {
            qtime.start();
        }
        ev._state = PROCESSING;
        res = func(args);
        ev._state = PROCESSED;
        if ( ev._event._timeout != 0 ) {
            int32_t  timeval = qtime.elapsed();
            if ( timeval > ev._event._timeout ) {
                ev._event._timeoutAction(ev._event._timeoutArgs);
            }
        }
        emit completedOne(ev, res);
    }
    _mutex.lock();
    _isFinished = true; 
    _mutex.unlock();
    emit completedAll();
}
