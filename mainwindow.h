#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "myEditDelagate.h"
#include "config.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QStandardItemModel *model;
    MyEditDelagate *delagate;
    QStandardItem *item;
    QStandardItem *item0[20];
    QStandardItem *item1[10];
    QStandardItem *item2[10];
    Ui::MainWindow *ui;
public slots:
    void action_importConfig_clicked();
    void action_openOut_clicked();
    void action_openExcel_clicked();
    void action_openHex_clicked();
    void action_refreshConfigList_clicked();
    void action_displayCurrentCfg_clicked();
    void action_newConfig_clicked();
    void action_fileConvert_clicked();
    void action_saveFileAs_clicked();
    void action_emuFileConvert_clicked();
    void action_ateFileConvert_clicked();
    void action_testFileConvert_clicked();
    void action_showCfgMethod_clicked();
    void action_saveSpace_clicked();
    void action_switchSpace_clicked();
//    void slotCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
    void treeItemChanged(QStandardItem * item);
    void slotTreeMenu(const QPoint &pos);
    void slotTextMenu(const QPoint &pos);
    void logShowerClear();
    void slotTreeMenuCfgEnable();
    void slotTreeMenuCfgDelete();
    void slotTreeMenuCfgShow();
    void on_pushButton_openSource_clicked();
    void on_pushButton_saveFileAs_clicked();
    void on_toolButton_fileConvert_clicked();
private slots:
    void on_action_enableAteConvert_triggered();
    void on_action_enableTestConvert_triggered();
    void on_action_enableEmuConvert_triggered();
};
void treeItemAdd(MainWindow* window,cfgTemp mCfgTemp,int num,QString currenfCfgName);
extern void logFileRead(MainWindow* window,QString fileName);

#endif // MAINWINDOW_H
