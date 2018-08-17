#ifndef WIDGETDATA_H
#define WIDGETDATA_H

#include "adapter/attritemfactory.hpp"
#include "attr/attritem.hpp"
#include "base/tree.hpp"
#include "base/uniqueid.hpp"
#include "repository/attrcontainer.hpp"
#include "repository/xmlnode.hpp"
#include <QRgb>
#include <QVariant>

#include "attr/coloritem.hpp"
#include "attr/positionitem.hpp"
#include "attr/sizeitem.hpp"
#include "skin/attributes.hpp"

class QXmlStreamReader;
class QXmlStreamWriter;
class ScreensModel;

class PositionItem;
class SizeItem;
class Font;
class WidgetDataObserver;
class WidgetObserver;

class ConverterItem
{
public:
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

private:
    QString mType;
    QString mText;
};

class WidgetData : public QObject,
                   public MixinTreeNode<WidgetData>,
                   protected AttrContainer,
                   public XmlData,
                   public UniqueId
{
    Q_OBJECT

public:
    using MixinTreeNode<WidgetData>::parent;

    explicit WidgetData(bool empty = false);
    ~WidgetData() override;

    enum WidgetType { Screen, Widget, Label, Pixmap };
    Q_ENUM(WidgetType)

    AttrItem* adaptersRoot() const { return mRoot; }
    AttrItem* getAttrAdapterPtr(const int key) const;

    WidgetType type() const { return mType; }
    bool setType(int type);
    void setType(WidgetType type);
    QString typeStr() const;
    static WidgetType strToType(QStringRef str, bool& ok);

    // Quick access to size
    QSize selfSize() const;
    QSize parentSize() const;

    // Xml
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    // Attr template type methods
    template <typename T>
    inline T getAttr(int key) const
    {
        return AttrContainer::getAttr<T>(key);
    }
    template <typename T>
    bool setAttr(int key, const T& value)
    {
        bool changed = AttrContainer::setAttr(key, value);
        if (changed) {
            notifyAttrChange(key);
            switch (key) {
            case Property::position:
                for (int i = 0; i < childCount(); ++i) {
                    // TODO: test
                    child(i)->parentSizeChanged();
                }
            }
        }
        return changed;
    }
    // Attr QVariant methods
    QVariant getAttr(const int key, int role) const;
    bool setAttr(const int key, const QVariant& value, int role);

    // Required for the optimisation:
    // only widgets which are viewed by someone
    // listen to color/font changed signals
    void connectNotify(const QMetaMethod& signal) override;
    void disconnectNotify(const QMetaMethod& signal) override;

signals:
    void attrChanged(int key);
    void typeChanged(WidgetType type);

private slots:
    void onColorChanged(const QString& name, QRgb value);
    void onFontChanged(const QString& name, const Font& value);

private:
    void buildPropertiesTree();
    void parentSizeChanged();
    AttrItem* getAttrPtr(int key);
    void notifyAttrChange(int key);

    template <class AttrType>
    void addProperty(int key, AttrItem* parent)
    {
        if (key != Property::invalid) {
            addAttr<AttrType>(key);
        }

        auto* adapter = AttrItemFactory::createAdapter(this, qMetaTypeId<AttrType>(), key);
        parent->appendChild(adapter);
        if (key != Property::invalid) {
            mAdapters[key] = adapter;
        }
    }

    //    template<typename T>
    //    inline T getHashMember(QHash<int,T> &hash, int k) {
    //        auto it = hash.find(k);
    //        if (it != hash.end()) {
    //            return it.value();
    //        } else {
    //            return T();
    //        }
    //    }

    //    template<typename T>
    //    inline bool setHashMember(QHash<int,T> &hash, int k, const T &value,
    //    bool add = false) {
    //        auto it = hash.find(k);
    //        if (it != hash.end()) {
    //            *it = value;
    //            emit attrChanged(k);
    //            return true;
    //        } else {
    //            hash[k] = value;
    //            return false;
    //        }
    //    }

    WidgetType mType;
    // own
    QVector<int> mPropertiesOrder;

    // TODO: move out of class
    // own
    AttrItem* mRoot;
    // refs within mRoot
    QHash<int, AttrItem*> mAdapters;

    QVector<ConverterItem> mConverters;

    int mConnectedCount;
};

#endif // WIDGETDATA_H
