#include "outputslistwindow.hpp"
#include "ui_outputslistwindow.h"
#include "model/outputsmodel.hpp"

OutputsListWindow::OutputsListWindow(OutputsModel* model, QWidget* parent)
    : QDialog(parent)
    , mModel(model)
    , ui(new Ui::OutputsListWindow)
{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    connect(ui->addButton, &QPushButton::clicked, this, &OutputsListWindow::addDefault);
    connect(ui->removeButton, &QPushButton::clicked, this, &OutputsListWindow::removeSelected);
}

OutputsListWindow::~OutputsListWindow()
{
    delete ui;
}

void OutputsListWindow::addDefault()
{
    bool ok;
    int i = 0;
    do {
        auto item = VideoOutput(QString("%1").arg(i), VideoOutputData{ QSize(1280, 720), 32 });
        ok = mModel->append(item);
        i++;
    } while (!ok);
}

void OutputsListWindow::removeSelected()
{
    auto index = ui->tableView->selectionModel()->currentIndex();
    mModel->removeRow(index.row());
}
