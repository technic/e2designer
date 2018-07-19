#include "skindelegate.hpp"
#include <QComboBox>

SkinDelegate::SkinDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget *SkinDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::EditRole);
    switch(data.type()) {
    case QVariant::StringList:
    {
        QComboBox *editor = new QComboBox(parent);
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

void SkinDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::EditRole);
    switch(data.type()) {
    case QVariant::StringList:
    {
        // current coresponds to DisplayRole
        // list coresponds to EditRole
        QString current = index.data(Qt::DisplayRole).toString();
        QComboBox *box = static_cast<QComboBox*>(editor);
        box->setCurrentIndex(box->findText(current));
        break;
    }
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void SkinDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::EditRole);
    switch(data.type()) {
    case QVariant::StringList:
    {
        QComboBox *box = static_cast<QComboBox*>(editor);
        QString value = box->currentText();
        model->setData(index, value, Qt::EditRole);
        break;
    }
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
}

void SkinDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
    Q_UNUSED(index);
}
