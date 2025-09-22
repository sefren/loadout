#pragma once
#include "Command.h"

namespace loadout {

    // Generic Interface
    template<typename CommandType>
    class ICommandHandler {
    public:
        virtual ~ICommandHandler() = default;
        virtual CommandResult Execute(const CommandType& command) = 0;
    };
}