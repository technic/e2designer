#ifndef SCREENSMODEL_H
#define SCREENSMODEL_H

#include "skin/widgetdata.hpp"
#include "model/windowstyle.hpp"
#include "commands/attrcommand.hpp"
#include <QAbstractItemModel>
#include <QUndoStack>

struct Preview
{
    Preview(QVariant value = QVariant(), Property::Render render = Property::Label)
        : value(value)
        , render(render)
    {}
    QVariant value;
    Property::Render render;
};

class ScreensTree
{
public:
    void loadPreviews(const QString& path);
    /**
     * @brief Save previews stored in QMap
     * @param path Xml file name
     */
    void savePreviews(const QString& path);
    Preview getPreview(const QString& screen, const QString& widget) const;

protected:
    QMap<QString, QMap<QString, Preview>> m_previews;
};

class ColorsModel;
class ColorRolesModel;
class FontsModel;

class ScreensModel : public QAbstractItemModel, public ScreensTree
{
    Q_OBJECT

public:
    explicit ScreensModel(ColorsModel& colors,
                          ColorRolesModel& roles,
                          FontsModel& fonts,
                          QObject* parent = Q_NULLPTR);
    ~ScreensModel() override;

    using Item = WidgetData;

    enum
    {
        ColumnElement,
        ColumnName,
        ColumnsCount
    };
    enum
    {
        TypeRole = Qt::UserRole + 1
    };

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row,
                      int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    void clear();

    bool moveRows(const QModelIndex& sourceParent,
                  int sourceRow,
                  int count,
                  const QModelIndex& destinationParent,
                  int destinationChild) override;

    // Drag and drop:

    /// Returns ownership according to Qt documentation
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    bool dropMimeData(const QMimeData* data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex& parent) override;
    Qt::DropActions supportedDropActions() const override;

    // Xml:
    void appendFromXml(QXmlStreamReader& xml);
    void toXml(XmlStreamWriter& xml);

    // Read only access to widget:
    const WidgetData& widget(const QModelIndex& index) const;

    // Access widget attributes:
    QVariant widgetAttr(const QModelIndex& index, int key) const;
    bool setWidgetAttr(const QModelIndex& index, int key, const QVariant& value);
    // Syntax sugar
    //    template<class T>
    //    bool setWidgetAttr(const QModelIndex &index, int key, const T &value) {
    //        return setWidgetAttr(index, key, QVariant::fromValue(value));
    //    }

    // Edit widget with XML editor
    bool setWidgetDataFromXml(const QModelIndex& index, QXmlStreamReader& xml);

    // Access associated fonts and colors
    const ColorsModel& colors() const { return m_colorsModel; }
    const FontsModel& fonts() const { return m_fontsModel; }
    const ColorRolesModel& roles() const { return m_colorRolesModel; }

    // Access undo model
    QUndoStack* undoStack() const { return m_commander; }

    // Move and resize widget
    void resizeWidget(const QModelIndex& index, const QSize& size);
    void moveWidget(const QModelIndex& index, const QPoint& pos);
    void changeWidgetRect(const QModelIndex& index, const QRect& rect);

    // widgetChanged only emitted for widgets being observed
    void registerObserver(const QModelIndex& index);
    void unregisterObserver(const QModelIndex& index);

    // Build preview map from widget tree
    void updatePreviewMap();
    void savePreviewTree(const QString& path);

signals:
    void widgetChanged(const QModelIndex& index, int attr);

public slots:
    // to be called from WidgetData
    void widgetAttrHasChanged(const WidgetData* widget, int attrKey);
    //
    void onColorChanged(const QString& name, QRgb value);
    void onStyledColorChanged(WindowStyleColor::ColorRole role, QRgb value);
    void onFontChanged(const QString& name, const Font& value);

protected:
    QVector<WidgetData*> takeChildren(int row, int count, WidgetData& parent);
    void insertChildren(int row, const QVector<WidgetData*>& childs, WidgetData& parent);
    friend class RemoveRowsCommand;
    friend class InsertRowsCommand;

private:
    Item* indexToItem(const QModelIndex& index) const;
    static Item* castItem(const QModelIndex& index);

    bool isValidMove(const QModelIndex& sourceParent,
                     int sourceRow,
                     int count,
                     const QModelIndex& destinationParent,
                     int destinationChild) const;
    void encodeRows(const QModelIndexList& indexes, QDataStream& stream) const;
    QVector<QModelIndex> decodeRows(QDataStream& stream) const;

    QHash<QPersistentModelIndex, int> m_observers;

    // QTimer* m_timer;
    // QTime m_lastShot;

    // ref
    ColorsModel& m_colorsModel;
    ColorRolesModel& m_colorRolesModel;
    FontsModel& m_fontsModel;

    // own
    WidgetData* m_root;
    // QObject owned
    QUndoStack* m_commander;
};

class WidgetObserverRegistrator
{
    Q_DISABLE_COPY(WidgetObserverRegistrator)
public:
    WidgetObserverRegistrator(ScreensModel* model, const QModelIndex& index);
    void setIndex(const QModelIndex& index);
    ~WidgetObserverRegistrator();

private:
    ScreensModel* m_model;
    QPersistentModelIndex m_index;
};

class RemoveRowsCommand : public QUndoCommand
{
public:
    RemoveRowsCommand(WidgetData& root, int row, int count, QUndoCommand* parent = nullptr);
    int id() const final { return getCommandId<decltype(this)>(); }
    void redo() final;
    void undo() final;
    ~RemoveRowsCommand();

private:
    // Reference to the parent element
    WidgetData& m_root;
    int m_row;
    int m_count;
    // Takes ownership of items removed from the model
    QVector<WidgetData*> m_items;
};

class InsertRowsCommand : public QUndoCommand
{
public:
    InsertRowsCommand(WidgetData& root,
                      int row,
                      QVector<WidgetData*> items,
                      QUndoCommand* parent = nullptr);
    int id() const final { return getCommandId<decltype(this)>(); }
    void redo() final;
    void undo() final;
    ~InsertRowsCommand();

private:
    // Reference to the parent element
    WidgetData& m_root;
    int m_row;
    int m_count;
    // Holds ownership of items until they are passed to the model
    QVector<WidgetData*> m_items;
};

#endif // SCREENSMODEL_H
