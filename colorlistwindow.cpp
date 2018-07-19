#include <QMessageBox>
#include <QDebug>
#include "colorlistwindow.hpp"
#include "ui_colorlistwindow.h"
#include "model/colorsmodel.hpp"
#include "repository/skinrepository.hpp"

#include <QtColorWidgets/ColorDialog>

ColorListWindow::ColorListWindow(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::ColorListWindow)
    , mModel(SkinRepository::colors())
{
    ui->setupUi(this);

//    connect(ui->addButton, &QPushButton::clicked, this, &ColorListWindow::add);
    connect(ui->removeButton, &QPushButton::clicked, this, &ColorListWindow::remove);
//    connect(ui->renameButton, &QPushButton::clicked, this, &ColorListWindow::rename);
    connect(ui->newButton, &QPushButton::clicked, this, &ColorListWindow::addDefault);

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &ColorListWindow::currentChanged);

    ui->tableView->setModel(mModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    mMapper = new QDataWidgetMapper(this);
    mMapper->setModel(mModel);
    mMapper->addMapping(ui->name, ColorsModel::ColumnName);
    mMapper->addMapping(ui->value, ColorsModel::ColumnValue);
    mMapper->addMapping(ui->wheel, ColorsModel::ColumnValue, "color");
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            mMapper, &QDataWidgetMapper::setCurrentModelIndex);

    connect(ui->wheel, &color_widgets::ColorWheel::colorChanged, [this] (QColor col) {
        mMapper->submit();
        update_widgets();
    });

    QWidget *w = new color_widgets::ColorDialog(this, Qt::Widget);
    ui->dockWidgetContents->layout()->addWidget(w);
}

ColorListWindow::~ColorListWindow()
{
    delete ui;
}

void ColorListWindow::update_widgets()
{
    QColor c = color();

    ui->slider_red->setValue(c.red());
    ui->spin_red->setValue(ui->slider_red->value());
    ui->slider_green->setValue(c.green());
    ui->spin_green->setValue(ui->slider_green->value());
    ui->slider_blue->setValue(c.blue());
    ui->spin_blue->setValue(ui->slider_blue->value());
}

QColor ColorListWindow::color()
{
    return ui->wheel->color();
}

void ColorListWindow::remove()
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

void ColorListWindow::addDefault()
{
    bool ok;
    int i = 0;
    do {
        auto c = Color(QString("Untitled%1").arg(i), QRgb());
        ok = SkinRepository::instance().colors()->append(c);
        i++;
    } while(!ok);
}

void ColorListWindow::add()
{
    const QString name = ui->name->text();
    if (confirmAdd(name)) {
        Color c = Color(ui->name->text(), currentColor());
        SkinRepository::instance().colors()->append(c);
    }
}

void ColorListWindow::rename()
{
    QString const name = ui->name->text();
    if (confirmAdd(name)) {
        QModelIndex index = mModel->index(mCurrentIndex.row(), ColorsModel::ColumnName);
        mModel->setData(index, name);
    }
}

void ColorListWindow::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    mCurrentIndex = current;
    Q_UNUSED(previous)
}

QRgb ColorListWindow::currentColor() const
{
    return ui->wheel->color().rgba();
}

bool ColorListWindow::confirmAdd(const QString &name)
{
    bool has = SkinRepository::colors()->contains(name);
    if (!has) {
        return true;
    }
    int ret = QMessageBox::warning(this, tr("Warning!"),
                                   tr("Color with name '%1' already exists. Overwrite?").arg(name),
                                   QMessageBox::Abort | QMessageBox::Yes);
    return ret == QMessageBox::Yes;
}
