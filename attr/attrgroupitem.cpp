#include "attrgroupitem.hpp"
#include <QApplication>
#include <QPalette>

QVariant AttrGroupItem::keyData(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return mTitle;
    case Qt::BackgroundRole:
        return QBrush(QApplication::palette().color(QPalette::Button));
    default:
        return AttrItem::keyData(role);
    }
}

QVariant AttrGroupItem::data(int role) const
{
    switch (role) {
    case Qt::BackgroundRole:
        return QBrush(QApplication::palette().color(QPalette::Button));
    default:
        return AttrItem::data(role);
    }
}
