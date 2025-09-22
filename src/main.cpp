#include "ProcessWatcher.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"
#include "Command.h"
#include "ProcessCommandHandler.h"

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    running = false;
}

void onValorantStarted(const std::shared_ptr<loadout::ProcessEvent>& ev,
                      std::shared_ptr<loadout::ProcessCommandHandler> commandHandler) {
    LO_INFO("Valorant started - executing gaming profile commands");

    // Close browsers
    loadout::Close closeChrome{"chrome.exe"};
    loadout::Close closeEdge{"msedge.exe"};
    loadout::Close closeFirefox{"firefox.exe"};

    // Launch Discord
    loadout::Launch launchDiscord{
        R"(C:\Users\tadit\AppData\Local\Discord\Update.exe)",
        "--processStart Discord.exe"
    };

    // Execute commands
    auto result1 = commandHandler->Execute(closeChrome);
    if (result1 == loadout::CommandResult::Success) {
        LO_INFO("Chrome closed successfully");
    } else if (result1 == loadout::CommandResult::NotFound) {
        LO_INFO("Chrome not running");
    }

    auto result2 = commandHandler->Execute(closeEdge);
    if (result2 == loadout::CommandResult::Success) {
        LO_INFO("Edge closed successfully");
    } else if (result2 == loadout::CommandResult::NotFound) {
        LO_INFO("Edge not running");
    }

    auto result3 = commandHandler->Execute(closeFirefox);
    if (result3 == loadout::CommandResult::Success) {
        LO_INFO("Firefox closed successfully");
    } else if (result3 == loadout::CommandResult::NotFound) {
        LO_INFO("Firefox not running");
    }

    auto result4 = commandHandler->Execute(launchDiscord);
    if (result4 == loadout::CommandResult::Success) {
        LO_INFO("Discord launched successfully");
    } else {
        LO_ERROR("Failed to launch Discord");
    }
}

int main() {
    Logger::Instance().SetLogFile("loadout.log");

    std::cout << "[LOADOUT] Running. Press Ctrl+C to quit\n";

    std::signal(SIGINT, signalHandler);
    LO_INFO("Loadout started.");

    // Create command handler
    auto processCommandHandler = std::make_shared<loadout::ProcessCommandHandler>();

    // Subscribe to process events
    loadout::EventBus::Instance().Subscribe(
        "process_started",
        [processCommandHandler](const std::shared_ptr<loadout::IEvent>& event) {

            auto pe = std::static_pointer_cast<loadout::ProcessEvent>(event);
            if (!pe) {
                LO_ERROR("Received process_started with wrong event type");
                return;
            }

            // Check if it's Valorant starting
            if (pe->process_name_ == "VALORANT-Win64-Shipping.exe") {
                onValorantStarted(pe, processCommandHandler);
            }
        }
    );

    // Start watching for Valorant
    loadout::ProcessWatcher watcher;
    watcher.SetProcessName("VALORANT-Win64-Shipping.exe");
    watcher.StartWatching();

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    watcher.StopWatching();
    LO_INFO("Loadout shutting down.");

    return 0;
}