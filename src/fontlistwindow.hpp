#ifndef FONTLISTWINDOW_H
#define FONTLISTWINDOW_H

#include "model/fontsmodel.hpp"
#include <QDialog>

namespace Ui {
class FontListWindow;
}

class FontListWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FontListWindow(QWidget* parent = Q_NULLPTR);
    ~FontListWindow();

public slots:
    void remove();
    void addDefault();

private:
    Ui::FontListWindow* ui;
    FontsModel* mModel;
};

#endif // FONTLISTWINDOW_H
