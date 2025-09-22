#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace loadout {

    class IEvent;

    /**
     * Singleton event bus for pub-sub messaging between components
     */
    class EventBus {
    private:
        // Map of event types to their subscriber callbacks
        std::unordered_map<std::string, std::vector<std::function<void(std::shared_ptr<IEvent>)>>> subscribers;

        EventBus() = default;

    public:
        static EventBus& Instance() {
            static EventBus instance;
            return instance;
        }

        EventBus(const EventBus&) = delete;
        EventBus& operator=(const EventBus&) = delete;

        /**
         * Register a callback for specific event type
         */
        void Subscribe(const std::string& eventType, const std::function<void(std::shared_ptr<IEvent>)> &callback) {
            subscribers[eventType].push_back(callback);
        }

        /**
         * Send event to all subscribers of that event type
         */
        void Publish(const std::string& eventType, const std::shared_ptr<IEvent> &event) {
            if (subscribers.find(eventType) != subscribers.end()) {
                for (auto& callback : subscribers[eventType]) {
                    callback(event);
                }
            }
        }
    };

}