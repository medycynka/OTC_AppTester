#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <set>
#include <cstdint>


class TimerQueue
{
public:
    typedef uint64_t                       id_type;
    typedef std::function<void()>          handler_type;
    typedef std::unique_lock<std::mutex>   ScopedLock;
    typedef std::chrono::steady_clock      Clock;
    typedef std::chrono::steady_clock::time_point Timestamp;
    typedef std::chrono::milliseconds      Duration;

private:
    struct QueueEvent{
        QueueEvent(id_type id = 0) : id(id), running(false) {};

        template<typename Tfunction>
        QueueEvent(id_type id, Timestamp next, Duration period, Tfunction&& handler) : id(id), next(next), period(period), handler(std::forward<Tfunction>(handler)), running(false) {};

        //QueueEvent(QueueEvent const& r) = delete;
        QueueEvent(QueueEvent&& r) : id(r.id), next(r.next), period(r.period), handler(std::move(r.handler)), running(r.running) {};

        //QueueEvent& operator=(QueueEvent const& r) = delete;
        QueueEvent& operator=(QueueEvent&& r) {
            if (this != &r) {
                id = r.id;
                next = r.next;
                period = r.period;
                handler = std::move(r.handler);
                running = r.running;
            }

            return *this;
        };

        id_type      id;
        Timestamp    next;
        Duration     period;
        handler_type handler;
        bool         running;
    };

    // Comparison functor to sort the timer "queue" by QueueEvent::next
    struct NextActiveComparator{
        bool operator()(const QueueEvent& a, const QueueEvent& b) const {
            return a.next < b.next;
        }
    };
    
public:
    typedef std::unordered_map<id_type, QueueEvent>         InstanceMap;
    typedef std::reference_wrapper<QueueEvent>              QueueValue;
    typedef std::multiset<QueueValue, NextActiveComparator> Queue;

    TimerQueue() : nextId(1), queue(comparator), done(false){
        ScopedLock lock(sync);
        worker = std::thread(std::bind(&TimerQueue::threadStart, this));
    };
    ~TimerQueue();

    id_type create(uint64_t msFromNow, uint64_t msPeriod, const handler_type& handler);
    id_type create(uint64_t msFromNow, uint64_t msPeriod, handler_type&& handler);
    bool    destroy(id_type id);
    bool    exists(id_type id);
    void    cancel_work();

private:
    void    threadStart();
    id_type createImpl(QueueEvent&& item);

private:
    std::mutex              sync;
    std::condition_variable wakeUp;
    id_type                 nextId;
    InstanceMap             active;
    NextActiveComparator    comparator;
    Queue                   queue;
    std::thread             worker;
    bool                    done; 
};

