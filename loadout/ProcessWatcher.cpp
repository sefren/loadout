#include "ProcessWatcher.h"

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
        }
        return *this;
    }

    void ProcessWatcher::StartWatching()
    {
        if (process_name_.empty())
        {
            std::cerr << "Process name not set." << std::endl;
            return;
        }

        if (should_watch_.load())
        {
            std::cerr << "Already watching processes." << std::endl;
            return;
        }

        should_watch_ = true;

        if (IsProcessRunning())
        {
            current_state_ = PROCESS_STATE::PROCESS_ALREADY_RUNNING;
        }

        watcher_thread_ = std::make_unique<std::thread>(&ProcessWatcher::WatcherThread, this);
    }

    void ProcessWatcher::StopWatching()
    {
        should_watch_ = false;

        if (watcher_thread_ && watcher_thread_->joinable())
        {
            watcher_thread_->join();
            watcher_thread_.reset();
        }
    }

    void ProcessWatcher::SetProcessName(const std::string& processName)
    {
        process_name_ = processName;  // Simple assignment - no conversion needed
    }

    void ProcessWatcher::WatcherThread()
    {
        bool wasRunning = (current_state_.load() == PROCESS_STATE::PROCESS_ALREADY_RUNNING);

        while (should_watch_.load())
        {
            bool isRunning = IsProcessRunning();

            if (isRunning && !wasRunning)
            {
                current_state_ = PROCESS_STATE::PROCESS_RUNNING;
                wasRunning = true;
                std::cout << process_name_ << " started." << std::endl;
            }
            else if (!isRunning && wasRunning)
            {
                current_state_ = PROCESS_STATE::PROCESS_NOT_RUNNING;
                wasRunning = false;
                std::cout << process_name_ << " stopped." << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    bool ProcessWatcher::IsProcessRunning() const
    {
        if (process_name_.empty()) return false;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) return false;

        // Simple RAII for handle cleanup
        struct HandleGuard
        {
            HANDLE h;
            HandleGuard(HANDLE handle) : h(handle) {}
            ~HandleGuard() { if (h != INVALID_HANDLE_VALUE) CloseHandle(h); }
        } guard(hSnap);

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnap, &pe)) return false;

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