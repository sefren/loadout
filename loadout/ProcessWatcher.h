#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <atomic>
#include <memory>

namespace loadout
{
    enum class PROCESS_STATE {
        PROCESS_NOT_RUNNING,
        PROCESS_RUNNING,
        PROCESS_ALREADY_RUNNING
    };

    class ProcessWatcher
    {
    public:
        ProcessWatcher() = default;
        ~ProcessWatcher();

        // Delete copy constructor and assignment operator
        ProcessWatcher(const ProcessWatcher&) = delete;
        ProcessWatcher& operator=(const ProcessWatcher&) = delete;

        // Move constructor and assignment operator
        ProcessWatcher(ProcessWatcher&& other) noexcept;
        ProcessWatcher& operator=(ProcessWatcher&& other) noexcept;

        void StartWatching();
        void StopWatching();
        void SetProcessName(const std::string& processName);
        PROCESS_STATE ProcessState() const { return current_state_.load(); }
        bool IsWatching() const { return should_watch_.load(); }

    private:
        bool IsProcessRunning() const;
        void WatcherThread();

        std::atomic<PROCESS_STATE> current_state_{ PROCESS_STATE::PROCESS_NOT_RUNNING };
        std::atomic<bool> should_watch_{ false };
        std::unique_ptr<std::thread> watcher_thread_;
        std::string process_name_;
    };
}