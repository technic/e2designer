#include "colorlistwindow.hpp"
#include "model/colorsmodel.hpp"
#include "repository/skinrepository.hpp"
#include "base/flagsetter.hpp"
#include "ui_colorlistwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <array>
#include <QtColorWidgets/color_line_edit.hpp>
#include <QtColorWidgets/color_dialog.hpp>
#include <QtColorWidgets/ColorSelector>

class ColorColumnDelegate : public QStyledItemDelegate
{
public:
    ColorColumnDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {}

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        auto itemOption = option;
        // Disable selection to keep background color
        itemOption.state.setFlag(QStyle::State_Selected, false);
        QStyledItemDelegate::paint(painter, itemOption, index);
    }
};

ColorListWindow::ColorListWindow(QWidget* parent)
    : QDockWidget(parent)
    , ui(new Ui::ColorListWindow)
    , mModel(SkinRepository::colors())
    , m_updating(false)
    , mMapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);

    connect(ui->removeButton, &QPushButton::clicked, this, &ColorListWindow::remove);
    connect(ui->newButton, &QPushButton::clicked, this, &ColorListWindow::addDefault);
    connect(ui->upButton, &QToolButton::clicked, this, &ColorListWindow::moveUp);
    connect(ui->downButton, &QToolButton::clicked, this, &ColorListWindow::moveDown);

    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this,
            &ColorListWindow::currentChanged);

    ui->tableView->setModel(mModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tableView->setDropIndicatorShown(true);
    ui->tableView->setItemDelegateForColumn(ColorsModel::ColumnColor,
                                            new ColorColumnDelegate(this));

    // Color selectors
    using namespace color_widgets;
    // Rgb
    connect(ui->slider_red, &GradientSlider::valueChanged, this, &ColorListWindow::setRgb);
    connect(ui->slider_green, &GradientSlider::valueChanged, this, &ColorListWindow::setRgb);
    connect(ui->slider_blue, &GradientSlider::valueChanged, this, &ColorListWindow::setRgb);
    // Hsv
    connect(ui->slider_hue, &HueSlider::valueChanged, this, &ColorListWindow::setHsv);
    connect(ui->slider_saturation, &GradientSlider::valueChanged, this, &ColorListWindow::setHsv);
    connect(ui->slider_value, &GradientSlider::valueChanged, this, &ColorListWindow::setHsv);
    // Alpha
    connect(ui->slider_alpha, &GradientSlider::valueChanged, this, &ColorListWindow::setAlpha);
    // Hex
    connect(ui->edit_hex, &ColorLineEdit::colorChanged, this, &ColorListWindow::setHex);
    // Wheel
    connect(ui->wheel, &ColorWheel::colorChanged, this, &ColorListWindow::setFromWheel);

    // Map model data to color widgets
    mMapper->setModel(mModel);
    mMapper->addMapping(ui->name, ColorsModel::ColumnName);
    mMapper->addMapping(this, ColorsModel::ColumnColor, "color");
    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mMapper,
            &QDataWidgetMapper::setCurrentModelIndex);
    mMapper->toFirst();
    // submit immediately
    connect(this, &ColorListWindow::colorChanged, mMapper, &QDataWidgetMapper::submit);
}

ColorListWindow::~ColorListWindow()
{
    delete ui;
}

