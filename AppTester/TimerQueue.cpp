#include "TimerQueue.h"

void TimerQueue::threadStart()
{
    ScopedLock lock(sync);

    while (!done) {
        if (queue.empty()) {
            // Wait (forever, in theory) for work
            wakeUp.wait(lock);
        }
        else {
            auto firstInstance = queue.begin();
            QueueEvent& instance = *firstInstance;
			auto now = Clock::now();

            if (now >= instance.next) {
                queue.erase(firstInstance);

                instance.running = true;

                lock.unlock();
                instance.handler();
                lock.lock();

                if (done) {
                    break;
                }
                else if (!instance.running) {
                    active.erase(instance.id);
                }
                else
                {
                    instance.running = false;

                    if (instance.period.count() > 0) {
                        instance.next = instance.next + instance.period;
                        queue.insert(instance);
                    }
                    else {
                        active.erase(instance.id);
                    }
                }
            }
            else {
                wakeUp.wait_until(lock, instance.next);
            }
        }
    }
}

TimerQueue::~TimerQueue() {
    cancel_work();
}

TimerQueue::id_type TimerQueue::create(uint64_t msFromNow, uint64_t msPeriod, const handler_type& handler)
{
    return createImpl(QueueEvent(0, Clock::now() + Duration(msFromNow), Duration(msPeriod), handler));
}

TimerQueue::id_type TimerQueue::create(uint64_t msFromNow, uint64_t msPeriod, handler_type&& handler)
{
    return createImpl(QueueEvent(0, Clock::now() + Duration(msFromNow), Duration(msPeriod), std::move(handler)));
}

bool TimerQueue::destroy(id_type id)
{
    ScopedLock lock(sync);
    auto i = active.find(id);

    if (i == active.end()) {
        return false;
    } else if (i->second.running) {
        i->second.running = false;
    }
    else {
        queue.erase(std::ref(i->second));
        active.erase(i);
    }

    wakeUp.notify_all();

    return true;
}

bool TimerQueue::exists(id_type id)
{
    ScopedLock lock(sync);

    return active.find(id) != active.end();
}

void TimerQueue::cancel_work()
{
    ScopedLock lock(sync);
    done = true;
    wakeUp.notify_all();
    lock.unlock();
    worker.join();
}

TimerQueue::id_type TimerQueue::createImpl(QueueEvent&& item)
{
    ScopedLock lock(sync);
    item.id = nextId++;
    auto iter = active.emplace(item.id, std::move(item));
    queue.insert(iter.first->second);
    wakeUp.notify_all();

    return item.id;
}
