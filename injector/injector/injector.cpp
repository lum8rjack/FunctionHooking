#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
using namespace std;

int getProcId(const wchar_t* target)
{
    DWORD pID = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    do {
        if (wcscmp(pe32.szExeFile, target) == 0) {
            CloseHandle(hSnapshot);
            pID = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    return pID;
}

int main(int argc, char* argv[])
{

    if (argc != 3)
    {
        cout << "injector.exe <program to inject into> <dll to inject>";
        return 1;
    }
 
    char* process = argv[1];
    size_t newsize = strlen(process) + 1;
    wchar_t* wcstring = new wchar_t[newsize];
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wcstring, newsize, process, _TRUNCATE); 
    int pID = getProcId(wcstring);

    char* dll = argv[2];
    char dllPath[MAX_PATH] = { 0 };
    GetFullPathNameA(dll, MAX_PATH, dllPath, NULL);

    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pID);
    LPVOID pszLibFileRemote = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    WriteProcessMemory(hProcess, pszLibFileRemote, dllPath, strlen(dllPath) + 1, NULL);
    HANDLE handleThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, NULL, NULL);

    WaitForSingleObject(handleThread, INFINITE);
    CloseHandle(handleThread);
    VirtualFreeEx(hProcess, dllPath, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return 0;
}

