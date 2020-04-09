#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QColorDialog>
#include <QFile>
#include <QBuffer>
#include <QFileDialog>
#include <QGraphicsView>
#include <QItemEditorFactory>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QRgb>
#include <QScreen>
#include <QSettings>
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QSplitter>

#include "colorlistbox.hpp"
#include "colorlistwindow.hpp"
#include "editor/xmlhighlighter.hpp"
#include "fontlistwindow.hpp"
#include "listbox.hpp"
#include "model/colorsmodel.hpp"
#include "repository/skinrepository.hpp"
#include "outputslistwindow.hpp"

#ifdef APPIMAGE_UPDATE
#include <AppImageUpdaterDialog>

using AppImageUpdaterBridge::AppImageUpdaterDialog;
#endif

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scene(new SkinScene(SkinRepository::screens()))
    , m_propertiesModel(new PropertiesModel(SkinRepository::screens(), this))
#ifdef APPIMAGE_UPDATE
    , m_updater(new AppImageUpdaterDialog(QPixmap(":/icons/misc/e2designer.png"), this))
#else
    , m_updater(nullptr)
#endif
{
    ui->setupUi(this);
    readSettings();
    createActions();

    // hide editor by default
    ui->splitterR->setSizes(QList<int>{ 0, 1 });

    //
    QFont monoFont("Monospace");
    monoFont.setStyleHint(QFont::TypeWriter);
    ui->textEdit->setFont(monoFont);
    auto highlighter = new XMLHighlighter(ui->textEdit->document());
    Q_UNUSED(highlighter);

    connect(ui->textEdit,
            &QPlainTextEdit::modificationChanged,
            ui->refreshButton,
            &QPushButton::setEnabled);

    ui->treeView->setModel(SkinRepository::screens());
    ui->treeView->setDragEnabled(true);
    //	ui->treeView->setDropIndicatorShown(true);
    ui->treeView->setAcceptDrops(true);
    //	ui->treeView->setDefaultDropAction(Qt::MoveAction);
    //	// accept dnd actions only from it self
    ui->treeView->setDragDropMode(QAbstractItemView::InternalMove);

    // Undo stack
    auto undoStack = SkinRepository::screens()->undoStack();
    ui->undoListView->setStack(undoStack);
    connect(undoStack, &QUndoStack::cleanChanged, this, [=](bool clean) {
        setWindowModified(!clean);
    });

    connect(&SkinRepository::instance(),
            &SkinRepository::filePathChanged,
            this,
            &MainWindow::setTitle);

    ui->propView->setModel(m_propertiesModel);
    // start editing with one click
    connect(ui->propView,
            &QTreeView::clicked,
            ui->propView,
            qOverload<const QModelIndex&>(&QTreeView::edit));

    //	SkinDelegate *delegate = new SkinDelegate(this);
    //	ui->propView->setItemDelegate(delegate);

    ui->graphicsView->setScene(m_scene);

    // Setup default editors
    auto* delegate = new QStyledItemDelegate(this);
    auto* factory = new QItemEditorFactory();
    factory->registerEditor(QVariant::Color, new QStandardItemEditorCreator<ColorListBox>());
    factory->registerEditor(qMetaTypeId<SkinEnumList>(), new QStandardItemEditorCreator<ListBox>());
    delegate->setItemEditorFactory(factory);
    ui->propView->setItemDelegate(delegate);

#ifdef APPIMAGE_UPDATE
    // Center the update dialog
    m_updater->move(QGuiApplication::primaryScreen()->geometry().center()
                    - m_updater->rect().center());
#endif

    // Synchronize selections

    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_scene->setSelectionModel(ui->treeView->selectionModel());

    connect(ui->treeView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &MainWindow::onCurrentSelectionChanged);
}

MainWindow::~MainWindow()
{
    delete m_scene;
    delete ui;
}

