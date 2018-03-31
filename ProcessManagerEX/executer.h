#ifndef EXECUTER_H
#define EXECUTER_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include "win32processexplorer.h"

namespace Ui {
class executer;
}

class executer : public QDialog
{
    Q_OBJECT
    
public:
    explicit executer(QWidget *parent = 0,
                      Win32ProcessExplorer *explorer = 0);
    ~executer();
    
private slots:
    void on_btnExecute_clicked();
    void on_btnClose_clicked();

    void on_twHelperList_clicked(const QModelIndex &);

private:
    Ui::executer *ui;
    Win32ProcessExplorer *_Explorer;
    void loadHelperList();
    void tweakUI();
};

#endif // EXECUTER_H
