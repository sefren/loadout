#include "ProcessWatcher.h"
#include "Events.h"
#include "EventBus.h"
#include "Logger.h"

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    running = false;
}

int main() {
    Logger::Instance().SetLogFile("loadout.log");

    std::cout << "[LOADOUT] Running — Ctrl+C to quit\n";

    std::signal(SIGINT, signalHandler);
    LO_INFO("Loadout started.");

    // Temporary: watching hardcoded process
    loadout::ProcessWatcher watcher;
    watcher.SetProcessName("notepad.exe");
    watcher.StartWatching();

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    watcher.StopWatching();
    LO_INFO("Loadout shutting down.");

    return 0;
}
