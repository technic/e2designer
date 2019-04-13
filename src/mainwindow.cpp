#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsView>
#include <QItemEditorFactory>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QRgb>
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
#endif

using namespace AppImageUpdaterBridge;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mView(new ScreenView(SkinRepository::screens()))
    , mPropertiesModel(new PropertiesModel(SkinRepository::screens(), this))
#ifdef APPIMAGE_UPDATE
    , m_updater(new AppImageUpdaterDialog(QPixmap(), this))
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

    ui->propView->setModel(mPropertiesModel);
    ui->propView->setIndentation(5);

    //	SkinDelegate *delegate = new SkinDelegate(this);
    //	ui->propView->setItemDelegate(delegate);

    ui->graphicsView->setScene(mView->scene());

    // Setup default editors
    auto* delegate = new QStyledItemDelegate(this);
    auto* factory = new QItemEditorFactory();
    factory->registerEditor(QVariant::Color, new QStandardItemEditorCreator<ColorListBox>());
    factory->registerEditor(qMetaTypeId<SkinEnumList>(), new QStandardItemEditorCreator<ListBox>());
    delegate->setItemEditorFactory(factory);
    ui->propView->setItemDelegate(delegate);

    // Synchronize selections

    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mView->setSelectionModel(ui->treeView->selectionModel());

    connect(ui->treeView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &MainWindow::onCurrentSelectionChanged);
}

MainWindow::~MainWindow()
{
    delete mView;
    delete ui;
}

void MainWindow::openFile(const QString& dirname)
{
    SkinRepository::instance().open(dirname);
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
    mPropertiesModel->setWidget(index);
    ui->propView->expandAll();

    setEditorText(index);
}

void MainWindow::newSkin()
{
    if (confirmClose()) {
        // TODO: create new skin
    }
}

void MainWindow::open()
{
    if (confirmClose()) {
        QSettings settings(QCoreApplication::organizationName(),
                           QCoreApplication::applicationName());
        QString startdir = settings.value("lastdir").toString();
        //		if (startdir)
        //		QDir::home()
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

void MainWindow::addSkinItem(int type)
{
    // FIXME: is m_view always athorative?
    // TODO: should model provide more friendly interface?
    auto& model = *SkinRepository::screens();
    QModelIndex screen = mView->currentIndex();
    model.insertRow(model.rowCount(screen), screen);
    QModelIndex widget = model.index(model.rowCount(screen) - 1, 0, screen);
    model.setData(widget, type, ScreensModel::TypeRole);
    auto size = SizeAttr(100, 100);
    model.setWidgetAttr(widget, Property::size, QVariant::fromValue(size));
    if (type == WidgetData::Widget) {
        model.setWidgetAttr(widget, Property::name, "Untitled");
    } else if (type == WidgetData::Label) {
        model.setWidgetAttr(widget, Property::text, "Default text");
    }
}

void MainWindow::addWidget()
{
    addSkinItem(WidgetData::Widget);
}
void MainWindow::addPixmap()
{
    addSkinItem(WidgetData::Pixmap);
}
void MainWindow::addLabel()
{
    addSkinItem(WidgetData::Label);
}
void MainWindow::addScreen()
{
    auto& model = *SkinRepository::screens();
    // TODO: should model provide more friendly interface?
    QModelIndex root;
    model.insertRow(model.rowCount(root), root);
    QModelIndex screen = model.index(model.rowCount(root) - 1, 0, root);
    model.setData(screen, WidgetData::Screen, ScreensModel::TypeRole);
    auto pos = PositionAttr("center,center");
    auto size = SizeAttr(300, 200);
    model.setWidgetAttr(screen, Property::position, QVariant::fromValue(pos));
    model.setWidgetAttr(screen, Property::size, QVariant::fromValue(size));
    model.setWidgetAttr(screen, Property::name, "UntitledScreen");
    QVariant(QColor(Qt::red));
}

void MainWindow::delWidget()
{
    if (!mView)
        return;
    QModelIndex idx = ui->treeView->selectionModel()->currentIndex();
    SkinRepository::screens()->removeRow(idx.row(), idx.parent());
    //    mView->deleteSelected();
    // FIXME: is m_view always athorative?
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

void MainWindow::checkUpdates()
{
#ifdef APPIMAGE_UPDATE
    auto fileName = appImagePath();
    if (fileName.isEmpty()) {
        qWarning() << "Can't update! Are you running AppImage?";
        return;
    }
    m_updater->setAppImage(fileName);
    m_updater->setShowLog(true);
    m_updater->init();
#endif
}

void MainWindow::setEditorText(const QModelIndex& index)
{
    QString str;
    XmlStreamWriter xml(&str);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    SkinRepository::screens()->widget(index).toXml(xml);
    if (str.startsWith('\n')) {
        str.remove(0, 1);
    }
    ui->textEdit->document()->setPlainText(str);
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

    ui->actionWidget_borders->setChecked(mView->haveBorders());
    connect(ui->actionWidget_borders, &QAction::triggered, mView, &ScreenView::displayBorders);

    connect(ui->actionEditColors, &QAction::triggered, this, &MainWindow::editColors);
    connect(ui->actionEditFonts, &QAction::triggered, this, &MainWindow::editFonts);
    connect(ui->actionEditOutputs, &QAction::triggered, this, &MainWindow::editOutputs);
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
