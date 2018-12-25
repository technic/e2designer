#include "colorlistbox.hpp"
#include "repository/skinrepository.hpp"

ColorListBox::ColorListBox(QWidget* parent)
    : QComboBox(parent)
{
    populateList();
}

QColor ColorListBox::color() const
{
    return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorListBox::setColor(const QColor& color)
{
    setCurrentIndex(findData(color, Qt::DecorationRole));
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
