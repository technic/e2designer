#pragma once

#include <memory>
#include <QMetaObject>
#include <QMetaProperty>
#include <QStyledItemDelegate>

#include <skin/colorattr.hpp>

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

class BaseDelegatePainter
{
public:
    virtual void paint(QPainter* painter,
                       const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;
};

class PropertyDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    PropertyDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {}

    void registerDelegatePainter(int type, std::unique_ptr<BaseDelegatePainter>&& painterImpl)
    {
        //        painters.insert(type, painterImpl);
    }

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override
    {
        auto editor = QStyledItemDelegate::createEditor(parent, option, index);
        if (editor) {
            int type = index.data(Qt::EditRole).userType();
            if (type == qMetaTypeId<ColorAttr>()) {
                connectEditorNotification(editor);
            }
        }
        return editor;
    }

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        int type = index.data(Qt::EditRole).userType();
        auto painterImpl = painters.value(type);
        if (painterImpl) {
            painterImpl->paint(painter, option, index);
        } else {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }

protected:
    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;

private slots:
    void commitEditorData()
    {
        QWidget* editor = qobject_cast<QWidget*>(sender());
        emit commitData(editor);
    }

private:
    void connectEditorNotification(QWidget* editor) const
    {
        QMetaProperty prop = editor->metaObject()->userProperty();
        if (prop.isValid() && prop.hasNotifySignal()) {
            // FIXME: Is there better way to construct QMetaMethod?
            int slotIndex = metaObject()->indexOfSlot("commitEditorData()");
            Q_ASSERT(slotIndex >= 0);
            connect(editor, prop.notifySignal(), this, metaObject()->method(slotIndex));
        }
    }

    QHash<int, BaseDelegatePainter*> painters;
};

class ColorPropertyPainter : public BaseDelegatePainter
{
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const final;
};
