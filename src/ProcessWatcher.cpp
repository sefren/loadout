#include "ProcessWatcher.h"
#include "Logger.h"

namespace loadout {
    ProcessWatcher::~ProcessWatcher()
    {
        StopWatching();
    }

    ProcessWatcher::ProcessWatcher(ProcessWatcher&& other) noexcept
        : current_state_(other.current_state_.load())
        , should_watch_(other.should_watch_.load())
        , watcher_thread_(std::move(other.watcher_thread_))
        , process_name_(std::move(other.process_name_))
    {
        other.current_state_ = PROCESS_STATE::PROCESS_NOT_RUNNING;
        other.should_watch_ = false;
        LO_DEBUG("ProcessWatcher moved");
    }

    ProcessWatcher& ProcessWatcher::operator=(ProcessWatcher&& other) noexcept
    {
        if (this != &other)
        {
            StopWatching();
            current_state_ = other.current_state_.load();
            should_watch_ = other.should_watch_.load();
            watcher_thread_ = std::move(other.watcher_thread_);
            process_name_ = std::move(other.process_name_);
            other.current_state_ = PROCESS_STATE::PROCESS_NOT_RUNNING;
            other.should_watch_ = false;
            LO_DEBUG("ProcessWatcher move assigned");
        }
        return *this;
    }

    void ProcessWatcher::StartWatching()
    {
        if (process_name_.empty())
        {
            LO_ERROR("Process name not set - cannot start watching");
            return;
        }

        if (should_watch_.load())
        {
            LO_WARN("Already watching processes - ignoring start request");
            return;
        }

        should_watch_ = true;

        if (IsProcessRunning())
        {
            current_state_ = PROCESS_STATE::PROCESS_ALREADY_RUNNING;
            LO_INFO("Process " + process_name_ + " was already running when monitoring started");
        }

        watcher_thread_ = std::make_unique<std::thread>(&ProcessWatcher::WatcherThread, this);
        LO_INFO("Started monitoring process: " + process_name_);
    }

    void ProcessWatcher::StopWatching()
    {
        if (should_watch_.load())
        {
            should_watch_ = false;
            LO_INFO("Stopping monitoring for process: " + process_name_);
        }

        if (watcher_thread_ && watcher_thread_->joinable())
        {
            watcher_thread_->join();
            watcher_thread_.reset();
            LO_DEBUG("Watcher thread stopped and cleaned up");
        }
    }

    void ProcessWatcher::SetProcessName(const std::string& processName)
    {
        process_name_ = processName;
        LO_DEBUG("Process name set to: " + processName);
    }

    void ProcessWatcher::WatcherThread()
    {
        LO_DEBUG("Watcher thread started");
        bool wasRunning = (current_state_.load() == PROCESS_STATE::PROCESS_ALREADY_RUNNING);

        while (should_watch_.load())
        {
            bool isRunning = IsProcessRunning();

            if (isRunning && !wasRunning)
            {
                current_state_ = PROCESS_STATE::PROCESS_RUNNING;
                wasRunning = true;
                LO_INFO("Process started: " + process_name_);
            }
            else if (!isRunning && wasRunning)
            {
                current_state_ = PROCESS_STATE::PROCESS_NOT_RUNNING;
                wasRunning = false;
                LO_INFO("Process stopped: " + process_name_);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        LO_DEBUG("Watcher thread ending");
    }

    bool ProcessWatcher::IsProcessRunning() const
    {
        if (process_name_.empty())
        {
            LO_DEBUG("Process name is empty - returning false");
            return false;
        }

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE)
        {
            LO_ERROR("Failed to create process snapshot");
            return false;
        }

        // Simple RAII for handle cleanup
        struct HandleGuard
        {
            HANDLE h;
            HandleGuard(HANDLE handle) : h(handle) {}
            ~HandleGuard() { if (h != INVALID_HANDLE_VALUE) CloseHandle(h); }
        } guard(hSnap);

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnap, &pe))
        {
            LO_ERROR("Failed to get first process from snapshot");
            return false;
        }

        do
        {
            if (_stricmp(pe.szExeFile, process_name_.c_str()) == 0)
            {
                return true;
            }
        } while (Process32Next(hSnap, &pe));

        return false;
    }
}
