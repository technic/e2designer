#pragma once

#include "skin/positionattr.hpp"
#include "skin/fontattr.hpp"
#include "skin/colorattr.hpp"
#include "borderset.hpp"
#include "repository/xmlnode.hpp"
#include "model/namedlist.hpp"
#include <QMetaEnum>

class WindowStyleTitle
{
public:
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    PositionAttr position;
    FontAttr font;
};

class WindowStyleColor
{
    Q_GADGET
public:
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    enum class ColorRole
    {
        Background,
        LabelForeground,
        ListboxBackground,
        ListboxForeground,
        ListboxSelectedBackground,
        ListboxSelectedForeground,
        ListboxMarkedBackground,
        ListboxMarkedForeground,
        ListboxMarkedAndSelectedBackground,
        ListboxMarkedAndSelectedForeground,
        WindowTitleForeground,
        WindowTitleBackground,
    };
    Q_ENUM(ColorRole)

    ColorRole role;
    CachedColor color;
};

class WindowStyle : public XmlData
{
    Q_GADGET
    friend class ColorRolesModel;

public:
    WindowStyle();

    QString name() const { return QString::number(m_id); }
    bool isNull() const { return m_id == -1; }

    using Value = WindowStyle;

    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;
    static int roleCount();

    ColorAttr getColor(WindowStyleColor::ColorRole role);
    void setColor(WindowStyleColor::ColorRole role, const ColorAttr& color);

private:
    QString m_type;
    int m_id;
    WindowStyleTitle m_title;
    QVector<WindowStyleColor> m_colors;
    BorderSet m_borderSet;
};

class WindowStylesList : public QObject, public NamedList<WindowStyle>
{
    Q_OBJECT
public:
    void append(WindowStyle style);
    // Xml:
    void toXml(QXmlStreamWriter& xml) const;
    inline const WindowStyle getStyle(int id) { return getValue(QString::number(id)); }
    void clear() { removeItems(0, itemsCount()); }
signals:
    void styleChanged(const QString& name, const WindowStyle& value) const;

protected:
    void emitValueChanged(const QString& name, const WindowStyle& value) const final;
};

class ColorsModel;

class ColorRolesModel : public QObject
{
    Q_OBJECT
    using ColorRole = WindowStyleColor::ColorRole;

public:
    ColorRolesModel(ColorsModel& colors, QObject* parent = nullptr);
    void setStyle(WindowStyle* style);
    QColor getQColor(ColorRole role) const;
signals:
    void colorChanged(WindowStyleColor::ColorRole role, QRgb value);
private slots:
    void onColorValueChanged(const QString& name, QRgb value);

private:
    void reload();
    // ref
    WindowStyle* m_style;
    ColorsModel& m_colors;
};
