#ifndef PROCESSENTRY_H
#define PROCESSENTRY_H
#include <QString>

class ProcessEntry
{
public:
    ProcessEntry();
    ProcessEntry(const QString pName, const int pID);
    ProcessEntry(const QString pName, const int ID, const int mUsage, int BaseAddr);
    ~ProcessEntry();

    //setters//
    void setProcName(const QString name);
    void setUserName(const QString name);
    void setDescription(const QString desc);
    void setCPULoad(const int load);
    void setMemUsage(const int mUsage);
    void setBaseAddr(const int bAddr);
    void setPriority(const QString PriClass);


    //getters//
    QString getProcName() const;
    QString getPriotity() const;
    QString getUserName() const;
    QString getDescription() const;
    QString getHexID() const;
    QString getHexBaseAddress() const;
    int     getCPULoad() const;
    int     getID() const;
    int     getMemoryUsage() const;
    int     getBaseAddr() const;

private:
    QList<QString*> _Modules;
    QString _ProcessName;
    QString _UserName;
    QString _Description;
    QString _PriorityClass;
    int _ID;
    int _CPULoad;
    int _MemUsage;
    int _BaseAddr;
};

#endif // PROCESSENTRY_H
