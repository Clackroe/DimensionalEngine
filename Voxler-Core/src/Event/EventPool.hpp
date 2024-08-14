#ifndef VX_EVENTPOOLH
#define VX_EVENTPOOLH
#include <core.hpp>

#include <Event/Event.hpp>
#include <memory>
#include <utility>

namespace Dimensional {

class EventPool {
public:
    template <typename EventType, typename... Args>
    Ref<EventType> retreiveEvent(Args&&... args)
    {
        Stack<Ref<Event>> pool = m_Pool[typeid(EventType)];
        if (!pool.empty()) { // Return an event from the pool if possible
            Ref<EventType> event = std::static_pointer_cast<EventType>(pool.top());
            pool.pop();

            // Very cool feature of C++. I just learned about it
            new (event.get()) EventType(std::forward<Args>(args)...);
            return event;

        } else {
            // If no pooled event is available, make a new one.
            return CreateRef<EventType>(std::forward<Args>(args)...);
        }
    }

    // Add the event back to the event pool for future use.
    void RecoverEvent(Ref<Event> event)
    {
        const Event& ref = *event;
        m_Pool[typeid(ref)].push(event);
    }

private:
    UMap<std::type_index, Stack<Ref<Event>>> m_Pool;
};
}

#endif
