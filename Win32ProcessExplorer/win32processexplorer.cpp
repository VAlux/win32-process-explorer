#include "win32processexplorer.h"


Win32ProcessExplorer::Win32ProcessExplorer()
{
    Refresh();
    SysInfo = new SYSTEM_INFO();
    GetSystemInfo(SysInfo);
}

QMap<int, QString> *Win32ProcessExplorer::enumerateProcesses()
{
    return &_ProcesesMap;
}

/**
 * @brief fill the process map with currently running processes.
 * @return List of pointers to process names.
 */
QList<QString> Win32ProcessExplorer::enumerateProcessNames() const
{
    return _ProcesesMap.values();
}

/**
 * @brief Fills the process map.
 */
void Win32ProcessExplorer::Refresh()
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == NULL)
        return;

    PROCESSENTRY32 proc;
    proc.dwSize = sizeof(proc);

    _ProcesesMap.clear();
    if (Process32First(hSnap, &proc))
    {
        while (Process32Next(hSnap, &proc))
            _ProcesesMap[proc.th32ProcessID] = QString::fromWCharArray(proc.szExeFile);
    }
    CloseHandle(hSnap);
}

/**
 * @brief get the list of threads id's of specified process.
 * @param procID Process identifier.
 * @return List of process threads id's.
 */
QList<DWORD> Win32ProcessExplorer::enumarateThreads(const DWORD procID) const
{
    HANDLE pThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, procID);
    bool isOk = false;
    THREADENTRY32 thrEntry;
    QList<DWORD> ThreadsIDList;

    thrEntry.dwSize = sizeof(THREADENTRY32);
    isOk = Thread32First(pThreadSnap, &thrEntry);

    while(isOk)
    {
        if(thrEntry.th32OwnerProcessID == procID)
            ThreadsIDList.append(thrEntry.th32ThreadID);

        isOk = Thread32Next(pThreadSnap, &thrEntry);
    }
    return ThreadsIDList;
}

/**
 * @brief enumerate process modules.
 * @param procID process identifyer.
 * @return list of process modules.
 */
QList<QString> Win32ProcessExplorer::enumarateModules(const DWORD procID) const
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    WCHAR szModName[MAX_PATH];

    QList<QString> modList;
    hProcess = _getProcessFullHandle(procID);
    if(!hProcess)
        return modList;

    if(EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for(unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); ++i)
        {
            if(GetModuleFileNameEx(hProcess, hMods[i], szModName,
                                   sizeof(szModName) / sizeof(WCHAR)))
            modList.append(QString::fromWCharArray(szModName));
        }
    }
    CloseHandle(hProcess);
    return modList;
}

/**
 * @brief kills Process By specified Name.
 * @param procName process name to kill.
 */
void Win32ProcessExplorer::killProcessByName(const QString procName)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(PROCESSENTRY32);
    BOOL hRes = Process32First(hSnap, &pEntry);
    while(hRes)
    {
        if(QString::fromWCharArray(pEntry.szExeFile) == procName)
        {
            HANDLE hProc = OpenProcess(PROCESS_TERMINATE, 0, pEntry.th32ProcessID);

            if(hProc != NULL)
                TerminateProcess(hProc, 9);

            CloseHandle(hProc);
        }
        hRes = Process32Next(hSnap, &pEntry);
    }
    CloseHandle(hSnap);
}

/**
 * @brief kills Process By specified process ID.
 * @param procName process ID to kill.
 */
void Win32ProcessExplorer::killProcessByID(const DWORD procID)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(PROCESSENTRY32);
    BOOL hRes = Process32First(hSnap, &pEntry);
    while(hRes)
    {
        if(pEntry.th32ProcessID == procID)
        {
            HANDLE hProc = OpenProcess(PROCESS_TERMINATE, 0, procID);
            if(hProc != NULL)
            {
                TerminateProcess(hProc, 9);
                CloseHandle(hProc);
            }
        }
        hRes = Process32Next(hSnap, &pEntry);
    }
    CloseHandle(hSnap);
}

/**
 * @brief Kill the specified process and all of his children.
 * @param procID parent process ID.
 */
void Win32ProcessExplorer::killProcessTree(const DWORD procID)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if(Process32First(hSnap, &entry))
    {
        BOOL isOk = TRUE;
        while(isOk)
        {
            if(entry.th32ParentProcessID == procID)
            {
                HANDLE hChild = _getProcessFullHandle(entry.th32ParentProcessID);
                if(hChild)
                {
                    TerminateProcess(hChild, 1);
                    CloseHandle(hChild);
                }
            }
            isOk = Process32Next(hSnap, &entry);
        }
    }
    HANDLE hProc = _getProcessFullHandle(procID);
    if(hProc)
    {
        TerminateProcess(hProc, 1);
        CloseHandle(hProc);
    }
}

/**
 * @brief get working set size of process memory usage.
 * @param procID specified process ID.
 * @return specified process memory usage.
 */
int Win32ProcessExplorer::getProcessMemoryUsage(const DWORD procID)
{
    PROCESS_MEMORY_COUNTERS counters;
    HANDLE procHandle = _getProcessFullHandle(procID);

    if(GetProcessMemoryInfo(procHandle, &counters, sizeof(counters)))
        return counters.WorkingSetSize;

    return -1;
}

/**
 * @brief Executes specified command
 * @param ProcName command
 */
void Win32ProcessExplorer::createProcess(const QString ProcName)
{
    ShellExecute(NULL, L"open", (LPCWSTR)ProcName.utf16(), NULL, NULL, SW_NORMAL);
}

