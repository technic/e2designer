#include "screenview.hpp"
#include "backgroundpixmap.hpp"
#include "foregroundwidget.hpp"
#include "repository/skinrepository.hpp"
#include "base/flagsetter.hpp"
#include <QCoreApplication>
#include <QGraphicsPixmapItem>

ScreenView::ScreenView(ScreensModel* model)
    : m_model(model)
    , m_selectionModel(nullptr)
    , m_disableSelectionSlots(false)
    , m_scene(new QGraphicsScene(this))
    , m_background(new BackgroundPixmap(QPixmap(":/background.jpg")))
    , m_backgroundRect(new BackgroundRect(QRectF()))
    , m_showBorders(true)
{
    // Add background pixmap on top, it has composition DestinationOver
    m_background->setZValue(1000);
    m_scene->addItem(m_background);
    // Add OSD background below, it has compostion Source
    m_backgroundRect->setBrush(QBrush(QColor(Qt::transparent)));
    m_backgroundRect->setZValue(-1000);
    m_scene->addItem(m_backgroundRect);

    // set inital scene size and subscribe to changes
    setSceneSize(SkinRepository::outputs()->getOutput(m_outputId).size());
    connect(SkinRepository::outputs(),
            &OutputsModel::valueChanged,
            this,
            &ScreenView::onOutputChanged);

    connect(m_model, &ScreensModel::widgetChanged, this, &ScreenView::onWidgetChanged);
    connect(m_model,
            &ScreensModel::rowsAboutToBeRemoved,
            this,
            &ScreenView::onRowsAboutToBeRemoved);
    connect(m_model, &ScreensModel::rowsAboutToBeMoved, this, &ScreenView::onRowsAboutToBeMoved);
    connect(m_model, &ScreensModel::rowsMoved, this, &ScreenView::onRowsMoved);
    connect(m_model, &ScreensModel::rowsInserted, this, &ScreenView::onRowsInserted);
    connect(m_model, &ScreensModel::modelReset, this, &ScreenView::onModelReset);

    connect(m_scene, &QGraphicsScene::selectionChanged, this, &ScreenView::onSceneSelectionChanged);
}

void ScreenView::onOutputChanged(int id, const VideoOutput& output)
{
    if (id == m_outputId) {
        setSceneSize(output.size());
    }
}

void ScreenView::setSceneSize(const QSize& size)
{
    m_scene->setSceneRect(0, 0, size.width(), size.height());
    m_backgroundRect->setRect(0, 0, size.width(), size.height());
    // Adjust background scale
    QSizeF pixmapSize = m_background->boundingRect().size();
    qreal sw = size.width() / pixmapSize.width();
    qreal sh = size.height() / pixmapSize.height();
    m_background->setTransform(QTransform::fromScale(sw, sh));
}

QModelIndex ScreenView::normalizeIndex(const QModelIndex& index) const
{
    return index.sibling(index.row(), ScreensModel::ColumnElement);
}

QItemSelection ScreenView::makeRowSelection(const QModelIndex& index)
{
    return QItemSelection(index.sibling(index.row(), 0),
                          index.sibling(index.row(), m_model->columnCount(index.parent()) - 1));
}

void ScreenView::setScreen(QModelIndex index)
{
    Q_ASSERT(index.data(ScreensModel::TypeRole).toInt()
             == static_cast<int>(WidgetData::WidgetType::Screen));

    if (m_root == index)
        return;

    qDebug() << "to delete:" << m_widgets.size();

    WidgetGraphicsItem* oldScreen = m_widgets[m_root];
    if (oldScreen) {
        // All items must be childs of the oldScreen
        m_scene->removeItem(oldScreen);
        delete oldScreen;
        m_scene->clearSelection();
    }
    m_widgets.clear();

    m_root = normalizeIndex(index);
    auto* screen = new WidgetGraphicsItem(this, m_root, nullptr);
    m_widgets[m_root] = screen;
    m_scene->addItem(screen);

    for (int i = 0; i < m_model->rowCount(m_root); ++i) {
        QModelIndex widgetIndex = m_model->index(i, ScreensModel::ColumnElement, m_root);
        Q_ASSERT(!m_widgets.contains(widgetIndex));
        m_widgets[widgetIndex] = new WidgetGraphicsItem(this, widgetIndex, screen);
    }
}

