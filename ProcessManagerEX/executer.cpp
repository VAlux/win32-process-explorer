#include "executer.h"
#include "ui_executer.h"


executer::executer(QWidget *parent, Win32ProcessExplorer *explorer) :
    QDialog(parent),
    ui(new Ui::executer)
{
    ui->setupUi(this);
    tweakUI();
    this->_Explorer = explorer;
    loadHelperList();
}

executer::~executer()
{
    delete ui;
}

void executer::on_btnExecute_clicked()
{
    _Explorer->createProcess(ui->tbAppName->text());
    this->close();
}

void executer::on_btnClose_clicked()
{
    this->close();
}

void executer::loadHelperList()
{
    QFile file("helperlist.txt");
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(this, "Error", file.errorString());
        return;
    }

    QTextStream input(&file);
    QStringList EntryList;

    while(!input.atEnd())
    {
        EntryList = input.readLine().split('|');
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->twHelperList);
        item->setText(0, EntryList.at(0));
        item->setText(1, EntryList.at(1));
        ui->twHelperList->addTopLevelItem(item);
    }
    file.close();
}

void executer::tweakUI()
{
    ui->twHelperList->setColumnCount(2);
    ui->twHelperList->setHeaderLabels(QStringList() << "Name" << "Command");
    ui->twHelperList->setColumnWidth(0, width() / 2);
    ui->twHelperList->setColumnWidth(1, width() / 2);
}

void executer::on_twHelperList_clicked(const QModelIndex &)
{
    ui->tbAppName->setText(
                  ui->twHelperList->currentItem()->data(1, Qt::DisplayRole).toString()
                );
}
