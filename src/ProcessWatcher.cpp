#include "ProcessWatcher.h"

#include "EventBus.h"
#include "Events.h"
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
        other.current_state_ = PROCESS_STATE::STOPPED;
        other.should_watch_ = false;
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
            other.current_state_ = PROCESS_STATE::STOPPED;
            other.should_watch_ = false;
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
            current_state_ = PROCESS_STATE::RUNNING;
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
        }
    }

    void ProcessWatcher::SetProcessName(const std::string& processName)
    {
        process_name_ = processName;
    }

    void ProcessWatcher::WatcherThread()
    {
        bool wasRunning = (current_state_.load() == PROCESS_STATE::RUNNING);

        // Publish initial state if already running
        if (wasRunning) {
            PublishProcessEvent(PROCESS_STATE::RUNNING);
        }

        while (should_watch_.load())
        {
            bool isRunning = IsProcessRunning();

            if (isRunning && !wasRunning)
            {
                current_state_ = PROCESS_STATE::STARTED;
                wasRunning = true;
                LO_INFO("Process started: " + process_name_);
                PublishProcessEvent(PROCESS_STATE::STARTED);
            }
            else if (!isRunning && wasRunning)
            {
                current_state_ = PROCESS_STATE::STOPPED;
                wasRunning = false;
                LO_INFO("Process stopped: " + process_name_);
                PublishProcessEvent(PROCESS_STATE::STOPPED);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void ProcessWatcher::PublishProcessEvent(PROCESS_STATE state)
    {
        auto event = std::make_shared<ProcessEvent>(state, process_name_);

        switch (state) {
            case PROCESS_STATE::STARTED:
                EventBus::Instance().Publish("process_started", event);
                break;
            case PROCESS_STATE::STOPPED:
                EventBus::Instance().Publish("process_stopped", event);
                break;
            case PROCESS_STATE::RUNNING:
                EventBus::Instance().Publish("process_already_running", event);
                break;
        }
    }

    bool ProcessWatcher::IsProcessRunning() const
    {
        if (process_name_.empty())
        {
            return false;
        }

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE)
        {
            LO_ERROR("Failed to create process snapshot");
            return false;
        }

        struct HandleGuard
        {
            HANDLE h;
            explicit HandleGuard(HANDLE handle) : h(handle) {}
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