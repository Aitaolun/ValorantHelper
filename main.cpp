#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

// 用于根据进程名称获取进程ID的函数
DWORD GetProcessIdByName(const char *processName) {
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    Process32First(processesSnapshot, &processInfo);
    if (!strcmp(processName, processInfo.szExeFile)) {
        CloseHandle(processesSnapshot);
        return processInfo.th32ProcessID;
    }

    while (Process32Next(processesSnapshot, &processInfo)) {
        if (!strcmp(processName, processInfo.szExeFile)) {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    }

    CloseHandle(processesSnapshot);
    return 0;
}

int main() {
    // 你要找的进程名称
    const char *processName = "example.exe";
    DWORD processID = GetProcessIdByName(processName);

    if (processID == 0) {
        std::cerr << "Process '" << processName << "' not found." << std::endl;
        return 1;
    }

    std::cout << "Process ID of '" << processName << "': " << processID << std::endl;

    // 现在你有了进程ID，你可以用它来打开进程句柄
    HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);
    if (processHandle == NULL) {
        std::cerr << "OpenProcess failed: " << GetLastError() << std::endl;
        return 1;
    }

    // 这里可以添加读取进程内存的代码...

    // 关闭句柄
    CloseHandle(processHandle);

    return 0;
}
