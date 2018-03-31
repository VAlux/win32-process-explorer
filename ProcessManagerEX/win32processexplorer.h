#ifndef WIN32PROCESSEXPLORER_H
#define WIN32PROCESSEXPLORER_H

#include "win32processexplorer_global.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <QMap>
#include <QString>

class WIN32PROCESSEXPLORERSHARED_EXPORT Win32ProcessExplorer
{

public:
    Win32ProcessExplorer();

    QMap<int, QString>* enumerateProcesses();
    QList<QString>      enumerateProcessNames() const;
    QList<DWORD>        enumarateThreads(const DWORD procID) const;
    QList<QString>      enumarateModules(const DWORD procID) const;

    QString getProcessName(const int IdProcess) const;
    QString getProcessOwner(const DWORD ProcID) const;
    QString getProcessPriority(const DWORD ProcID) const;
    int     getProcessID(const QString ProcName) const;
    int     getProcessID(const int Position) const;
    int     getTotalMemUsage() const;
    int     getProcessBaseAddress(const DWORD ProcID);
    int     getProcessMemoryUsage(const DWORD procID);

// not implemented!!
    int     getProcessCpuUsage(const DWORD procID);
    int     getCpuUsage();
////
    void createProcess(const QString ProcName);
    void setProcessPriority(const DWORD ProcID, const DWORD PriClass);
    void killProcessByName(const QString procName);
    void killProcessByID(const DWORD procID);
    void killProcessTree(const DWORD procID);

    int getProcessAmount() const;
    int getThreadsAmount(const DWORD procID) const;

    void Refresh();
    BOOL EnableDebugPrivilege(bool enable);

// System info additional features:
    int getProcessorsAmount() const;
    int getPageSize() const;
    QString getProcessorArchitecture() const;
////

private:
    QMap<int, QString> _ProcesesMap;
    HANDLE _getProcessSecureHandle(const DWORD ProcID) const;
    HANDLE _getProcessFullHandle(const DWORD ProcID) const;
    SYSTEM_INFO *SysInfo;
};

#endif // WIN32PROCESSEXPLORER_H
