#pragma once

#include <QDialog>

namespace Ui {
class OutputsListWindow;
}

class OutputsModel;

class OutputsListWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OutputsListWindow(OutputsModel* model, QWidget* parent = nullptr);
    ~OutputsListWindow() final;

private slots:
    void addDefault();
    void removeSelected();

private:
    // refs
    OutputsModel* m_model;
    // own
    Ui::OutputsListWindow* ui;
};

