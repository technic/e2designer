#ifndef SCREENSMODEL_H
#define SCREENSMODEL_H

#include "repository/widgetdata.h"
#include <QAbstractItemModel>


class WidgetObserver;


struct Preview {
    Preview(QVariant value = QVariant(), int render = Property::Label) :
        value(value), render(render) { }
    QVariant value;
    int render;
};


class ScreensTree
{
public:
    void loadPreviews();
    Preview getPreview(const QString &screen, const QString &widget);

private:
    QMap<QString, QMap<QString, Preview>> mPreviews;
};


class ScreensModel : public QAbstractItemModel, public ScreensTree
{
    Q_OBJECT

public:
    explicit ScreensModel(QObject *parent = Q_NULLPTR);
    ~ScreensModel() override;

    //	typedef MixinTreeNode<WidgetData> Item;
    typedef WidgetData Item;

    enum { ColumnElement, ColumnName, ColumnsCount };
    enum { IdRole = Qt::UserRole + 1, TypeRole };

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void clear();

    // Xml:
    void appendFromXml(QXmlStreamReader &xml);
    void toXml(QXmlStreamWriter &xml);

    // Access widget attributes:
    QVariant getWidgetAttr(const QModelIndex &index, int attrKey, int role = Qt::DisplayRole) const;
    bool setWidgetAttr(const QModelIndex &index, int attrKey, const QVariant &value, int role);

    // Access widget by raw pointer, use carefully
    WidgetData *getWidget(const QModelIndex &index);

public slots:
    // to be called from WidgetData
    void widgetAttrHasChanged(const WidgetData *widget, int attrKey);

private:
    static Item *castItem(const QModelIndex &index);

    WidgetData *mRoot;
    QMap<QString, QMap<QString, Preview>> mPreviews;

    QMap<int, WidgetObserver*> mObservers;
};


class WidgetObserver: public QObject
{
    Q_OBJECT

public:

};

#endif // SCREENSMODEL_H
