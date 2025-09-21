#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace loadout {

    class IEvent;

    class EventBus {
    private:
        std::unordered_map<std::string, std::vector<std::function<void(std::shared_ptr<IEvent>)>>> subscribers;

        // Singleton stuff
        EventBus() = default;

    public:
        // Singleton instance
        static EventBus& Instance() {
            static EventBus instance;
            return instance;
        }

        EventBus(const EventBus&) = delete;
        EventBus& operator=(const EventBus&) = delete;

        void Subscribe(const std::string& eventType, const std::function<void(std::shared_ptr<IEvent>)> &callback) {
            subscribers[eventType].push_back(callback);
        }

        void Publish(const std::string& eventType, const std::shared_ptr<IEvent> &event) {
            if (subscribers.find(eventType) != subscribers.end()) {
                for (auto& callback : subscribers[eventType]) {
                    callback(event);
                }
            }
        }
    };

}