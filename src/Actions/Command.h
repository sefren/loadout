#pragma once
#include <string>
#include <variant>

namespace loadout {

    enum class CommandResult {
        Success,
        Failed,
        NotFound,
        Skipped,
    };

    // Base for all commands
    class ICommand {
    public:
        virtual ~ICommand() = default;
        virtual std::string GetName() const = 0;
    };

    // Process commands
    struct Launch : ICommand {
        std::string executable;
        std::string arguments;
        std::string workingDir;
        bool waitForExit = false;

        Launch(const std::string& exe, const std::string& args = "")
            : executable(exe), arguments(args) {}
        std::string GetName() const override { return "Launch"; }
    };

    // Process Commands
    struct Close : ICommand {
        std::string processName;
        bool forceKill = false;
        int timeoutSeconds = 5;

        Close(const std::string& name) : processName(name) {}
        std::string GetName() const override { return "Close"; }
    };

    using CommandVariant = std::variant<
        Launch, Close
    >;

}
