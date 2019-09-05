#pragma once

#include <QComboBox>

class ColorListBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QString color READ color WRITE setColor USER true)

public:
    ColorListBox(QWidget* parent = nullptr);

    QString color() const;
    void setColor(const QString& color);

private:
    void populateList();
};

