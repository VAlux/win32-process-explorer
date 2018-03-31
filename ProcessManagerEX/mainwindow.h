#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QTimer>
#include "executer.h"
#include "processmodel.h"

namespace Ui {
class MainWindow;
}

enum RefreshFrequency{
    NORMALRF = 2500,
    SLOWRF   = 5000,
    HIGHRF   = 1000
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void refreshModel();
    void filterTextChanged();

    void on_actionExit_triggered();
    void on_actionTest_Kill_triggered();
    void on_tvProcessTable_clicked(const QModelIndex &index);
    void on_action_Normal_triggered();
    void on_action_Slow_triggered();
    void on_action_High_triggered();
    void on_action_Pause_triggered();
    void on_action_Refresh_triggered();
    void on_actionCreate_Process_triggered();
    void on_actionEnable_Debug_Privilege_triggered();
    void on_actionKill_Process_Tree_triggered();
    void on_actionAbove_Normal_triggered();
    void on_actionBelow_Normal_triggered();
    void on_actionHigh_triggered();
    void on_actionIdle_triggered();
    void on_actionNoemal_triggered();
    void on_actionAbout_triggered();

private:
    QString _getSelectedProcessName() const;
    int     _getSelectedProcessID() const;
    void    _tweakUI();

    Ui::MainWindow          *ui;
    executer                *ExecDialog;
    Win32ProcessExplorer    *_Explorer;
    ProcessModel            *_ProcModel;
    QSortFilterProxyModel   *_ProxyModel;
    QTimer                  *_RefreshTimer;
    int                     _SelectedIndex;
    bool                    _DebugPrivEnabled;
};

#endif // MAINWINDOW_H
