#pragma once
#include <boost/signals2.hpp>
#include <unordered_map>
#include <typeindex>
#include <any>

class event_aggregator
{
public:
    static event_aggregator& Instance()
    {
        static event_aggregator instance;
        return instance;
    }

    template <typename Event>
    void Publish(Event event)
    {
        typedef boost::signals2::signal<void(Event)> signal_t;
        const std::type_index index(typeid(signal_t));

        const auto it = signals_.find(index);
        if (it == signals_.end())
            return;
        
        auto signal = std::any_cast<std::shared_ptr<signal_t>>(&it->second);
        if (signal != nullptr)
        {
            (*(*signal))(event);
        }
    }


    template <typename Event>
    boost::signals2::connection Subscribe(boost::signals2::signal<void(Event)>::slot_type subscriber)
    {
        typedef boost::signals2::signal<void(Event)> signal_t;
        const std::type_index index(typeid(signal_t));

        auto& signal_any = signals_[index];
        if (!signal_any.has_value())
            signal_any = std::make_shared<signal_t>();

        auto signal = std::any_cast<std::shared_ptr<signal_t>>(&signal_any);
        if (signal != nullptr)
        {
            return (*signal)->connect(subscriber);
        }

        throw std::runtime_error("Failed to subscribe to signal");
    }



private:
    event_aggregator() = default;
    event_aggregator(const event_aggregator&) = delete;
    event_aggregator& operator=(const event_aggregator&) = delete;

    std::unordered_map<std::type_index, std::any> signals_;
};