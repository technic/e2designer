#ifndef COLORLISTBOX_H
#define COLORLISTBOX_H

#include <QComboBox>

class ColorListBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor USER true)

public:
    ColorListBox(QWidget* parent = nullptr);

    QColor color() const;
    void setColor(const QColor& color);

private:
    void populateList();
};

#endif // COLORLISTBOX_H
