#include "ProcessWatcher.h"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    // Create a process watcher
    loadout::ProcessWatcher watcher;

    // Set the process to watch
    watcher.SetProcessName("notepad.exe");

    // Start watching
    watcher.StartWatching();

    std::cout << "Started monitoring notepad.exe..." << std::endl;
    std::cout << "The watcher will automatically log when the process starts/stops." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}