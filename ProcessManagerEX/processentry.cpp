#include <QtCore>
#include "processentry.h"


ProcessEntry::ProcessEntry(const QString pName, const int ID, const int mUsage, int BaseAddr) :
    _ProcessName(pName),
    _ID(ID),
    _MemUsage(mUsage),
    _BaseAddr(BaseAddr)
{}

ProcessEntry::~ProcessEntry()
{
    qDeleteAll(_Modules);
}

ProcessEntry::ProcessEntry(const QString pName, const int pID) :
    _ProcessName(pName),
    _ID(pID)
{}

void ProcessEntry::setProcName(const QString name)
{
    this->_ProcessName = name;
}

void ProcessEntry::setUserName(const QString name)
{
    this->_UserName = name;
}

void ProcessEntry::setCPULoad(const int load)
{
    this->_CPULoad = load;
}

void ProcessEntry::setMemUsage(const int mUsage)
{
    this->_MemUsage = mUsage;
}

void ProcessEntry::setBaseAddr(const int bAddr)
{
    this->_BaseAddr = bAddr;
}

void ProcessEntry::setPriority(const QString PriClass)
{
    this->_PriorityClass = PriClass;
}

void ProcessEntry::setDescription(const QString desc)
{
    this->_Description = desc;
}

QString ProcessEntry::getProcName() const
{
    return this->_ProcessName;
}

QString ProcessEntry::getPriotity() const
{
    return this->_PriorityClass;
}

QString ProcessEntry::getUserName() const
{
    return this->_UserName;
}

QString ProcessEntry::getDescription() const
{
    return this->_Description;
}

QString ProcessEntry::getHexID() const
{
    QString str;
    return str.sprintf("%02X", _ID);
}

QString ProcessEntry::getHexBaseAddress() const
{
    QString str;
    return str.sprintf("%02X", _BaseAddr);
}

int ProcessEntry::getCPULoad() const
{
    return this->_CPULoad;
}

int ProcessEntry::getID() const
{
    return this->_ID;
}

int ProcessEntry::getMemoryUsage() const
{
    return this->_MemUsage;
}

int ProcessEntry::getBaseAddr() const
{
    return this->_BaseAddr;
}
