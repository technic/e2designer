#pragma once

#include <QStyledItemDelegate>

/**
 * @brief The SkinDelegate class
 * delegate for editing skin properties
 */

class SkinDelegate : public QStyledItemDelegate
{
public:
    SkinDelegate(QObject* parent = Q_NULLPTR);

    /* QStyledItemDelegate */
    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const Q_DECL_OVERRIDE;
};
