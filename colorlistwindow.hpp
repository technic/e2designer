#ifndef COLORLISTWINDOW_H
#define COLORLISTWINDOW_H

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QDialog>
#include <QDockWidget>

#include <QtColorWidgets/ColorWheel>

namespace Ui {
class ColorListWindow;
}

class ColorsModel;

class ColorListWindow : public QDockWidget
{
    Q_OBJECT

public:
    explicit ColorListWindow(QWidget* parent = Q_NULLPTR);
    ~ColorListWindow();

    QColor color();
public slots:
    void update_widgets();

private slots:
    void remove();
    void add();
    void addDefault();
    void rename();

    void currentChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    QRgb currentColor() const;
    bool confirmAdd(const QString& name);

    Ui::ColorListWindow* ui;
    ColorsModel* mModel;
    QDataWidgetMapper* mMapper;
    QPersistentModelIndex mCurrentIndex;
};

#endif // COLORLISTWINDOW_H
