#pragma once

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
    ~FontListWindow() override;

public slots:
    void remove();
    void addDefault();

private:
    Ui::FontListWindow* ui;
    FontsModel* m_model;
};

