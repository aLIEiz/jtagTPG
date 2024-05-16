#include "myEditDelagate.h"
#include <QComboBox>

MyEditDelagate::MyEditDelagate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

MyEditDelagate::~MyEditDelagate()
{
}

QWidget *MyEditDelagate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
{
    /* 只对第1列采用此方法编辑 */
    if (index.column() == 1)
    {
        QComboBox* box = new QComboBox(parent);
        box->addItems({
                          QString::fromLocal8Bit("core0"),
                          QString::fromLocal8Bit("core1"),
                          QString::fromLocal8Bit("core2") });
        return box;
    }
    return NULL;
}

void MyEditDelagate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox* box = static_cast<QComboBox*>(editor);
    box->setCurrentText(value);
}

void MyEditDelagate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* box = static_cast<QComboBox*>(editor);
    model->setData(index, box->currentText(), Qt::EditRole);
}

void MyEditDelagate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

