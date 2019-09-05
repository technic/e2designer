#pragma once

#include "namedlist.hpp"
#include "movablelistmodel.hpp"
#include "repository/xmlnode.hpp"
#include <QAbstractTableModel>
#include <QRgb>

class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * @brief Stores color (name, value) pair defined in skin
 */
class Color
{
public:
    Color(const QString& name = QString(), QRgb value = 0);

    QString name() const { return m_name; }
    QRgb value() const { return m_value; }
    bool isNull() const { return m_name.isNull(); }
    QString valueStr() const;

    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    using Value = QRgb;

private:
    QString m_name;
    QRgb m_value;
};

/**
 * @brief Stores colors definition section in skin
 */
class ColorsList : public NamedList<Color>, public XmlData
{
protected:
    void fromXml(QXmlStreamReader& xml);

public:
    void toXml(QXmlStreamWriter& xml) const;
};

/**
 * @brief Provides interface to color pallet used in skin
 */
class ColorsModel : public MovableListModel, public ColorsList
{
    Q_OBJECT

public:
    explicit ColorsModel(QObject* parent = nullptr);

    enum
    {
        ColumnName,
        ColumnValue,
        ColumnColor,
        ColumnsCount,
    };

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insert(int row, const Color& item);
    bool append(const Color& c) { return insert(itemsCount(), c); }

    bool insertRows(int row, int count, const QModelIndex& parent) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    bool moveRows(const QModelIndex& sourceParent,
                  int sourceRow,
                  int count,
                  const QModelIndex& destinationParent,
                  int destinationChild) override;

    // Xml:
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    // Get value by name:
    //    inline bool contains(QString name) { return
    //    ColorsList::contains(name);
    //    }
    //    inline Color getValue(QString name) { return ColorsList::value(name);
    //    }

signals:
    void valueChanged(const QString& name, QRgb value) const;

protected:
    void emitValueChanged(const QString& name, const Color& value) const final;
};
