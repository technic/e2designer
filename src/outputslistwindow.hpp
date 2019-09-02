#ifndef OUTPUTSLISTWINDOW_HPP
#define OUTPUTSLISTWINDOW_HPP

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
    ~OutputsListWindow();

private slots:
    void addDefault();
    void removeSelected();

private:
    // refs
    OutputsModel* m_model;
    // own
    Ui::OutputsListWindow* ui;
};

#endif // OUTPUTSLISTWINDOW_HPP
