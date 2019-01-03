#ifndef FONTSMODEL_H
#define FONTSMODEL_H

#include "namedlist.hpp"
#include "repository/xmlnode.hpp"
#include <QAbstractTableModel>

class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * @brief Stores font (name, filename) pair defined in skin
 */
class Font
{
public:
    Font(const QString& name = QString(), const QString& fileName = QString());

    // Base
    QString name() const { return mName; }
    QString value() const { return mFileName; }
    bool isNull() const { return mName.isNull(); }
    QFont font() const;

    // Xml
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    typedef QString Value;

private:
    void load();

    QString mName;
    QString mFileName;
    int mFontId;
    QString mStyle;
};

/**
 * @brief Stores fonts definition section in skin
 */
class FontsList : public NamedList<Font>, public XmlData
{
protected:
    void fromXml(QXmlStreamReader& xml);

public:
    void toXml(QXmlStreamWriter& xml) const;
};

/**
 * @brief Provides interface to fonts defined in skin
 */
class FontsModel : public QAbstractTableModel, public FontsList
{
    Q_OBJECT

public:
    explicit FontsModel(QObject* parent = nullptr);

    enum { ColumnName, ColumnFile, ColumnFont, ColumnsCount };

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool append(const Font& f);

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    // Xml
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

signals:
    void valueChanged(const QString& name, const Font& value) const;

protected:
    void emitValueChanged(const QString& name, const Font& value) const final;

private:
};

#endif // FONTSMODEL_H
