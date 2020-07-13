#include "skindelegate.hpp"
#include <QComboBox>
#include <QLineEdit>
#include <QApplication>
#include <QTableView>
#include <QPainter>
#include <QDebug>

SkinDelegate::SkinDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

QWidget* SkinDelegate::createEditor(QWidget* parent,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
    QVariant data = index.data(Qt::EditRole);
    switch (data.type()) {
    case QVariant::StringList: {
        auto* editor = new QComboBox(parent);
        editor->addItems(data.toStringList());
        return editor;
    }

        //	case You need regex:
        //		QLineEdit *edit = new QLineEdit(parent);
        //		QRegExp rx("\\w*,\\w*");
        //		Q_ASSERT(rx.isValid());
        //		QValidator *v = new QRegExpValidator(rx, edit);
        //		edit->setValidator(v);

    default:
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void SkinDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QVariant data = index.data(Qt::EditRole);
    switch (data.type()) {
    case QVariant::StringList: {
        // current coresponds to DisplayRole
        // list coresponds to EditRole
        QString current = index.data(Qt::DisplayRole).toString();
        auto* box = static_cast<QComboBox*>(editor);
        box->setCurrentIndex(box->findText(current));
        break;
    }
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void SkinDelegate::setModelData(QWidget* editor,
                                QAbstractItemModel* model,
                                const QModelIndex& index) const
{
    QVariant data = index.data(Qt::EditRole);
    switch (data.type()) {
    case QVariant::StringList: {
        auto* box = static_cast<QComboBox*>(editor);
        QString value = box->currentText();
        model->setData(index, value, Qt::EditRole);
        break;
    }
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
}

void SkinDelegate::updateEditorGeometry(QWidget* editor,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    editor->setGeometry(option.rect);

    Q_UNUSED(index);
}

// void ColorPropertyPainter::paint(QPainter* painter,
//                                 const QStyleOptionViewItem& option,
//                                 const QModelIndex& index) const
//{
//    option.rect.height() painter->fillRect(0,
//                                           0,
//                                           option.rect.height(),
//                                           qMin(option.rect.width(), option.rect.height()),
//                                           QBrush());
//}

void PropertyDelegate::updateEditorGeometry(QWidget* editor,
                                            const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
    int type = index.data(Qt::EditRole).userType();
    if (type == qMetaTypeId<ColorAttr>()) {
        editor->setGeometry(option.rect);

        qDebug() << option.rect << "SIZE:" << editor->size() << "hint/min/max" << editor->sizeHint()
                 << editor->minimumSize() << editor->maximumSize();

    } else {
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
    return;

    if (painters.contains(type)) {
        editor->setGeometry(option.rect);
    } else {
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
}