void MainWindow::openFile(const QString& dirname)
{
    auto& model = SkinRepository::instance();
    bool ok = model.open(dirname);
    if (!ok) {
        QMessageBox::warning(
          this,
          tr("Error"),
          tr("Failed to open skin to directory:\n%1\n%2.").arg(dirname).arg(model.lastError()));
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (confirmClose()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::onCurrentSelectionChanged(const QModelIndex& index, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    qDebug() << "current changed" << index;
    m_propertiesModel->setWidget(index);
    for (int i = 0; i < m_propertiesModel->rowCount(); i++) {
        auto index = m_propertiesModel->index(i, 0);
        if (index.data(ShallExpandRole).toBool()) {
            ui->propView->expand(index);
        }
    }

    setEditorText(index);
}

void MainWindow::newSkin()
{
    auto& model = SkinRepository::instance();
    if (confirmClose()) {
        QSettings settings(QCoreApplication::organizationName(),
                           QCoreApplication::applicationName());
        QString startdir = settings.value("lastdir").toString();
        const QString fname = QFileDialog::getSaveFileName(this,
                                                           tr("New skin file"),
                                                           startdir + "/skin.xml",
                                                           tr("Skin file (skin.xml)"),
                                                           nullptr/*,
                                                           QFileDialog::DontUseNativeDialog*/);
        if (fname.isNull())
            return;

        QString dir = QFileInfo(fname).absoluteDir().path();
        qDebug() << "creating new skin in" << dir;
        settings.setValue("lastdir", dir);
        bool ok = model.create(dir);
        if (!ok) {
            QMessageBox::warning(
              this,
              tr("Error"),
              tr("Failed to create skin in %1:\n%2").arg(dir).arg(model.lastError()));
        }
    }
}

void MainWindow::open()
{
    if (confirmClose()) {
        QSettings settings(QCoreApplication::organizationName(),
                           QCoreApplication::applicationName());
        QString startdir = settings.value("lastdir").toString();
        const QString fname =
          QFileDialog::getOpenFileName(this, tr("Open skin"), startdir, tr("Skin file (skin.xml)"));
        if (!fname.isNull()) {
            QString dir = QFileInfo(fname).absoluteDir().path();
            qDebug() << "opening" << dir;
            settings.setValue("lastdir", dir);
            openFile(dir);
        }
    }
}

bool MainWindow::save()
{
    auto& model = SkinRepository::instance();
    if (!model.isOpened()) {
        return saveAs();
    }
    bool saved = model.save();
    if (!saved) {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Failed to save skin to directory:\n%1\n%2.")
                               .arg(model.dir().absolutePath())
                               .arg(model.lastError()));
    }
    return saved;
}

bool MainWindow::saveAs()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString startdir = settings.value("lastdir").toString();
    const QString path = QFileDialog::getSaveFileName(this,
                                                      tr("Select new skin folder"),
                                                      startdir,
                                                      tr("Skin file (skin.xml)"));
    if (!path.isNull()) {
        auto& model = SkinRepository::instance();
        QString dir = QFileInfo(path).absoluteDir().path();
        qDebug() << "saving to" << dir;
        settings.setValue("lastdir", dir);
        bool saved = model.saveAs(dir);
        if (!saved) {
            QMessageBox::warning(
              this,
              tr("Error"),
              tr("Failed to save skin to directory:\n%1\n%2.").arg(path).arg(model.lastError()));
        }
        return saved;
    }
    return false;
}

void MainWindow::about()
{
    QMessageBox::about(this,
                       QString("%1 %2").arg(tr("e2designer"),
                                            QCoreApplication::applicationVersion()),
                       tr("The <b>e2designer</b> allows you to edit enigma2 "
                          "skins in a nice way"
                          "<br>Usefull hints:"
                          "<ul>"
                          "<li>Arrow keys - move object<li>"
                          "<li>Shift + Arrow keys - presize move object<li>"
                          "<li>Ctrl + Left mouse click - multiple selection<li>"
                          "</ul>"));
}

void MainWindow::setTitle(const QString& skinPath)
{
    setWindowTitle(QString("%1[*] - %2").arg(skinPath, QCoreApplication::applicationName()));
}

void MainWindow::addSkinItem(WidgetData::WidgetType type)
{
    // Get parent screen for new skin item
    auto screen = ui->treeView->selectionModel()->currentIndex();
    if (!screen.isValid()) {
        QMessageBox::warning(this, tr("Error"), tr("Can not add widget to the skin top level"));
        return;
    }
    // TODO: should model provide more friendly interface?
    auto& model = *SkinRepository::screens();
    model.insertWidget(model.rowCount(screen), screen, type);
    QModelIndex widget = model.index(model.rowCount(screen) - 1, 0, screen);
    auto size = SizeAttr(100, 100);
    model.setWidgetAttr(widget, Property::size, QVariant::fromValue(size));
    if (type == WidgetData::WidgetType::Widget) {
        model.setWidgetAttr(widget, Property::name, "Untitled");
    } else if (type == WidgetData::WidgetType::Label) {
        model.setWidgetAttr(widget, Property::text, "Default text");
    }
}

void MainWindow::addWidget()
{
    addSkinItem(WidgetData::WidgetType::Widget);
}
void MainWindow::addPixmap()
{
    addSkinItem(WidgetData::WidgetType::Pixmap);
}
void MainWindow::addLabel()
{
    addSkinItem(WidgetData::WidgetType::Label);
}
void MainWindow::addScreen()
{
    auto& model = *SkinRepository::screens();
    // TODO: should model provide more friendly interface?
    QModelIndex root;
    model.insertWidget(model.rowCount(root), root, WidgetData::WidgetType::Screen);
    QModelIndex screen = model.index(model.rowCount(root) - 1, 0, root);
    auto pos = PositionAttr("center,center");
    auto size = SizeAttr(300, 200);
    model.setWidgetAttr(screen, Property::position, QVariant::fromValue(pos));
    model.setWidgetAttr(screen, Property::size, QVariant::fromValue(size));
    model.setWidgetAttr(screen, Property::name, "UntitledScreen");
}

void MainWindow::delWidget()
{
    if (!m_scene)
        return;
    QModelIndex idx = ui->treeView->selectionModel()->currentIndex();
    SkinRepository::screens()->removeRow(idx.row(), idx.parent());
    //    mView->deleteSelected();
    // FIXME: is m_view always authoritative?
}

void MainWindow::editColors()
{
    auto colorsWindow = new ColorListWindow(this);
    colorsWindow->setFloating(true);
    colorsWindow->show();
}

void MainWindow::editFonts()
{
    auto fontsWindow = new FontListWindow(this);
    fontsWindow->show();
}

void MainWindow::editOutputs()
{
    auto outputsWindow = new OutputsListWindow(SkinRepository::outputs(), this);
    outputsWindow->show();
}

void MainWindow::fitWidgetToPixmap()
{
    auto* model = SkinRepository::screens();
    auto index = ui->treeView->selectionModel()->currentIndex();
    QString path = model->widget(index).pixmap(Property::pixmap);
    QPixmap pixmap(SkinRepository::instance().resolveFilename(path));
    if (!pixmap.isNull())
        model->resizeWidget(index, pixmap.size());
}

void MainWindow::checkUpdates()
{
#ifdef APPIMAGE_UPDATE
    auto fileName = appImagePath();
    if (fileName.isEmpty()) {
        qWarning() << "Can't update! Are you running AppImage?";
        return;
    }
    m_updater->init();
#endif
}

void MainWindow::setEditorText(const QModelIndex& index)
{
    // Don't write directly to QString,
    // because modified writer does not support it.
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);

    XmlStreamWriter xml(&buf);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    SkinRepository::screens()->widget(index).toXml(xml);
    auto str = QString::fromUtf8(buf.buffer());
    if (str.startsWith('\n')) {
        str.remove(0, 1);
    }
    ui->textEdit->document()->setPlainText(str);
    ui->textEdit->document()->setModified(false);
}

