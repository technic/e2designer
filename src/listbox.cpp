#include "listbox.hpp"

SkinEnumList ListBox::list() const
{
    QStringList list;
    for (int i = 0; i < count(); ++i) {
        list.append(itemData(i, Qt::EditRole).toString());
    }
    return SkinEnumList(currentText(), list);
}

void ListBox::setList(SkinEnumList value)
{
    addItems(value.second);
    setCurrentIndex(findText(value.first));
}
