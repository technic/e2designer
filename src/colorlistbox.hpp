#pragma once

#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QtColorWidgets/color_dialog.hpp>
#include <skin/colorattr.hpp>

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

class ColorChooser : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(ColorAttr color READ color WRITE setColor NOTIFY colorChanged USER true)

public:
    ColorChooser(QWidget* parent = nullptr)
        : QWidget(parent)
        , m_comboBox(new ColorListBox())
        , m_btn(new QPushButton())
        , m_dialog(new color_widgets::ColorDialog(this))
    {
        m_dialog->setAlphaEnabled(true);
        setAutoFillBackground(true);
        auto hbox = new QHBoxLayout(this);
        hbox->setMargin(0);
        hbox->addWidget(m_comboBox);
        hbox->addWidget(m_btn);

        connect(m_comboBox, &ColorListBox::currentTextChanged, this, &ColorChooser::setColorName);
        connect(m_btn, &QPushButton::clicked, this, &ColorChooser::showDialog);
        connect(m_dialog,
                &color_widgets::ColorDialog::colorSelected,
                this,
                &ColorChooser::setQColor);
    }

    ColorAttr color() const { return m_color; }
    void setColor(const ColorAttr& color)
    {
        m_color = color;
        m_comboBox->blockSignals(true);
        m_comboBox->setColor(color.name());
        m_comboBox->blockSignals(false);
    }

signals:
    void colorChanged();

private slots:
    void showDialog() { m_dialog->open(); }
    void setQColor(const QColor& color)
    {
        if (color.isValid()) {
            qDebug() << color << color.alpha();
            setColor(ColorAttr(color));
            emit colorChanged();
        }
    }
    void setColorName(const QString& name)
    {
        if (!name.isEmpty()) {
            setColor(ColorAttr(name));
            emit colorChanged();
        }
    }

private:
    ColorListBox* m_comboBox;
    QPushButton* m_btn;
    color_widgets::ColorDialog* m_dialog;

    ColorAttr m_color;
};
