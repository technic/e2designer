#pragma once

#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QProxyStyle>
#include <QToolButton>
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
        // Editor is shown on top of the delegate.
        // Always fill background, to ensure that the editor not transparent
        setAutoFillBackground(true);

        // Do not expand the button horizontally
        m_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        m_btn->setIcon(QIcon(":/icons/icons/color-circle.svg"));

        // Enable alpha channel editing in color dialog
        m_dialog->setAlphaEnabled(true);

        auto layout = new QHBoxLayout(this);
        layout->setMargin(0);
        layout->addWidget(m_comboBox);
        layout->addWidget(m_btn);

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
        m_comboBox->setColor(color.toString());
        m_comboBox->blockSignals(false);
    }

signals:
    void colorChanged();

private slots:
    void showDialog()
    {
        if (m_color.state() == ColorAttr::State::Fixed) {
            m_dialog->showColor(m_color.value());
        } else {
            m_dialog->open();
        }
    }

    void setQColor(const QColor& color)
    {
        if (color.isValid()) {
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
    ColorAttr m_color;

    // QObject owned
    ColorListBox* m_comboBox;
    QPushButton* m_btn;
    color_widgets::ColorDialog* m_dialog;
};
