#include "processmodel.h"
#include <QDebug>

ProcessModel::ProcessModel(QObject *parent, Win32ProcessExplorer *explorer) :
    QAbstractTableModel(parent),
    _Sections(6)
{
    this->_Explorer = explorer;
}

/**
 * @brief add new process.
 * @param entry process to add.
 */
void ProcessModel::appendProcess(ProcessEntry* entry)
{
    beginInsertRows(QModelIndex(),
                    _processes.length(),
                    _processes.length() + 1);
    this->_processes.push_back(entry);
    endInsertRows();
}

/**
 * @brief remove process with specified name.
 * @param pName process name to remove.
 */
void ProcessModel::removeProcess(const QString pName)
{
    beginRemoveRows(QModelIndex(), 0, _processes.length());
    for (int i = 0; i < _processes.size(); ++i) {
        if(_processes[i]->getProcName() == pName)
        {
            delete _processes[i];
            _processes.removeAt(i);
        }
    }    
    endRemoveRows();
}

void ProcessModel::removeProcess(const int ProcID)
{
    _Explorer->killProcessByID(ProcID);
    int index = _getProcIndexByID(ProcID);

    if(index == -1)
        return;

    delete _processes[index];
    beginRemoveRows(QModelIndex(),index, index + 1);
        _processes.removeAt(index);
    endRemoveRows();
}
// TODO: test more!!
void ProcessModel::removeProcessTree(const int ParentProcIndex)
{
    _Explorer->killProcessTree(ParentProcIndex);
    refill();
}

/**
 * @brief Update the process model.
 */
void ProcessModel::refill()
{
    _Explorer->Refresh();
    qDeleteAll(_processes);

    beginResetModel(); // reset started.
    _processes.clear();
    ProcessEntry *entry;
    int iBaseAddr;
    QMapIterator<int, QString> i(*_Explorer->enumerateProcesses());
    while(i.hasNext())
    {
        i.next();

        iBaseAddr = _Explorer->getProcessBaseAddress(i.key());
        if(iBaseAddr == -1)
            continue;

        entry = new ProcessEntry(i.value(), i.key());
        entry->setBaseAddr(iBaseAddr);
        entry->setUserName(_Explorer->getProcessOwner(i.key()));
        entry->setMemUsage(_Explorer->getProcessMemoryUsage(i.key()) / 1024); // in Kbytes.
        entry->setPriority(_Explorer->getProcessPriority(i.key()));
        _processes.append(entry);
    }
    endResetModel(); // reset ended.
}

QList<QString> ProcessModel::getProcModules(const int ProcID) const
{
    return _Explorer->enumarateModules(ProcID);
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
            case 0:
                    return "0x" + _processes[index.row()]->getHexBaseAddress();
                break;
            case 1:
                    return QString::number(_processes[index.row()]->getID()) +
                            " (0x" +_processes[index.row()]->getHexID() + ")";
                break;
            case 2:
                    return QVariant(_processes[index.row()]->getProcName());
                break;
            case 3:
                    return QString::number(_processes[index.row()]->getMemoryUsage()) + " Kb";
                break;
            case 4:
                    return QVariant(_processes[index.row()]->getPriotity());
                break;
            case 5:
                    return QVariant(_processes[index.row()]->getUserName());
                break;
        default:
                break;
        }
    }
    return QVariant();
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            switch(section)
            {
                case 0: return tr("Base Address");
                case 1: return tr("ID");
                case 2: return tr("Name");
                case 3: return tr("Memory");
                case 4: return tr("Priority");
                case 5: return tr("User");
            }
        }
        else return section + 1;
    }
    return QVariant();
}

int ProcessModel::_getProcIndexByID(const int ProcID)
{
    int index;
    for(index = 0; index < _processes.length(); ++index)
        if(_processes[index]->getID() == ProcID)
            return index;
    return -1;
}

ProcessEntry *ProcessModel::getProcess(const int index) const
{
    return this->_processes[index];
}

ProcessEntry *ProcessModel::getProcess(const QString ProcName) const
{
    for(ProcessEntry *proc: _processes)
        if(proc->getProcName() == ProcName)
            return proc;
    return nullptr;
}

int ProcessModel::getProcessAmount() const
{
    return this->_processes.length();
}

int ProcessModel::rowCount(const QModelIndex &) const
{
    return this->_processes.size();
}

int ProcessModel::columnCount(const QModelIndex &) const
{
    return this->_Sections;
}
