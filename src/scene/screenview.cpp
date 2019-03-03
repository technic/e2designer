#include "screenview.hpp"
#include "backgroundpixmap.hpp"
#include "foregroundwidget.hpp"
#include "repository/skinrepository.hpp"
#include "base/flagsetter.hpp"
#include <QCoreApplication>
#include <QGraphicsPixmapItem>

ScreenView::ScreenView(ScreensModel* model)
    : mModel(model)
    , mSelectionModel(nullptr)
    , mDisableSelectionSlots(false)
    , mScene(new QGraphicsScene(this))
    , mBackground(new BackgroundPixmap(QPixmap(":/background.jpg")))
    , mBackgroundRect(new BackgroundRect(QRectF()))
    , m_showBorders(true)
{
    // Add background pixmap on top, it has composition DestinationOver
    mBackground->setZValue(1000);
    mScene->addItem(mBackground);
    // Add OSD background below, it has compostion Source
    mBackgroundRect->setBrush(QBrush(QColor(Qt::transparent)));
    mBackgroundRect->setZValue(-1000);
    mScene->addItem(mBackgroundRect);

    // set inital scene size and subscribe to changes
    setSceneSize(SkinRepository::outputs()->getOutput(mOutputId).size());
    connect(SkinRepository::outputs(),
            &VideoOutputRepository::valueChanged,
            this,
            &ScreenView::onOutputChanged);

    connect(mModel, &ScreensModel::widgetChanged, this, &ScreenView::onWidgetChanged);
    connect(mModel, &ScreensModel::rowsAboutToBeRemoved, this, &ScreenView::onRowsAboutToBeRemoved);
    connect(mModel, &ScreensModel::rowsInserted, this, &ScreenView::onRowsInserted);
    connect(mModel, &ScreensModel::modelReset, this, &ScreenView::onModelReset);

    connect(mScene, &QGraphicsScene::selectionChanged, this, &ScreenView::onSceneSelectionChanged);
}

void ScreenView::onOutputChanged(int id, const VideoOutput& output)
{
    if (id == mOutputId) {
        setSceneSize(output.size());
    }
}

void ScreenView::setSceneSize(const QSize& size)
{
    mScene->setSceneRect(0, 0, size.width(), size.height());
    mBackgroundRect->setRect(0, 0, size.width(), size.height());
    // Adjust background scale
    QSizeF pixmapSize = mBackground->boundingRect().size();
    qreal sw = size.width() / pixmapSize.width();
    qreal sh = size.height() / pixmapSize.height();
    mBackground->setTransform(QTransform::fromScale(sw, sh));
}

QModelIndex ScreenView::normalizeIndex(const QModelIndex& index) const
{
    return index.sibling(index.row(), ScreensModel::ColumnElement);
}

QItemSelection ScreenView::makeRowSelection(const QModelIndex& index)
{
    return QItemSelection(index.sibling(index.row(), 0),
                          index.sibling(index.row(), mModel->columnCount(index.parent()) - 1));
}

void ScreenView::setScreen(QModelIndex index)
{
    Q_ASSERT(index.data(ScreensModel::TypeRole).toInt() == WidgetData::Screen);

    if (mRoot == index)
        return;

    qDebug() << "to delete:" << mWidgets.size();

    WidgetGraphicsItem* oldScreen = mWidgets[mRoot];
    if (oldScreen) {
        // All items must be childs of the oldScreen
        mScene->removeItem(oldScreen);
        delete oldScreen;
        mScene->clearSelection();
    }
    mWidgets.clear();

    mRoot = normalizeIndex(index);
    auto* screen = new WidgetGraphicsItem(this, mRoot, nullptr);
    mWidgets[mRoot] = screen;
    mScene->addItem(screen);

    for (int i = 0; i < mModel->rowCount(mRoot); ++i) {
        QModelIndex widgetIndex = mModel->index(i, ScreensModel::ColumnElement, mRoot);
        auto* view = new WidgetGraphicsItem(this, widgetIndex, screen);
        Q_ASSERT(!mWidgets.contains(widgetIndex));
        mWidgets[widgetIndex] = view;
    }
}

void ScreenView::setSelectionModel(QItemSelectionModel* model)
{
    if (Q_UNLIKELY(model->model() != mModel)) {
        qWarning() << "failed to set selectionModel"
                   << "because it works on a different model";
        return;
    }
    // disconnect from old model
    if (mSelectionModel) {
        disconnect(mSelectionModel,
                   &QItemSelectionModel::currentChanged,
                   this,
                   &ScreenView::setCurrentWidget);
        disconnect(mSelectionModel,
                   &QItemSelectionModel::selectionChanged,
                   this,
                   &ScreenView::updateSelection);
    }
    mSelectionModel = model;
    // connect to new model
    if (mSelectionModel) {
        connect(mSelectionModel,
                &QItemSelectionModel::currentChanged,
                this,
                &ScreenView::setCurrentWidget);
        connect(mSelectionModel,
                &QItemSelectionModel::selectionChanged,
                this,
                &ScreenView::updateSelection);
    }
}

