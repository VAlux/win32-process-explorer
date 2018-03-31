#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>
#include <QProxyStyle>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _SelectedIndex(0),
    _DebugPrivEnabled(true)
{
    ui->setupUi(this);
    _tweakUI();
    _Explorer = new Win32ProcessExplorer();
    _Explorer->EnableDebugPrivilege(_DebugPrivEnabled); // for system processes(disabled by default).
    _ProcModel = new ProcessModel(this, _Explorer);
    _ProxyModel = new QSortFilterProxyModel(this);
    _ProcModel->refill();
    _ProxyModel->setSourceModel(_ProcModel); // set proxy.
    _ProxyModel->setFilterKeyColumn(2);
    _RefreshTimer = new QTimer(this);
    ui->tvProcessTable->setModel(_ProxyModel);

//SIGNAL - SLOT connections:
    connect(_RefreshTimer, SIGNAL(timeout()), this, SLOT(refreshModel())); // model refreshing
    connect(ui->tbFilterBox, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged())); // model filtering
    connect(ui->btnCaseSensetive, SIGNAL(clicked()), this, SLOT(filterTextChanged()));
//
    _RefreshTimer->start(NORMALRF);
}

MainWindow::~MainWindow()
{
    delete _ProcModel;
    delete ui;
}

void MainWindow::on_tvProcessTable_clicked(const QModelIndex &index)
{
    _SelectedIndex = index.row();
    ui->tvProcessTable->selectRow(_SelectedIndex);
    ui->listWidget->clear();
    QList<QString> ModulesList = _ProcModel->getProcModules(_getSelectedProcessID());
    ui->lcdModulesAmount->display(ModulesList.size());
    ui->lcdThreadsAmount->display(_Explorer->getThreadsAmount(_getSelectedProcessID()));
    ui->gbStats->setTitle("[ " + _getSelectedProcessName() + " ] process statistics: ");

    for(int i = 0; i < ModulesList.size(); ++i)
        ui->listWidget->addItem(ModulesList[i]);
}

void MainWindow::refreshModel()
{
    _ProcModel->refill();
    ui->tvProcessTable->selectRow(_SelectedIndex);
//total processes amount + Idle, audiodg and System proceses.
    ui->statusBar->showMessage(QString::number(
                                   _ProcModel->getProcessAmount() + 3) +  " Processes");
    ui->pbMemUsage->setValue(_Explorer->getTotalMemUsage());
}

void MainWindow::on_actionTest_Kill_triggered()
{
    _ProcModel->removeProcess(_getSelectedProcessID());
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_action_Normal_triggered()
{
    _RefreshTimer->setInterval(NORMALRF);
    _RefreshTimer->start();
}

void MainWindow::on_action_Slow_triggered()
{
    _RefreshTimer->setInterval(SLOWRF);
    _RefreshTimer->start();
}

void MainWindow::on_action_High_triggered()
{
    _RefreshTimer->setInterval(HIGHRF);
    _RefreshTimer->start();
}

void MainWindow::on_action_Pause_triggered()
{
    _RefreshTimer->stop();
}

void MainWindow::on_action_Refresh_triggered()
{
    refreshModel();
}

void MainWindow::on_actionCreate_Process_triggered()
{
    ExecDialog = new executer(this, _Explorer);
    ExecDialog->show();
}

void MainWindow::on_actionEnable_Debug_Privilege_triggered()
{
    _DebugPrivEnabled = !_DebugPrivEnabled;
    _Explorer->EnableDebugPrivilege(_DebugPrivEnabled);
}

void MainWindow::on_actionKill_Process_Tree_triggered()
{
   _ProcModel->removeProcessTree(_getSelectedProcessID());
}

void MainWindow::on_actionAbove_Normal_triggered()
{
    _Explorer->setProcessPriority(_getSelectedProcessID(),
                                  ABOVE_NORMAL_PRIORITY_CLASS);
}

void MainWindow::on_actionBelow_Normal_triggered()
{
    _Explorer->setProcessPriority(_getSelectedProcessID(),
                                  BELOW_NORMAL_PRIORITY_CLASS);
}

void MainWindow::on_actionHigh_triggered()
{
    _Explorer->setProcessPriority(_getSelectedProcessID(),
                                  HIGH_PRIORITY_CLASS);
}

void MainWindow::on_actionIdle_triggered()
{
    _Explorer->setProcessPriority(_getSelectedProcessID(),
                                  IDLE_PRIORITY_CLASS);
}

void MainWindow::on_actionNoemal_triggered()
{
    _Explorer->setProcessPriority(_getSelectedProcessID(),
                                  NORMAL_PRIORITY_CLASS);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About ProcessManagerEX",
                       "Created by Alexander Voevodin");
}

void MainWindow::filterTextChanged()
{
    QRegExp::PatternSyntax syntax = QRegExp::PatternSyntax(ui->cbFilterType->currentIndex());
    Qt::CaseSensitivity caseSens = ui->btnCaseSensetive->isChecked() ? Qt::CaseSensitive
                                                                     : Qt::CaseInsensitive;
    QRegExp regExp(ui->tbFilterBox->text(), caseSens, syntax);
    _ProxyModel->setFilterRegExp(regExp);
}

QString MainWindow::_getSelectedProcessName() const
{
    return _ProxyModel->index(_SelectedIndex,2).data().toString();
}

int MainWindow::_getSelectedProcessID() const
{    
    return  _ProcModel->getProcess(_getSelectedProcessName())->getID();
}

void MainWindow::_tweakUI()
{
    ui->tvProcessTable->setWordWrap(true);
    ui->tvProcessTable->setShowGrid(false);
    ui->tvProcessTable->setAutoScroll(false);
    ui->tvProcessTable->setSortingEnabled(false);
    ui->tvProcessTable->verticalHeader()->setStretchLastSection(false);
    ui->lcdModulesAmount->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdThreadsAmount->setSegmentStyle(QLCDNumber::Flat);
    ui->cbFilterType->addItem("Regex");
    ui->cbFilterType->addItem("String");
}
