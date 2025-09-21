#include "ProcessWatcher.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{

	Logger::Instance().SetLogFile("process_watcher.log");

    LO_INFO("ProcessWatcher application started");

    // Create a process watcher
    loadout::ProcessWatcher watcher;

	std::string processName = "notepad.exe";

    // Set the process to watch
    watcher.SetProcessName(processName);

    // Start watching
    watcher.StartWatching();

    // Keep the application running
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}