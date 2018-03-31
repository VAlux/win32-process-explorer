#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H
#include <QAbstractTableModel>
#include <QList>
#include "processentry.h"
#include "win32processexplorer.h"

class ProcessModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ProcessModel(QObject *parent = 0, Win32ProcessExplorer *explorer = 0);

    void appendProcess(ProcessEntry* entry);
    void removeProcess(const QString pName);
    void removeProcess(const int ProcID);
    void removeProcessTree(const int ParentProcIndex);
    void refill();
    int getProcessAmount() const;
    ProcessEntry* getProcess(const int index) const;
    ProcessEntry* getProcess(const QString ProcName) const;
    QList<QString> getProcModules(const int ProcID) const;

signals:
    
public slots:

private:
// Overloaded methods:
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    int _getProcIndexByID(const int ProcID);

    int _Sections; // columns
    Win32ProcessExplorer *_Explorer;
    QList<ProcessEntry *> _processes;
};

#endif // PROCESSMODEL_H
