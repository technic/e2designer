#pragma once

#include <QGraphicsScene>
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QString>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "model/propertiesmodel.hpp"
#include "scene/screenview.hpp"

// Forward declarations

namespace Ui {
class MainWindow;
}

namespace AppImageUpdaterBridge {
class AppImageUpdaterDialog;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = Q_NULLPTR);
    ~MainWindow() override;

    void openFile(const QString& dirname);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onCurrentSelectionChanged(const QModelIndex& index, const QModelIndex& previous);

    void newSkin();
    void open();
    bool save();
    bool saveAs();
    void about();
    void setTitle(const QString& skinPath);

    void addSkinItem(WidgetData::WidgetType type);
    void addWidget();
    void addPixmap();
    void addLabel();
    void addScreen();
    void addPanel();

    void delWidget();
    void editColors();
    void editFonts();
    void editOutputs();
    void fitWidgetToPixmap();

    void checkUpdates();
    void setEditorText(const QModelIndex& index);
    void loadEditorText();
    void showXmlEditor(bool show);

private:
    // menu and toolbar
    void createActions();
    void readSettings();
    void writeSettings();
    // shows save before close dialog
    bool confirmClose();
    bool isModified();
    // AppImage
    QString appImagePath();

    Ui::MainWindow* ui;

    //	SkinModel *m_model;

    SkinScene* m_scene;

    // QObject owned
    PropertiesModel* m_propertiesModel;
    AppImageUpdaterBridge::AppImageUpdaterDialog* m_updater;

    //	QSortFilterProxyModel *m_topfilter;
    //	QSortFilterProxyModel *m_headerfilter;
};
