#ifndef VX_EVENTDISPATCHERH
#define VX_EVENTDISPATCHERH
#include <Event/Event.hpp>
#include <core.hpp>
#include <memory>

namespace Voxler {

class EventDispatcher {
public:
    // Easier to read and understand code
    template <typename EventType>
    using EventCallback = std::function<void(const Ref<EventType>&)>;

public:
    EventDispatcher() = default;
    ~EventDispatcher() = default;

    template <typename EventType>
    void AddListener(EventCallback<EventType> callback)
    {
        auto& listenerVector = m_Listeners[typeid(EventType)];

        // Just learned about C++ lamda functions. Super cool!!
        listenerVector.push_back([callback](const Ref<Event>& event) {
            callback(std::static_pointer_cast<EventType>(event));
        });
    }

    void DispatchCallbacks(const Ref<Event>& event) const
    {
        const Event& ref = *event;
        auto iterator = m_Listeners.find(typeid(ref));
        if (iterator != m_Listeners.end()) {
            for (const auto& callback : iterator->second) {
                callback(event);
            }
        }
    }

private:
    UMap<std::type_index, std::vector<std::function<void(const Ref<Event>&)>>> m_Listeners;
};

}

#endif