void MainWindow::loadEditorText()
{
    auto* model = SkinRepository::screens();
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QModelIndex parent = index.parent();
    QXmlStreamReader xml(ui->textEdit->toPlainText());
    bool ok = xml.readNextStartElement();
    if (!ok) {
        QMessageBox::warning(this, tr("XML parser error"), xml.errorString());
        return;
    }
    if (!model->setWidgetDataFromXml(index, xml)) {
        QMessageBox::warning(this, tr("XML parser error"), xml.errorString());
        return;
    }
    index = model->index(index.row(), ScreensModel::ColumnElement, parent);
    ui->treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Current);
    ui->textEdit->document()->setModified(false);
}

void MainWindow::showXmlEditor(bool show)
{
    if (show) {
        ui->splitterR->setSizes(QList<int>{ 300, 300 });
    } else {
        ui->splitterR->setSizes(QList<int>{ 0, 10 });
    }
}

void MainWindow::createActions()
{
    // Connect actions
    auto undoStack = SkinRepository::screens()->undoStack();
    connect(ui->actionUndo, &QAction::triggered, undoStack, &QUndoStack::undo);
    connect(ui->actionRedo, &QAction::triggered, undoStack, &QUndoStack::redo);
    connect(undoStack, &QUndoStack::canUndoChanged, ui->actionUndo, &QAction::setEnabled);
    connect(undoStack, &QUndoStack::canRedoChanged, ui->actionRedo, &QAction::setEnabled);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newSkin);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionAddWidget, &QAction::triggered, this, &MainWindow::addWidget);
    connect(ui->actionDeleteWidget, &QAction::triggered, this, &MainWindow::delWidget);
    connect(ui->actionAddPixmap, &QAction::triggered, this, &MainWindow::addPixmap);
    connect(ui->actionAddLabel, &QAction::triggered, this, &MainWindow::addLabel);
    connect(ui->actionAddScreen, &QAction::triggered, this, &MainWindow::addScreen);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, this, &QApplication::aboutQt);

    connect(ui->actionCheckForUpdates, &QAction::triggered, this, &MainWindow::checkUpdates);
    if (appImagePath().isEmpty()) {
        ui->actionCheckForUpdates->setEnabled(false);
        ui->actionCheckForUpdates->setVisible(false);
    }

    // TODO: ui->actionWidget_borders->setChecked(m_scene->haveBorders());
    connect(ui->actionWidget_borders, &QAction::triggered, m_scene, &SkinScene::displayBorders);
    connect(ui->actionXmlEditor, &QAction::triggered, this, &MainWindow::showXmlEditor);
    connect(ui->actionFitPixmap, &QAction::triggered, this, &MainWindow::fitWidgetToPixmap);

    connect(ui->actionUndoStack, &QAction::triggered, ui->dockWidget, &QDockWidget::setVisible);
    connect(ui->dockWidget,
            &QDockWidget::visibilityChanged,
            ui->actionUndoStack,
            &QAction::setChecked);

    connect(ui->actionToolbar, &QAction::triggered, ui->mainToolBar, &QToolBar::setVisible);
    connect(ui->mainToolBar, &QToolBar::visibilityChanged, ui->actionToolbar, &QAction::setChecked);

    connect(ui->actionEditColors, &QAction::triggered, this, &MainWindow::editColors);
    connect(ui->actionEditFonts, &QAction::triggered, this, &MainWindow::editFonts);
    connect(ui->actionEditOutputs, &QAction::triggered, this, &MainWindow::editOutputs);

    // Connect buttons
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::loadEditorText);
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        setWindowState(Qt::WindowMaximized);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::confirmClose()
{
    if (!isModified())
        return true;

    const QMessageBox::StandardButton ret =
      QMessageBox::warning(this,
                           tr("Application"),
                           tr("The skin has been modified.\n"
                              "Do you want to save your changes?"),
                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        return true;
    }
}

bool MainWindow::isModified()
{
    return !SkinRepository::screens()->undoStack()->isClean();
}

QString MainWindow::appImagePath()
{
    auto str = qgetenv("APPIMAGE");
    qDebug() << "APPIMAGE" << str;
    return str;
}
