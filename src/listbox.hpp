#pragma once

#include "model/propertytree.hpp"
#include <QComboBox>

class ListBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(SkinEnumList list READ list WRITE setList USER true)

public:
    ListBox(QWidget* parent = Q_NULLPTR)
        : QComboBox(parent)
    {}
    SkinEnumList list() const;
    void setList(SkinEnumList value);
};
