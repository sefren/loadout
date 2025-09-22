#pragma once

#include "CommandHandler.h"
#include <windows.h>
#include <tlhelp32.h>

namespace loadout {
    class ProcessCommandHandler :
        public ICommandHandler<Launch>,
        public ICommandHandler<Close> {

    public:
        CommandResult Execute(const Launch& cmd) override {
            STARTUPINFO si = {sizeof(si)};
            PROCESS_INFORMATION pi = {};

            std::string cmdLine = cmd.executable;
            if (!cmd.arguments.empty()) {
                cmdLine += " " + cmd.arguments;
            }

            BOOL success = CreateProcess(
                cmd.executable.c_str(),
                &cmdLine[0],
                nullptr, nullptr, FALSE, 0, NULL,
                cmd.workingDir.empty() ? nullptr : cmd.workingDir.c_str(),
                &si, &pi
            );

            if (!success) {
                return CommandResult::Failed;
            }

            if (cmd.waitForExit) {
                WaitForSingleObject(pi.hProcess, INFINITE);
            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return CommandResult::Success;
        }

        CommandResult Execute(const Close& cmd) override {
            DWORD pid = FindProcessByName(cmd.processName);
            if (pid == 0) {
                return CommandResult::NotFound;
            }

            if (cmd.forceKill) {
                return KillProcess(pid);
            }

            return CloseProcessGracefully(pid, cmd.timeoutSeconds);
        }

    private:
        DWORD FindProcessByName(const std::string& processName) {
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snapshot == INVALID_HANDLE_VALUE) return 0;

            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(entry);

            if (Process32First(snapshot, &entry)) {
                do {
                    if (_stricmp(entry.szExeFile, processName.c_str()) == 0) {
                        CloseHandle(snapshot);
                        return entry.th32ProcessID;
                    }
                } while (Process32Next(snapshot, &entry));
            }

            CloseHandle(snapshot);
            return 0;
        }

         CommandResult KillProcess(DWORD pid) {
            HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            if (!process) return CommandResult::Failed;

            bool result = TerminateProcess(process, 0);
            CloseHandle(process);
            return result ? CommandResult::Success : CommandResult::Failed;
        }

        CommandResult CloseProcessGracefully(DWORD pid, int timeoutSeconds) {
            // Send WM_CLOSE to all windows of this process
            EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
                DWORD wndPid;
                GetWindowThreadProcessId(hwnd, &wndPid);
                if (wndPid == lParam) {
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
                return TRUE;
            }, static_cast<LPARAM>(pid));

            // Wait for process to exit
            HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
            if (!process) return CommandResult::Failed;

            DWORD waitResult = WaitForSingleObject(process, timeoutSeconds * 1000);
            CloseHandle(process);

            return (waitResult == WAIT_OBJECT_0) ? CommandResult::Success : CommandResult::Failed;
        }

    };
}