void ScreenView::setSelectionModel(QItemSelectionModel* model)
{
    if (Q_UNLIKELY(model->model() != m_model)) {
        qWarning() << "failed to set selectionModel"
                   << "because it works on a different model";
        return;
    }
    // disconnect from old model
    if (m_selectionModel) {
        disconnect(m_selectionModel,
                   &QItemSelectionModel::currentChanged,
                   this,
                   &ScreenView::setCurrentWidget);
        disconnect(m_selectionModel,
                   &QItemSelectionModel::selectionChanged,
                   this,
                   &ScreenView::updateSelection);
    }
    m_selectionModel = model;
    // connect to new model
    if (m_selectionModel) {
        connect(m_selectionModel,
                &QItemSelectionModel::currentChanged,
                this,
                &ScreenView::setCurrentWidget);
        connect(m_selectionModel,
                &QItemSelectionModel::selectionChanged,
                this,
                &ScreenView::updateSelection);
    }
}

void ScreenView::deleteSelected()
{
    // I only delete last clicked one
    if (m_scene->selectedItems().isEmpty())
        return;
    QGraphicsItem* item = m_scene->selectedItems().last();
    auto w = qgraphicsitem_cast<WidgetGraphicsItem*>(item);
    if (w) {
        QModelIndex i = w->modelIndex();
        m_model->removeRow(i.row(), i.parent());
    } else {
        qWarning() << "garbage selected";
    }
}

void ScreenView::displayBorders(bool display)
{
    m_showBorders = display;
    for (const auto& widget : m_widgets) {
        widget->showBorder(display);
    }
}

void ScreenView::onWidgetChanged(const QModelIndex& index, int key)
{
    auto it = m_widgets.find(index);
    if (it != m_widgets.end()) {
        (*it)->updateAttribute(key);
    }
}

/**
 * @brief Check if @p index is equal to or is child of @a m_root
 * @param index
 * @return
 */
bool ScreenView::belongsToRoot(QModelIndex index) const
{
    while (index.isValid() && !(index == m_root)) {
        index = index.parent();
    }
    return index.isValid();
}

/**
 * @brief Check if @p index is a parent of @a m_root
 * @param index
 * @return
 */
bool ScreenView::containsOurRoot(const QModelIndex& index, int first, int last) const
{
    QModelIndex root = m_root;
    while (root.isValid()) {
        QModelIndex parent = root.parent();
        if (parent == index) {
            break;
        } else {
            root = parent;
        }
    }
    int r = root.row();
    return root.isValid() && (first <= r && r <= last);
}

void ScreenView::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    if (containsOurRoot(parent, first, last)) {
        // our root is one of the deleted elements
        WidgetGraphicsItem* screen = m_widgets[m_root];
        if (screen) {
            m_scene->removeItem(screen);
            delete screen;
            m_widgets.clear();
        }
    } else {
        // part of our screen's children will be removed
        removeChildren(parent, first, last);
    }
}

void ScreenView::onRowsAboutToBeMoved(const QModelIndex& sourceParent,
                                      int sourceStart,
                                      int sourceEnd,
                                      const QModelIndex& destinationParent,
                                      int destinationRow)
{
    if (belongsToRoot(sourceParent) && !belongsToRoot(destinationParent)) {
        // some children will disappear in our screen subtree
        removeChildren(sourceParent, sourceStart, sourceEnd);
    }
    Q_UNUSED(destinationRow)
}

