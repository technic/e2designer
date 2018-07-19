#include "positionitem.hpp"
#include "repository/widgetdata.hpp"
#include <QDebug>


// PositionAttr

QPoint PositionAttr::toPoint(const WidgetData &widget)
{
    QSize s = widget.selfSize();
    QSize p = widget.parentSize();
    return QPoint(mX.getInt(s.width(), p.width()),
                  mY.getInt(s.height(), p.height()));

}

void PositionAttr::setPoint(const WidgetData &widget, const QPoint &pos)
{
    // Try to be smart
    const QSize s = widget.selfSize();
    const QSize p = widget.parentSize();
    mX.parseInt(pos.x(), s.width(), p.width());
    mY.parseInt(pos.y(), s.height(), p.height());
}

QString PositionAttr::toStr() const
{
    return mX.toStr() + "," + mY.toStr();
}

void PositionAttr::fromStr(const QString& str)
{
    QStringList list = str.split(",");
    if (list.length() == 2) {
        mX.parseStr(list[0]);
        mY.parseStr(list[1]);
    } else {
        qWarning() << "bad position:" << str;
    }
}


// AbsolutePositionItem

QVariant AbsolutePositionItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    {
        QPoint pos = pWidget->getAttr<PositionAttr>(key()).toPoint(*pWidget);
        return QString("[X=%1 Y=%2]").arg(pos.x(), pos.y());
    }
    default:
        return AttrItem::data(role);
    }
}


// PositionItem

PositionItem::PositionItem(WidgetData* widget, int key)
    : AttrItem(widget, key)
{
    // TODO: test it
//    appendChild(new AbsolutePositionItem(pWidget));
//    appendChild(new CoordinateItem(pWidget, Coordinate::X));
//    appendChild(new CoordinateItem(pWidget, Coordinate::Y));
}


QVariant PositionItem::data(int role) const
{
    switch (role) {
    case Roles::GraphicsRole:
        return attr().toPoint(*pWidget);
    case Roles::AnchorRole:
        return QVariant::fromValue(attr().anchor());
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return attr().toStr();
    default:
        return AttrItem::data(role);
    }
}

bool PositionItem::setData(const QVariant &value, int role)
{
    switch (role) {
    case Roles::GraphicsRole:
    {
        auto pos = attr();
        pos.setPoint(*pWidget, value.toPoint());
        setAttr(pos);
        return true;
    }
    case Roles::XmlRole:
    case Qt::EditRole:
        setAttr(PositionAttr(value.toString()));
        return true;
    default:
        return AttrItem::setData(value, role);
    }
}

PositionAttr PositionItem::attr() const
{
    return pWidget->getAttr<PositionAttr>(key());
}

void PositionItem::setAttr(const PositionAttr &value)
{
    pWidget->setAttr(key(), value);
}


// PositionItem::CoordinateItem

QVariant PositionItem::CoordinateItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        // TODO: wrong logic - should edit relative values
        auto pos = pWidget->getAttr<PositionAttr>(Property::position).toPoint(*pWidget);
        switch (axes) {
        case AxesName::X:
            return pos.x();
        case AxesName::Y:
            return pos.y();
        }
        break;
    }
    default:
        return AttrItem::data(role);
    }
    return QVariant();
}

bool PositionItem::CoordinateItem::setData(const QVariant& value, int role)
{
    // FIXME: implement!
    return false;
}

static AttrItemRegistrator<PositionAttr, PositionItem> registrator;