/**
 * @brief for getting handles for any sys. object
 * @return success | fail
 */
BOOL Win32ProcessExplorer::EnableDebugPrivilege(bool enabled)
{
    HANDLE            hToken;
    LUID              lpLuid;
    TOKEN_PRIVILEGES  NewState;

    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        if(LookupPrivilegeValue(NULL, L"SeDebugPrivilege", &lpLuid))
        {
            NewState.PrivilegeCount = 1;
            NewState.Privileges[0].Luid = lpLuid;
            if(enabled)
                NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges (hToken, FALSE, &NewState, sizeof(NewState), NULL, NULL);
            return TRUE;
        }
        CloseHandle (hToken);
    }
    return FALSE;
}

int Win32ProcessExplorer::getTotalMemUsage() const
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return statex.dwMemoryLoad;
}

int Win32ProcessExplorer::getProcessBaseAddress(const DWORD ProcID)
{
    HANDLE hProc = _getProcessSecureHandle(ProcID);
    MODULEINFO miInfo;
    if (GetModuleInformation(hProc, NULL, &miInfo, sizeof(miInfo)))
        return (DWORD)miInfo.EntryPoint;
    else
        return -1;
}

int Win32ProcessExplorer::getProcessAmount() const
{
    return this->_ProcesesMap.size();
}

int Win32ProcessExplorer::getThreadsAmount(const DWORD procID) const
{
    return enumarateThreads(procID).size();
}

QString Win32ProcessExplorer::getProcessName(const int ProcessId) const
{
    return this->_ProcesesMap[ProcessId];
}

/**
 * @brief get owner domain and user name of the specified process.
 * @param ProcID Process Identifier.
 * @return owner domain and user name.
 */
QString Win32ProcessExplorer::getProcessOwner(const DWORD ProcID) const
{
    HANDLE hProc = _getProcessFullHandle(ProcID);
    HANDLE hProcessToken;
    OpenProcessToken(hProc, TOKEN_READ, &hProcessToken);
    DWORD dwProcessTokenInfoSize = 0;
    QString qsUsrName = "SYSTEM SERVICE";

    GetTokenInformation(hProcessToken, TokenUser, NULL, 0, &dwProcessTokenInfoSize);
    PTOKEN_USER pUsrToken = reinterpret_cast<PTOKEN_USER>(new BYTE[dwProcessTokenInfoSize]);
    if(pUsrToken != NULL)
    {
        if(GetTokenInformation(hProcessToken, TokenUser, pUsrToken, dwProcessTokenInfoSize, &dwProcessTokenInfoSize))
        {
            SID_NAME_USE   snuSIDNameUse;
            TCHAR          szUser[MAX_PATH];
            TCHAR          szDomain[MAX_PATH];
            DWORD          dwUserNameLength = MAX_PATH;
            DWORD          dwDomainNameLength = MAX_PATH;
            if(LookupAccountSid(NULL,
                                pUsrToken->User.Sid,
                                szUser,
                                &dwUserNameLength,
                                szDomain,
                                &dwDomainNameLength,
                                &snuSIDNameUse))
            {
                qsUsrName = QString::fromWCharArray(szDomain) + "\\" +
                            QString::fromWCharArray(szUser);
                CloseHandle(hProcessToken);
            }
        }
    }
    delete [] pUsrToken;
    return qsUsrName;
}

QString Win32ProcessExplorer::getProcessPriority(const DWORD ProcID) const
{
    HANDLE hProc = _getProcessSecureHandle(ProcID);
    DWORD dwPriClass = GetPriorityClass(hProc);
    switch(dwPriClass)
    {
        case ABOVE_NORMAL_PRIORITY_CLASS:
            return "Above Normal";
        case BELOW_NORMAL_PRIORITY_CLASS:
            return "Below normal";
        case HIGH_PRIORITY_CLASS:
            return "High";
        case IDLE_PRIORITY_CLASS:
            return "Idle";
        case NORMAL_PRIORITY_CLASS:
            return "Normal";
        case REALTIME_PRIORITY_CLASS:
            return "Realtime";
        default:
            return "Unknown";
    }
}

QString Win32ProcessExplorer::getProcessorArchitecture() const
{
    switch(this->SysInfo->wProcessorArchitecture)
    {
        case PROCESSOR_ARCHITECTURE_AMD64:
            return "AMD x64";
        case PROCESSOR_ARCHITECTURE_ARM:
            return "ARM";
        case PROCESSOR_ARCHITECTURE_INTEL:
            return "Intel x86";
        case PROCESSOR_ARCHITECTURE_UNKNOWN:
            return "Unknown";
        default:
            return "Unsupported";
    }
}

void Win32ProcessExplorer::setProcessPriority(const DWORD ProcID, const DWORD PriClass)
{
    HANDLE hProc = _getProcessFullHandle(ProcID);
    SetPriorityClass(hProc, PriClass);
}

int Win32ProcessExplorer::getProcessorsAmount() const
{
    return this->SysInfo->dwNumberOfProcessors;
}

int Win32ProcessExplorer::getPageSize() const
{
    return this->SysInfo->dwPageSize;
}

int Win32ProcessExplorer::getProcessID(const QString procName) const
{
    return this->_ProcesesMap.key(procName);
}

int Win32ProcessExplorer::getProcessID(const int position) const
{
    return this->_ProcesesMap.keys().at(position);
}

HANDLE Win32ProcessExplorer::_getProcessSecureHandle(const DWORD ProcID) const
{
    return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcID);
}

HANDLE Win32ProcessExplorer::_getProcessFullHandle(const DWORD ProcID) const
{
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcID);
}
