// Events.h
#pragma once
#include "ProcessWatcher.h"

namespace loadout {

    /**
     * Base interface for all events in the system
     */
    class IEvent {
    public:
        virtual ~IEvent() = default;
    };

    /**
     * Event published when a monitored process changes state
     */
    class ProcessEvent final : public IEvent {
    public:
        PROCESS_STATE state_;
        std::string process_name_;

        ProcessEvent(PROCESS_STATE state, const std::string& processName)
            : state_(state)
            , process_name_(processName)
        {
        }
    };

} // loadout