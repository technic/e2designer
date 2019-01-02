#include "colorlistbox.hpp"
#include "repository/skinrepository.hpp"

ColorListBox::ColorListBox(QWidget* parent)
    : QComboBox(parent)
{
    populateList();
}

QString ColorListBox::color() const
{
    return currentText();
}

void ColorListBox::setColor(const QString& color)
{
    setCurrentIndex(findText(color));
}

void ColorListBox::populateList()
{
    auto* colors = SkinRepository::colors();
    for (int i = 0; i < colors->rowCount(); ++i) {
        const Color& color = colors->itemAt(i);
        insertItem(i, color.name());
        setItemData(i, QColor::fromRgba(color.value()), Qt::DecorationRole);
    }
}