void ScreenView::onRowsMoved(const QModelIndex& parent,
                             int start,
                             int end,
                             const QModelIndex& destination,
                             int row)
{
    if (!belongsToRoot(parent) && belongsToRoot(destination)) {
        // children has appeared in our screen subtree
        onRowsInserted(destination, row, row + end - start);
    }
}

void ScreenView::removeChildren(const QModelIndex& parent, int first, int last)
{
    if (belongsToRoot(parent)) {
        // rely on our map
        for (int i = first; i <= last; ++i) {
            QModelIndex index = m_model->index(i, 0, parent);
            Q_ASSERT(m_widgets.contains(index));

            WidgetGraphicsItem* w = m_widgets.take(index);
            // releases ownership
            m_scene->removeItem(w);
            delete w;
        }
    }
}

void ScreenView::onRowsInserted(const QModelIndex& parent, int first, int last)
{
    if (!belongsToRoot(parent))
        return;

    // Ok it belongs to our root
    WidgetGraphicsItem* screen = m_widgets[m_root];

    for (int i = first; i <= last; ++i) {
        QModelIndex windex = m_model->index(i, 0, parent);
        auto* view = new WidgetGraphicsItem(this, windex, screen);
        Q_ASSERT(!m_widgets.contains(windex));
        m_widgets[windex] = view;
    }
}

void ScreenView::onModelAboutToBeReset()
{
    // FIXME
}

void ScreenView::onModelReset()
{
    // FIXME
}

/**
 * @brief Push scene selection changes to the selectionModel
 */
void ScreenView::onSceneSelectionChanged()
{
    if (m_disableSelectionSlots)
        return;
    if (!m_selectionModel)
        return;

    FlagSetter fs(&m_disableSelectionSlots);

    for (auto index : m_selectionModel->selectedIndexes()) {
        auto it = m_widgets.find(index);
        if (it != m_widgets.end() && !(*it)->isSelected()) {
            m_selectionModel->select(makeRowSelection(index), QItemSelectionModel::Deselect);
        }
    }
    for (auto* item : m_scene->selectedItems()) {
        auto w = qgraphicsitem_cast<WidgetGraphicsItem*>(item);
        if (w) {
            m_selectionModel->select(makeRowSelection(w->modelIndex()),
                                     QItemSelectionModel::Select);
        }
    }
    if (!m_scene->selectedItems().empty()) {
        auto* item = m_scene->selectedItems().back();
        auto w = qgraphicsitem_cast<WidgetGraphicsItem*>(item);
        if (w) {
            m_selectionModel->setCurrentIndex(w->modelIndex(), QItemSelectionModel::NoUpdate);
        }
    }
}

void ScreenView::setCurrentWidget(const QModelIndex& current, const QModelIndex& previous)
{
    if (m_disableSelectionSlots)
        return;

    FlagSetter fs(&m_disableSelectionSlots);

    // previous widget should be in the scene
    if (previous.isValid()) {
        qDebug() << "previous in the scene:" << m_widgets.contains(normalizeIndex(previous));
    }
    // find parent Screen
    QModelIndex index = current;
    while (index.isValid()
           && index.data(ScreensModel::TypeRole).toInt()
                != static_cast<int>(WidgetData::WidgetType::Screen)) {
        index = index.parent();
    }
    if (index.isValid()) {
        setScreen(index);
    } else {
        qWarning() << "failed to set current widget: screen not found!";
        return;
    }
    // current widget should be in the scene
    Q_ASSERT(m_widgets.contains(normalizeIndex(current)));
    m_widgets[normalizeIndex(current)]->setSelected(true);
}

void ScreenView::updateSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (m_disableSelectionSlots)
        return;

    FlagSetter fs(&m_disableSelectionSlots);

    for (QModelIndex index : deselected.indexes()) {
        auto it = m_widgets.find(index);
        if (it != m_widgets.end()) {
            (*it)->setSelected(false);
        }
    }
    for (QModelIndex index : selected.indexes()) {
        auto it = m_widgets.find(index);
        if (it != m_widgets.end()) {
            (*it)->setSelected(true);
        }
    }
}