void ScreenView::deleteSelected()
{
    // I only delete last clicked one
    QGraphicsItem* item = mScene->selectedItems().last();
    // FIXME: size check
    auto w = qgraphicsitem_cast<WidgetGraphicsItem*>(item);
    if (w) {
        QModelIndex i = w->modelIndex();
        mModel->removeRow(i.row(), i.parent());
    } else {
        qWarning() << "garbage selected";
    }
}

void ScreenView::displayBorders(bool display)
{
    m_showBorders = display;
    for (const auto& widget : mWidgets) {
        widget->showBorder(display);
    }
}

void ScreenView::onWidgetChanged(const QModelIndex& index, int key)
{
    auto it = mWidgets.find(index);
    if (it != mWidgets.end()) {
        (*it)->updateAttribute(key);
    }
}

void ScreenView::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    // check if delete our root
    const int r = mRoot.row();
    if (mRoot.parent() == parent && first <= r && r <= last) {
        WidgetGraphicsItem* screen = mWidgets[mRoot];
        if (screen) {
            mScene->removeItem(screen);
            delete screen;
            mWidgets.clear();
        }
    } else {
        // just rely on our map
        for (int i = first; i <= last; ++i) {
            QModelIndex index = mModel->index(i, 0, parent);
            Q_ASSERT(mWidgets.contains(index));

            if (mWidgets.contains(index)) {
                WidgetGraphicsItem* w = mWidgets.take(index);
                // releases ownership
                mScene->removeItem(w);
                delete w;
            }
        }
    }
}

void ScreenView::onRowsInserted(const QModelIndex& parent, int first, int last)
{
    QModelIndex widget = parent;
    while (widget.isValid() && !(widget == mRoot)) {
        widget = widget.parent();
    }
    if (!widget.isValid())
        return;

    // Ok it belongs to our root
    WidgetGraphicsItem* screen = mWidgets[mRoot];

    for (int i = first; i <= last; ++i) {
        QModelIndex windex = mModel->index(i, 0, parent);
        auto* view = new WidgetGraphicsItem(this, windex, screen);
        Q_ASSERT(!mWidgets.contains(windex));
        mWidgets[windex] = view;
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
    if (mDisableSelectionSlots)
        return;
    if (!mSelectionModel)
        return;

    FlagSetter fs(&mDisableSelectionSlots);

    for (auto index : mSelectionModel->selectedIndexes()) {
        auto it = mWidgets.find(index);
        if (it != mWidgets.end() && !(*it)->isSelected()) {
            mSelectionModel->select(makeRowSelection(index), QItemSelectionModel::Deselect);
        }
    }
    for (auto* item : mScene->selectedItems()) {
        auto w = qgraphicsitem_cast<WidgetGraphicsItem*>(item);
        if (w) {
            mSelectionModel->select(makeRowSelection(w->modelIndex()), QItemSelectionModel::Select);
        }
    }
    if (!mScene->selectedItems().empty()) {
        auto* item = mScene->selectedItems().back();
        auto w = qgraphicsitem_cast<WidgetGraphicsItem*>(item);
        if (w) {
            mSelectionModel->setCurrentIndex(w->modelIndex(), QItemSelectionModel::NoUpdate);
        }
    }
}

void ScreenView::setCurrentWidget(const QModelIndex& current, const QModelIndex& previous)
{
    if (mDisableSelectionSlots)
        return;

    FlagSetter fs(&mDisableSelectionSlots);

    // previous widget should be in the scene
    if (previous.isValid()) {
        qDebug() << "previous in the scene:" << mWidgets.contains(normalizeIndex(previous));
    }
    // find parent Screen
    QModelIndex index = current;
    while (index.isValid() && index.data(ScreensModel::TypeRole).toInt() != WidgetData::Screen) {
        index = index.parent();
    }
    if (index.isValid()) {
        setScreen(index);
    } else {
        qWarning() << "failed to set current widget: screen not found!";
        return;
    }
    // current widgget should be in the scene
    Q_ASSERT(mWidgets.contains(normalizeIndex(current)));
    mWidgets[normalizeIndex(current)]->setSelected(true);
}

void ScreenView::updateSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (mDisableSelectionSlots)
        return;

    FlagSetter fs(&mDisableSelectionSlots);

    for (QModelIndex index : deselected.indexes()) {
        auto it = mWidgets.find(index);
        if (it != mWidgets.end()) {
            (*it)->setSelected(false);
        }
    }
    for (QModelIndex index : selected.indexes()) {
        auto it = mWidgets.find(index);
        if (it != mWidgets.end()) {
            (*it)->setSelected(true);
        }
    }
}
