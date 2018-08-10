#include "screenview.hpp"
#include "backgroundpixmap.hpp"
#include "foregroundwidget.hpp"
#include "repository/skinrepository.hpp"
#include <QCoreApplication>
#include <QGraphicsPixmapItem>

ScreenView::ScreenView(ScreensModel* model)
    : mModel(model)
    , mScene(new QGraphicsScene(this))
    , mSelector(new RectSelector(nullptr))
    , mBackground(new QGraphicsPixmapItem(QPixmap(":/background.jpg")))
{
    mScene->addItem(mBackground);

    // set inital scene size and subscribe to changes
    setSceneSize(SkinRepository::outputs()->getOutput(mOutputId).size());
    connect(SkinRepository::outputs(), &VideoOutputRepository::valueChanged,
            this, &ScreenView::onOutputChanged);

    mSelector->setZValue(1000.);
    mSelector->setPen(Qt::NoPen);
    mScene->addItem(mSelector);

    //	// Fill in with transparent layer by default
    //	QRectF r(0, 0, sz.width(), sz.height());
    //	auto *foreground = new ForegroundWidget(r, nullptr);
    //	foreground->setBrush(QBrush(QColor(0,0,0,10)));
    //	m_scene->addItem(foreground);

    connect(mModel, &ScreensModel::rowsAboutToBeRemoved, this, &ScreenView::onRowsAboutToBeRemoved);
    connect(mModel, &ScreensModel::rowsInserted, this, &ScreenView::onRowsInserted);
    connect(mModel, &ScreensModel::modelReset, this, &ScreenView::onModelReset);

    connect(mScene, &QGraphicsScene::selectionChanged, this, &ScreenView::onSceneSelectionChanged);
}

void ScreenView::selectCurrentWidget(QModelIndex selected, QModelIndex deselected)
{
    Q_UNUSED(deselected);
    QModelIndex index = selected;
    Q_ASSERT(mWidgets.contains(index));
    mScene->clearSelection();
    mWidgets[index]->setSelected(true);
}

void ScreenView::onOutputChanged(int id, const VideoOutput &output)
{
    if (id == mOutputId) {
        setSceneSize(output.size());
    }
}

void ScreenView::setSceneSize(const QSize &size)
{
    mScene->setSceneRect(0, 0, size.width(), size.height());
    // Adjust background scale
    QSizeF pixmapSize = mBackground->boundingRect().size();
    qreal sw = size.width() / pixmapSize.width();
    qreal sh = size.height() / pixmapSize.height();
    mBackground->setTransform(QTransform::fromScale(sw, sh));
}

void ScreenView::setScreen(QModelIndex index)
{
    Q_ASSERT(index.data(ScreensModel::TypeRole).toInt() == WidgetData::Screen);

    if (mRoot == index)
        return;

    qDebug() << "to delete:" << mWidgets.size();

    WidgetView* oldScreen = mWidgets[mRoot];
    if (oldScreen) {
        // All items must be childs of the oldScreen
        mScene->removeItem(oldScreen);
        delete oldScreen;
    }
    mWidgets.clear();

    mRoot = index.sibling(index.row(), ScreensModel::ColumnElement);
    WidgetView* screen = new WidgetView(this, mRoot, nullptr);
    mWidgets[mRoot] = screen;
    mScene->addItem(screen);

    for (int i = 0; i < mModel->rowCount(mRoot); ++i) {
        QModelIndex widgetIndex = mModel->index(i, ScreensModel::ColumnElement, mRoot);
        WidgetView* view = new WidgetView(this, widgetIndex, screen);
        Q_ASSERT(!mWidgets.contains(widgetIndex));
        mWidgets[widgetIndex] = view;
    }
}

void ScreenView::point(QItemSelectionModel* model)
{
    if (model) {
        mSelectionModel = model;
        connect(mSelectionModel, &QItemSelectionModel::currentChanged, this,
                &ScreenView::selectCurrentWidget);
    }
}

void ScreenView::deleteSelected()
{
    // I only delete last clicked one
    QGraphicsItem* item = mScene->selectedItems().last();
    // FIXME: size check
    if (item->type() == WidgetView::Type) {
        WidgetView* w = static_cast<WidgetView*>(item);
        QModelIndex i = w->modelIndex();
        mModel->removeRow(i.row(), i.parent());
    } else {
        qWarning() << "garbage selected";
    }
}

void ScreenView::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    // check if delete our root
    const int r = mRoot.row();
    if (mRoot.parent() == parent && first <= r && r <= last) {
        WidgetView* screen = mWidgets[mRoot];
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
                WidgetView* w = mWidgets.take(index);
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
    WidgetView* screen = mWidgets[mRoot];

    for (int i = first; i <= last; ++i) {
        QModelIndex windex = mModel->index(i, 0, parent);
        WidgetView* view = new WidgetView(this, windex, screen);
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

void ScreenView::onSceneSelectionChanged()
{
    //    for (auto item: mScene->selectedItems()) {
    //        if (item->type() != WidgetView::Type)
    //            continue;
    //        WidgetView *w = static_cast<WidgetView*>(item);
    //        QModelIndex index = w->modelIndex();
    //        if (mSelectionModel->isSelected(index)) {
    //            mSelectionModel->select(index, QItemSelectionModel::Select);
    //        }
    //    }

    if (!mScene->selectedItems().isEmpty()) {
        QGraphicsItem* i = mScene->selectedItems().back();
        if (i->type() == WidgetView::Type) {
            emit selectionChanged(static_cast<WidgetView*>(i)->modelIndex());
        }
    }
}

// bool ScreenView::isInOurView(QModelIndex index)
//{
//	// we track only first column
//	widget = index.sibling(widget.row(), 0);

//	while (widget.isValid() && !isWidget(widget)) {
//		widget = widget.parent();
//	}
//	QModelIndex screen = widget;
//	while (screen.isValid() && screen.data(RoleType) != TypeScreen) {
//		screen = screen.parent();
//	}
//	if (!screen.isValid()) {
//		qDebug() << "selected not in screen";
//		return;
//	}
//	if (!(m_view && m_view->currentIndex() == screen)) {
//		if (m_view)
//			delete m_view;
//		m_view = new ScreenView(m_model, screen);
//	    ui->graphicsView->setScene(m_view->scene());
//		ui->graphicsView->fitInView(m_view->scene()->sceneRect(),
// Qt::KeepAspectRatio);
//		QObject::connect(m_view, &ScreenView::selectionChanged, this,
//&MainWindow::updateSceneSelection);
//	}
//	m_view->selectWidget(widget);

//}
