#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

DWORD GetProcessIdByName(const char *processName) {
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    Process32First(processesSnapshot, &processInfo);
    if (strcmp(processName, processInfo.szExeFile) == 0) {
        CloseHandle(processesSnapshot);
        return processInfo.th32ProcessID;
    }

    while (Process32Next(processesSnapshot, &processInfo)) {
        if (strcmp(processName, processInfo.szExeFile) == 0) {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    }

    CloseHandle(processesSnapshot);
    return 0;
}

bool WriteMemory(DWORD processID, LPVOID address, const void* data, SIZE_T size) {
    HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, processID);
    if (processHandle == NULL) {
        std::cerr << "OpenProcess failed: " << GetLastError() << std::endl;
        return false;
    }

    BOOL result = WriteProcessMemory(processHandle, address, data, size, NULL);
    if (result == FALSE) {
        std::cerr << "WriteProcessMemory failed: " << GetLastError() << std::endl;
    } else {
        std::cout << "Memory written successfully" << std::endl;
    }

    CloseHandle(processHandle);
    return result != FALSE;
}

int main() {
    const char *processName = "VALORANT.exe"; 
    LPCVOID targetAddress = (LPCVOID)0x484f4f5849; 
    int valueToWrite = 21195828; // 替换为特殊值

    DWORD processID = GetProcessIdByName(processName);
    if (processID == 0) {
        std::cerr << "Process not found." << std::endl;
        return 1;
    }

    std::cout << "Process ID: " << processID << std::endl;

    if (WriteMemory(processID, (LPVOID)targetAddress, &valueToWrite, sizeof(valueToWrite))) {
        std::cout << "Value written successfully." << std::endl;
    }

    HWND hWnd_Target = FindWindow(NULL, TEXT("VALORANT")); 
    HDC hDC_Target = GetDC(hWnd_Target);

    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    SelectObject(hDC_Desktop, hPen);

    // 设置填充为透明
    SetBkMode(hDC_Desktop, TRANSPARENT);

    // 绘制三个白色方框
    Rectangle(hDC_Desktop, 100, 100, 200, 200);
    Rectangle(hDC_Desktop, 300, 100, 400, 200);
    Rectangle(hDC_Desktop, 500, 100, 600, 200);

    // 释放设备上下文
    ReleaseDC(NULL, hDC_Desktop);
    DeleteObject(hPen);

    return 0;
}