void ColorListWindow::updateColorWidgets(const QColor& col)
{
    FlagSetter fs(&m_updating);

    const std::array<QWidget*, 16> widgets = {
        ui->wheel,      ui->edit_hex,                            //
        ui->slider_red, ui->slider_green,      ui->slider_blue,  // RGB
        ui->spin_red,   ui->spin_green,        ui->spin_blue,    // RGB
        ui->slider_hue, ui->slider_saturation, ui->slider_value, // HSV
        ui->spin_hue,   ui->spin_saturation,   ui->spin_value,   // HSV
        ui->spin_alpha, ui->slider_alpha                         //
    };
    for (auto& w : widgets) {
        w->blockSignals(true);
    }

    // Wheel

    ui->wheel->setColor(col);

    // RGB sliders

    ui->slider_red->setValue(col.red());
    ui->spin_red->setValue(ui->slider_red->value());
    ui->slider_red->setFirstColor(QColor(0, col.green(), col.blue()));
    ui->slider_red->setLastColor(QColor(255, col.green(), col.blue()));

    ui->slider_green->setValue(col.green());
    ui->spin_green->setValue(ui->slider_green->value());
    ui->slider_green->setFirstColor(QColor(col.red(), 0, col.blue()));
    ui->slider_green->setLastColor(QColor(col.red(), 255, col.blue()));

    ui->slider_blue->setValue(col.blue());
    ui->spin_blue->setValue(ui->slider_blue->value());
    ui->slider_blue->setFirstColor(QColor(col.red(), col.green(), 0));
    ui->slider_blue->setLastColor(QColor(col.red(), col.green(), 255));

    // HSV sliders

    ui->slider_hue->setValue(qRound(ui->wheel->hue() * 360.0));
    ui->slider_hue->setColorSaturation(ui->wheel->saturation());
    ui->slider_hue->setColorValue(ui->wheel->value());
    ui->spin_hue->setValue(ui->slider_hue->value());

    ui->slider_saturation->setValue(qRound(ui->wheel->saturation() * 255.0));
    ui->spin_saturation->setValue(ui->slider_saturation->value());
    ui->slider_saturation->setFirstColor(QColor::fromHsvF(ui->wheel->hue(), 0, ui->wheel->value()));
    ui->slider_saturation->setLastColor(QColor::fromHsvF(ui->wheel->hue(), 1, ui->wheel->value()));

    ui->slider_value->setValue(qRound(ui->wheel->value() * 255.0));
    ui->spin_value->setValue(ui->slider_value->value());
    ui->slider_value->setFirstColor(QColor::fromHsvF(ui->wheel->hue(), ui->wheel->saturation(), 0));
    ui->slider_value->setLastColor(QColor::fromHsvF(ui->wheel->hue(), ui->wheel->saturation(), 1));

    // Alpha slider

    ui->slider_alpha->setValue(col.alpha());
    ui->spin_alpha->setValue(ui->slider_alpha->value());
    QColor apha_color = col;
    apha_color.setAlpha(0);
    ui->slider_alpha->setFirstColor(apha_color);
    apha_color.setAlpha(255);
    ui->slider_alpha->setLastColor(apha_color);

    // Hex editor

    ui->edit_hex->setColor(col);

    // Push changes to model
    qDebug() << "changed" << color();
    emit colorChanged();

    for (auto& w : widgets) {
        w->blockSignals(false);
    }
}

QColor ColorListWindow::color()
{
    QColor col = ui->wheel->color();
    col.setAlpha(ui->slider_alpha->value());
    return col;
}

void ColorListWindow::setColor(const QColor& col)
{
    if (m_updating)
        return;
    updateColorWidgets(col);
}

void ColorListWindow::setFromWheel()
{
    QColor col = ui->wheel->color();
    col.setAlpha(ui->slider_alpha->value());
    updateColorWidgets(col);
}

void ColorListWindow::setRgb()
{
    QColor col(ui->slider_red->value(),
               ui->slider_green->value(),
               ui->slider_blue->value(),
               ui->slider_alpha->value());
    updateColorWidgets(col);
}
void ColorListWindow::setHsv()
{
    auto col = QColor::fromHsv(ui->slider_hue->value(),
                               ui->slider_saturation->value(),
                               ui->slider_value->value(),
                               ui->slider_alpha->value());
    updateColorWidgets(col);
}

void ColorListWindow::setHex(const QColor& color)
{
    updateColorWidgets(color);
}

void ColorListWindow::setAlpha()
{
    updateColorWidgets(color());
}

void ColorListWindow::remove()
{
    QList<int> rows;
    for (auto const& index : ui->tableView->selectionModel()->selectedRows()) {
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
        ok = mModel->append(c);
        i++;
    } while (!ok);
}

void ColorListWindow::add()
{
    const QString name = ui->name->text();
    if (confirmAdd(name)) {
        Color c = Color(ui->name->text(), currentColor());
        mModel->append(c);
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

void ColorListWindow::moveUp()
{
    auto index = ui->tableView->selectionModel()->currentIndex();
    // parent is same, thus we move rows before destination index
    mModel->moveRow(index.parent(), index.row(), index.parent(), index.row() - 1);
}

void ColorListWindow::moveDown()
{
    auto index = ui->tableView->selectionModel()->currentIndex();
    // parent is same, thus we move rows before destination index
    mModel->moveRow(index.parent(), index.row(), index.parent(), index.row() + 2);
}

void ColorListWindow::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    mCurrentIndex = current;
    Q_UNUSED(previous)
}

QRgb ColorListWindow::currentColor() const
{
    return ui->wheel->color().rgba();
}
bool ColorListWindow::confirmAdd(const QString& name)
{
    bool has = mModel->contains(name);
    if (!has) {
        return true;
    }
    int ret = QMessageBox::warning(this,
                                   tr("Warning!"),
                                   tr("Color with name '%1' already exists. Overwrite?").arg(name),
                                   QMessageBox::Abort | QMessageBox::Yes);
    return ret == QMessageBox::Yes;
}
