#include "fontlistwindow.h"
#include "ui_fontlistwindow.h"
#include "repository/skinrepository.h"

FontListWindow::FontListWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FontListWindow)
    , mModel(SkinRepository::fonts())
{
    ui->setupUi(this);
    ui->tableView->setModel(mModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->removeButton, &QPushButton::clicked, this, &FontListWindow::remove);
    connect(ui->newButton, &QPushButton::clicked, this, &FontListWindow::addDefault);
}

FontListWindow::~FontListWindow()
{
    delete ui;
}

void FontListWindow::remove()
{
    QList<int> rows;
    for (auto const &index : ui->tableView->selectionModel()->selectedRows()) {
        rows.append(index.row());
    }
    // from bigger to smaller
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (int r : rows) {
        mModel->removeRow(r);
    }
}

void FontListWindow::addDefault()
{
    bool ok;
    int i = 0;
    do {
        auto f = Font(QString("Untitled%1").arg(i), QString());
        ok = SkinRepository::fonts()->append(f);
        i++;
    } while(!ok);
}
