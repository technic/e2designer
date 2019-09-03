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
    ~ColorListWindow() override;

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    QColor color();
    void setColor(const QColor& color);

signals:
    void colorChanged();

public slots:
    // Color selector widgets
    void setFromWheel();
    void setRgb();
    void setHsv();
    void setHex(const QColor& color);
    void setAlpha();

private slots:
    // Colors model
    void remove();
    void add();
    void addDefault();
    void rename();

    void moveUp();
    void moveDown();

    void currentChanged(const QModelIndex& current, const QModelIndex& previous);

    void updateColorWidgets(const QColor& col);

private:
    QRgb currentColor() const;
    bool confirmAdd(const QString& name);

    Ui::ColorListWindow* ui;
    ColorsModel* m_model;
    bool m_updating;
    QDataWidgetMapper* m_mapper;
    QPersistentModelIndex m_currentIndex; // TODO: remove
};

#endif // COLORLISTWINDOW_H
