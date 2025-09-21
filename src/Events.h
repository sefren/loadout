#pragma once
#include "ProcessWatcher.h"

namespace loadout {

    class IEvent {
    public:
        virtual ~IEvent() = default;
    };

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