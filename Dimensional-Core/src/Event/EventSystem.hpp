#ifndef DM_EVENTSYSTEMH
#define DM_EVENTSYSTEMH
#include <core.hpp>

#include "Event/EventDispatcher.hpp"
#include "Event/EventPool.hpp"
#include <Event/Event.hpp>
#include <utility>

namespace Dimensional {

// Currently a singleton, could change in the future.
class DMCORE_API EventSystem {
public:
    EventSystem()
    {
    }
    ~EventSystem() = default;

    void Init()
    {
        if (s_EventSystem) {
            DM_CORE_WARN("EventSystem Already initialized!");
            return;
        }
        s_EventSystem = this;
        DM_CORE_INFO("Event System Initialized.")
    }

    template <typename EventType, typename... Args>
    static void RaiseEvent(Args&&... args)
    {
        auto s_Sys = Get();
        auto event = s_Sys->m_Pool.retreiveEvent<EventType>(std::forward<Args>(args)...);
        s_Sys->m_Queue.push(event);
    }

    static void ProcessEvents()
    {
        auto s_Sys = Get();
        while (!s_Sys->m_Queue.empty()) {
            auto event = s_Sys->m_Queue.front();
            s_Sys->m_Queue.pop();
            s_Sys->m_Dispatcher.DispatchCallbacks(event);
            s_Sys->m_Pool.RecoverEvent(event);
        }
    }

    template <typename EventType>
    static void AddListener(const typename EventDispatcher::EventCallback<EventType> callback)
    {
        auto s_Sys = Get();
        s_Sys->m_Dispatcher.AddListener<EventType>(std::move(callback));
    }

private:
    static EventSystem* Get()
    {
        return s_EventSystem;
    }
    static EventSystem* s_EventSystem;

    std::queue<Ref<Event>> m_Queue;
    EventPool m_Pool;
    EventDispatcher m_Dispatcher;
};
}

#endif
