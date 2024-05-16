#ifndef MYEDITDELAGATE_H
#define MYEDITDELAGATE_H



#include <QStyledItemDelegate>

class MyEditDelagate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MyEditDelagate(QObject *parent);
    ~MyEditDelagate();
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // MYEDITDELAGATE_H
