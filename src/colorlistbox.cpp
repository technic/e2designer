#include "colorlistbox.hpp"
#include "repository/skinrepository.hpp"

ColorListBox::ColorListBox(QWidget* parent)
    : QComboBox(parent)
{
    populateList();
    setEditable(true);
}

QString ColorListBox::color() const
{
    return currentText();
}

void ColorListBox::setColor(const QString& color)
{
    setCurrentIndex(findText(color));
    setEditText(color);
}

void ColorListBox::populateList()
{
    QPixmap px(iconSize());
    auto* colors = SkinRepository::colors();
    for (int i = 0; i < colors->rowCount(); ++i) {
        const Color& color = colors->itemAt(i);
        px.fill(color.value());
        addItem(QIcon(px), color.name());
    }
}
