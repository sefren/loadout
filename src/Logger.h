#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <iostream>

enum class LogLevel { Debug, Info, Warn, Error };

class Logger {
public:
    static Logger& Instance() {
        static Logger inst;
        return inst;
    }

    void SetLogFile(const std::string& path) {
        std::lock_guard<std::mutex> lk(mu_);
        if (ofs_.is_open()) ofs_.close();
        ofs_.open(path, std::ios::app);
    }

    void Log(LogLevel lvl, const std::string& msg) {
        std::lock_guard<std::mutex> lk(mu_);
        std::ostringstream line;
        line << TimeNow() << " " << Prefix(lvl) << " " << msg << "\n";
        std::string s = line.str();
        std::cout << s;
        if (ofs_.is_open()) ofs_ << s;
    }

private:
    std::mutex mu_;
    std::ofstream ofs_;

    Logger() = default;
    ~Logger() { if (ofs_.is_open()) ofs_.close(); }

    static std::string Prefix(const LogLevel lvl) {
        switch (lvl) {
        case LogLevel::Debug: return "LO:DEBUG";
        case LogLevel::Info:  return "LO:INFO ";
        case LogLevel::Warn:  return "LO:WARN ";
        case LogLevel::Error: return "LO:ERROR";
        }
        return "LO:UNKNOWN";
    }

    static std::string TimeNow() {
        using namespace std::chrono;
        const auto now = system_clock::now();
        const auto tt = system_clock::to_time_t(now);
        const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream ss;
        ss << std::put_time(std::localtime(&tt), "%Y-%m-%d %H:%M:%S")
            << "." << std::setw(3) << std::setfill('0') << ms.count();
        return ss.str();
    }
};

// Macros
#define LO_DEBUG(msg) Logger::Instance().Log(LogLevel::Debug, msg)
#define LO_INFO(msg)  Logger::Instance().Log(LogLevel::Info,  msg)
#define LO_WARN(msg)  Logger::Instance().Log(LogLevel::Warn,  msg)
#define LO_ERROR(msg) Logger::Instance().Log(LogLevel::Error, msg)
