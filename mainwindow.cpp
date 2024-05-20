#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QFileDialog>
#include <QComboBox>
#include <QAbstractItemView>
#include <QObject>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <qmath.h>
#include <iostream>
#include "winCmd.h"
#include "jtag.h"
#include "excel.h"
#include"coffTrans.h"
#include "hex.h"
#include "elf.h"
//config list
int gItemNum = 0;
QString gDefaultFilePath;
QString gStrFileName;
QString gLogFileName;
QString gChoosedFileName;

cfgList gCfgList;
JtagList* gJtagCmdList;
LongCmdList* gLongCmdList;
cfgTemp gCurrentCfg;
QStandardItemModel *gModel;
MainWindow* gWindow;

//用于右键菜单参数传递的指针
QModelIndex* gIndex;

uint64_t gPcStartAddr;
uint64_t gHexData[5];

bool gSign_outFileImport = false;
bool gSign_csvFileImport = false;


void treeViewInit(MainWindow* window);
void toolBottonInit(MainWindow* window);
int sourceFileOpen(QString filePath);
void outFileSort(QString filePath);
//void treeItemAdd(MainWindow* window,cfgTemp mCfgTemp,int num,QString currenfCfgName);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    gWindow = this;
    gDefaultFilePath = QCoreApplication::applicationDirPath();
    gCurrentCfg.coreCount = 0;
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/img/icon.png"));
    this->model = new QStandardItemModel(this);
    //this->delagate = new MyEditDelagate(this->ui->treeView);
    gModel = this->model;

    ui->textBrowser_resShower->setReadOnly(false);
    ui->textBrowser_resShower->setTextInteractionFlags(Qt::TextEditorInteraction);
    //初始化树状视图，用于显示配置列表
    treeViewInit(this);
    //转换按键初始化
    toolBottonInit(this);
    //log文件加载
    gLogFileName = gDefaultFilePath;
    gLogFileName.append("/log.ini");
    logFileRead(this,gLogFileName);
    //导入cfg文件
    connect(ui-> action_newConfig,&QAction::triggered,this,&MainWindow::action_newConfig_clicked);
    connect(ui->action_importConfig,&QAction::triggered,this,&MainWindow::action_importConfig_clicked);
    connect(ui->action_refreshConfigList,&QAction::triggered,this,&MainWindow::action_refreshConfigList_clicked);
    connect(ui->action_displayCurrentCfg,&QAction::triggered,this,&MainWindow::action_displayCurrentCfg_clicked);

    connect(ui->action_openOut,&QAction::triggered,this,&MainWindow::action_openOut_clicked);
    connect(ui->action_openExcel,&QAction::triggered,this,&MainWindow::action_openExcel_clicked);
    connect(ui->action_openHex,&QAction::triggered,this,&MainWindow::action_openHex_clicked);

    connect(ui->action_fileConvert,&QAction::triggered,this,&MainWindow::action_fileConvert_clicked);
    connect(ui->action_saveFileAs,&QAction::triggered,this,&MainWindow::action_saveFileAs_clicked);
    connect(ui->action_emuFileConvert,&QAction::triggered,this,&MainWindow::action_emuFileConvert_clicked);
    connect(ui->action_ateFileConvert,&QAction::triggered,this,&MainWindow::action_ateFileConvert_clicked);
    connect(ui->action_testFileConvert,&QAction::triggered,this,&MainWindow::action_testFileConvert_clicked);
    connect(ui->action_showCfgMethod,&QAction::triggered,this,&MainWindow::action_showCfgMethod_clicked);
    connect(ui->action_saveSpace,&QAction::triggered,this,&MainWindow::action_saveSpace_clicked);
    connect(ui->action_switchSpace,&QAction::triggered,this,&MainWindow::action_switchSpace_clicked);

    //选中item信号处理
    //    connect(ui->treeView->selectionModel(),&QItemSelectionModel::currentChanged,this,&MainWindow::slotCurrentChanged);
    //item内容修改信号
    connect(this->model,&QStandardItemModel::itemChanged,this,&MainWindow::treeItemChanged);
    //treeView右键菜单栏信号
    connect(this->ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::slotTreeMenu);
    //logShower右键菜单栏信号
    connect(this->ui->textBrowser_logShower, &QTextBrowser::customContextMenuRequested, this, &MainWindow::slotTextMenu);

    this->ui->treeView->expandsOnDoubleClick();
}
void changeConvertBottomText(generateCodeFormate codeFormate)
{
    switch(codeFormate)
    {
    case ATE_CODE:gWindow->ui->toolButton_fileConvert->setText("机测码转换");break;
    case TEST_CODE:gWindow->ui->toolButton_fileConvert->setText("实装码转换");break;
    case EMU_CODE:gWindow->ui->toolButton_fileConvert->setText("仿真码转换");break;
    case EMPTY_CODE:gCurrentCfg.codeFormate = ATE_CODE;gWindow->ui->toolButton_fileConvert->setText("机测码转换");break;
    }
}
void toolBottonInit(MainWindow* window)
{
    QMenu *switchMenu = new QMenu(window);
    switchMenu->addAction(window->ui->action_enableAteConvert);
    switchMenu->addAction(window->ui->action_enableTestConvert);
    switchMenu->addAction(window->ui->action_enableEmuConvert);
    window->ui->toolButton_fileConvert->setMenu(switchMenu);
}
void checkStateInit(cfgTemp* lCfgTemp,QStandardItem * item)
{
    item->child(lCfgTemp->currentCore,0)->setCheckState(Qt::Checked);
}

void displayCfg(cfgTemp* lCfgTemp)
{
    gWindow->ui->textBrowser_resShower->clear();
    gWindow->ui->textBrowser_resShower->append("[MAIN]");
    QString temp = QString("配置文件名称：%1").arg(lCfgTemp->cfgName);
    gWindow->ui->textBrowser_resShower->append(temp);
    temp = QString("指令库的路径：%1").arg(lCfgTemp->libPath);
    gWindow->ui->textBrowser_resShower->append(temp);
    gWindow->ui->textBrowser_resShower->append("[CFG]");
    temp = QString("内核数量：%1").arg(lCfgTemp->coreCount);
    gWindow->ui->textBrowser_resShower->append(temp);

    temp = QString("当前内核：%1").arg(lCfgTemp->coreCfg[lCfgTemp->currentCore].coreName);
    gWindow->ui->textBrowser_resShower->append(temp);
    if(lCfgTemp->coreCfg[lCfgTemp->currentCore].withIcepick)
        temp = QString("是否有ICEPick：true");
    else
        temp = QString("是否有ICEPick：false");
    gWindow->ui->textBrowser_resShower->append(temp);
    if(lCfgTemp->coreCfg[lCfgTemp->currentCore].useDma)
        temp = QString("是否使用DMA下载程序：true");
    else
        temp = QString("是否使用DMA下载程序：false");
    gWindow->ui->textBrowser_resShower->append(temp);

    gWindow->ui->textBrowser_resShower->append(temp);
    temp = QString("内核指令长度：%1 bit").arg(lCfgTemp->coreCfg[lCfgTemp->currentCore].coreCmdLength);
    gWindow->ui->textBrowser_resShower->append(temp);
    temp = QString("一条DMA指令所含数据长度：%1 bit").arg(lCfgTemp->coreCfg[lCfgTemp->currentCore].dmaDataLength);
    gWindow->ui->textBrowser_resShower->append(temp);
    temp = QString("地址模式：%1").arg(lCfgTemp->coreCfg[lCfgTemp->currentCore].ramAddressMode);
    gWindow->ui->textBrowser_resShower->append(temp);

    temp = QString("解析器名称：%1").arg(lCfgTemp->coreCfg[lCfgTemp->currentCore].parserName);
    gWindow->ui->textBrowser_resShower->append(temp);

    gWindow->ui->textBrowser_resShower->append("[CMD]");
    QString lTemp = QString::number(lCfgTemp->coreCfg[lCfgTemp->currentCore].dmaCmdHead,16);
    QByteArray ba = lTemp.toLatin1(); // must
    char* ch = ba.data();
    temp = QString("DMA指令头：0x%1").arg(ch);
    gWindow->ui->textBrowser_resShower->append(temp);
    lTemp = QString::number(lCfgTemp->coreCfg[lCfgTemp->currentCore].regWriteAddrHead,16);
    ba = lTemp.toLatin1();
    ch = ba.data();
    temp = QString("写ram地址指令：0x%1").arg(ch);
    gWindow->ui->textBrowser_resShower->append(temp);
    lTemp = QString::number(lCfgTemp->coreCfg[lCfgTemp->currentCore].regWriteDataHead,16);
    ba = lTemp.toLatin1();
    ch = ba.data();
    temp = QString("写ram数据指令：0x%1").arg(ch);
    gWindow->ui->textBrowser_resShower->append(temp);

    gWindow->ui->textBrowser_resShower->append("[VAR]");
    temp = QString("指令间空clock数：%1").arg(lCfgTemp->coreCfg[lCfgTemp->currentCore].jtagEmptyClockBetweenCmd);
    gWindow->ui->textBrowser_resShower->append(temp);
    temp = QString("jtag频率的乘数：%1").arg(lCfgTemp->coreCfg[lCfgTemp->currentCore].jtagFreqMult);
    gWindow->ui->textBrowser_resShower->append(temp);

}
void clearAllCheck()
{
    for(int i=0;i<gItemNum;i++)
    {
        gModel->item(i,0)->setIcon(QIcon(":/img/notChecked.png"));
    }
}
void checkStateChange(cfgTemp* lCfgTemp,QStandardItem * item)
{
    gWindow->disconnect(gWindow->model, &QStandardItemModel::itemChanged, gWindow, &MainWindow::treeItemChanged);
    //    Qt::CheckState state = item->checkState();//获取当前的选择状态
    QString tabName = item->text();

    for(uint32_t i = 0;i<lCfgTemp->coreCount;i++)
    {
        if(tabName.compare(lCfgTemp->coreCfg[i].coreName, Qt::CaseInsensitive) == 0)
        {
            lCfgTemp->currentCore = i;
            changeConvertBottomText(lCfgTemp->codeFormate);
            gWindow->ui->textBrowser_logShower->append("enable core:"+lCfgTemp->coreCfg[lCfgTemp->currentCore].coreName);
        }
        else
            item->parent()->child(i,0)->setCheckState(Qt::Unchecked);
    }

    if(gCurrentCfg.cfgName.compare(lCfgTemp->cfgName,Qt::CaseInsensitive) == 0)
        gCurrentCfg = *lCfgTemp;
    gWindow->connect(gWindow->model,&QStandardItemModel::itemChanged,gWindow,&MainWindow::treeItemChanged);

}
void treeViewInit(MainWindow* window)
{
    //window->ui->treeView->setItemDelegateForRow(0, window->delagate);

    //设置model
    window->ui->treeView->setModel(window->model);
    //设置表头
    //    window->model->setHorizontalHeaderLabels(QStringList()<<"参数"<<"配置");
    window->model->setHorizontalHeaderLabels(QStringList()<<"配置列表");
    window->ui->treeView->header()->setDefaultAlignment(Qt::AlignCenter);//居中

    //    window->ui->treeView->setEditTriggers(QTreeView::NoEditTriggers); /* 单元格不能编辑 */
    window->ui->treeView->setSelectionBehavior(QTreeView::SelectRows); /* 一次选中整行 */
    window->ui->treeView->setSelectionMode(QTreeView::SingleSelection); /* 单选，配合上面的整行就是一次选单行 */
    //window->ui->treeView->setAlternatingRowColors(true); /* 每间隔一行颜色不一样，当有qss时该属性无效 */
    window->ui->treeView->setFocusPolicy(Qt::NoFocus); /* 去掉鼠标移到单元格上时的虚线框 */
    window->ui->treeView->setEditTriggers(QTreeView::DoubleClicked);			//单元格双击编辑
    window->ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    //    window->ui->treeView->setColumnWidth(0,160);
    //    window->ui->treeView->setColumnWidth(1,30);

}
void treeItemAdd(MainWindow* window,cfgTemp mCfgTemp,int num,QString currenfCfgName)
{
    window->item = new QStandardItem(mCfgTemp.cfgName);
    window->disconnect(window->model, &QStandardItemModel::itemChanged, window, &MainWindow::treeItemChanged);
    window->model->setItem(num,0,window->item);
    //    window->item0[0] = new QStandardItem("core");
    //    window->item0[1] = new QStandardItem("-----------------------------");

    if(mCfgTemp.coreCount == 0)
    {
        window->ui->textBrowser_logShower->append("无效的配置文件！");
        return;
    }
    for(uint32_t i = 0;i<mCfgTemp.coreCount;i++)
    {
        window->item0[i] = new QStandardItem(mCfgTemp.coreCfg[i].coreName);
        window->item0[i]->setCheckable(true);
        window->model->item(num,0)->setChild(i,0,window->item0[i]);
    }

    if(currenfCfgName.compare(mCfgTemp.cfgName,Qt::CaseInsensitive) == 0)
    {
        window->item ->setIcon(QIcon(":/img/checked.png"));
        window->ui->treeView->expand(window->model->item(num,0)->index());
    }
    else
        window->item ->setIcon(QIcon(":/img/notChecked.png"));

    checkStateInit(&mCfgTemp,window->model->item(num,0));

    window->connect(window->model,&QStandardItemModel::itemChanged,window,&MainWindow::treeItemChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::action_importConfig_clicked()
{
    cfgTemp mCfgTemp;
    int returnValue=0;

    QFileDialog *pFileName = new QFileDialog(this, Qt::Dialog);
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";
    pFileName->setDirectory(gStrFileName);//设置打开的默认目录
    pFileName->setNameFilter(tr("ini(*.ini)"));//设置限定文件类型
    pFileName->setOption(QFileDialog::DontResolveSymlinks);//可以加载链接路径而不会跳转

    if (pFileName->exec()!=QFileDialog::Accepted)
    {
        return;
    }
    //存储选中文件路径
    gStrFileName = pFileName->selectedFiles().first();

    returnValue = cfgFileRead(gStrFileName,mCfgTemp);
    if(returnValue==-2)
    {
        gWindow->ui->textBrowser_logShower->append("配置文件中的libPath或核配置文件的路径不存在！");
        return;
    }
    else if(returnValue==-3)
    {
        gWindow->ui->textBrowser_logShower->append("无效的配置文件！");
        return;
    }
    cfgNameFilter(&mCfgTemp.cfgName);
    gCfgList.append(&mCfgTemp);
    treeItemAdd(this,mCfgTemp,gItemNum,"NULL");
    logFileAppend(mCfgTemp,gStrFileName);
    gItemNum++;
}
void MainWindow::action_refreshConfigList_clicked()
{
    for(int i =0;i<gItemNum;i++)
    {
        QModelIndex indexTemp = gModel->indexFromItem(gModel->item(i,0));
        gModel->removeRow(indexTemp.row());
    }
    gItemNum = 0;
    gCfgList.removeAll();

    logFileRead(this,gLogFileName);
    this->ui->treeView->setColumnWidth(0,160);
    this->ui->treeView->setColumnWidth(1,30);
    this->ui->textBrowser_logShower->append("刷新完成！");
}
void MainWindow::action_displayCurrentCfg_clicked()
{
    if(gCurrentCfg.coreCount == 0)
        gWindow->ui->textBrowser_logShower->append("当前未选中配置文件！");
    else
        displayCfg(&gCurrentCfg);
}
void MainWindow::action_newConfig_clicked()
{
    QString tempFilePath = gDefaultFilePath;
    tempFilePath.append("/lib/cfgTemp.ini");
    QString resFilePath = gDefaultFilePath;
    resFilePath.append("/newCfg.ini");
    QFile inCfgFile(tempFilePath);
    if(!inCfgFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        gWindow->ui->textBrowser_logShower->append("文件打开失败");
        return;
    }
    QFile outCfgFile(resFilePath);
    if(!outCfgFile.open(QIODevice::WriteOnly|QIODevice::Text)){
        gWindow->ui->textBrowser_logShower->append("文件创建失败");
        return;
    }
    QTextStream in(&inCfgFile);
    QTextStream out(&outCfgFile);
    QString line;
    while(in.readLineInto(&line))
    {
        line.append("\n");
        out<<line;
    }
    inCfgFile.close();
    outCfgFile.close();

    resFilePath.prepend("notepad ");

    executeCommandNotWait(resFilePath);

}
//void MainWindow::slotCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
//{
//    QStandardItem* item = localModel->itemFromIndex(current);
//    if (item)
//    {
//        QString text = item->text();
//        QString data1=item->data(Qt::UserRole + 1).toString();
//        bool checked = item->checkState();
//        localWindow->ui->textBrowser_logShower->append("slotCurrentChanged triggered");
//    }
//}


void MainWindow::treeItemChanged(QStandardItem * item)
{
    if(item == nullptr)
        return;
    QString itemName = item->parent()->text();//得到配置名

    cfgTemp* lCfgTemp = matchCfg(itemName);
    if(lCfgTemp)//返回值非空
    {
        //如果条目是存在选框的，那么就进行下面的操作
        checkStateChange(lCfgTemp,item);
    }
    else {
        gWindow->ui->textBrowser_logShower->append("can not find matching configer file!");
    }
}
void MainWindow::slotTreeMenu(const QPoint &pos)
{
    QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
            QMenu::item{padding:3px 20px 3px 20px;}\
            QMenu::indicator{width:13px;height:13px;}\
            QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
            QMenu::separator{height:1px;background:#757575;}";

            QMenu menu;
    menu.setStyleSheet(qss);    //可以给菜单设置样式

    QModelIndex curIndex = ui->treeView->indexAt(pos);      //当前点击的元素的index
    QModelIndex index = curIndex.sibling(curIndex.row(),0); //该行的第1列元素的index
    gIndex = &index;
    if (index.isValid())
    {
        //添加一行菜单，进行展开
        menu.addAction(QStringLiteral("启用"), this, SLOT(slotTreeMenuCfgEnable()));
        menu.addSeparator();    //添加一个分隔线
        menu.addAction(QStringLiteral("删除"), this, SLOT(slotTreeMenuCfgDelete()));
        menu.addSeparator();    //添加一个分隔线
        menu.addAction(QStringLiteral("显示配置参数"), this, SLOT(slotTreeMenuCfgShow()));
    }
    else
    {
        menu.addAction(QStringLiteral("导入cfg文件"), this, SLOT(action_importConfig_clicked()));
        menu.addSeparator();    //添加一个分隔线
        menu.addAction(QStringLiteral("刷新cfg列表"), this, SLOT(action_refreshConfigList_clicked()));
    }
    menu.exec(QCursor::pos());  //显示菜单
}
void MainWindow::logShowerClear()
{
    gWindow->ui->textBrowser_logShower->clear();
}
void MainWindow::slotTextMenu(const QPoint &pos)
{
    QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
            QMenu::item{padding:3px 20px 3px 20px;}\
            QMenu::indicator{width:15px;height:15px;}\
            QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
            QMenu::separator{height:1px;background:#757575;}";
            QMenu menu;
    menu.setStyleSheet(qss);    //可以给菜单设置样式

    menu.addAction(QStringLiteral("清空"), this, SLOT(logShowerClear()));
    menu.addSeparator();    //添加一个分隔线

    menu.exec(QCursor::pos());  //显示菜单
}
void MainWindow::slotTreeMenuCfgEnable()
{
    //获取元素的文本、data,进行其他判断处理
    QStandardItem* item = gModel->itemFromIndex(*gIndex);
    //先将修改item会触发的槽函数断开
    gWindow->disconnect(gWindow->model, &QStandardItemModel::itemChanged, gWindow, &MainWindow::treeItemChanged);
    QString itemName = item->text();
    clearAllCheck();
    if(matchCfg(itemName)==nullptr)//未选中cfg名
    {
        gWindow->ui->textBrowser_logShower->append("请选中配置名后再启用！");
        return;
    }
    else
    {
        item->setIcon(QIcon(":/img/checked.png"));
    }
    gWindow->connect(gWindow->model,&QStandardItemModel::itemChanged,gWindow,&MainWindow::treeItemChanged);
    gCurrentCfg = * matchCfg(itemName);//修改当前所用配置
    changeConvertBottomText(gCurrentCfg.codeFormate);
    logFileEnableCfg(gCurrentCfg);

    gWindow->ui->textBrowser_logShower->append("enable config:");
    gWindow->ui->textBrowser_logShower->append(itemName);
}
void MainWindow::slotTreeMenuCfgDelete()
{
    //获取元素的文本、data,进行其他判断处理
    QModelIndex indexTemp = *gIndex;
    QStandardItem* item = gModel->itemFromIndex(*gIndex);
    gWindow->disconnect(gWindow->model, &QStandardItemModel::itemChanged, gWindow, &MainWindow::treeItemChanged);
    QString itemName = item->text();
    if(matchCfg(itemName)==nullptr)
    {
        gWindow->ui->textBrowser_logShower->append("请选中cfg name一栏后再右键删除！");
        return;
    }
    //删除树状图中的节点
    gWindow->model->removeRow(indexTemp.row());
    if(gCurrentCfg.cfgName == itemName)//删除的是当前正在用的配置
        gCurrentCfg.coreCount = 0;//将核的数量清为0，表明当前配置不可用
    //将删除的配置从链表中删除
    delCfgFromList(itemName);

    logFileDeleteCfg(itemName);
    gWindow->connect(gWindow->model,&QStandardItemModel::itemChanged,gWindow,&MainWindow::treeItemChanged);
}
//显示配置的详细信息
void MainWindow::slotTreeMenuCfgShow()
{
    QStandardItem* item = gModel->itemFromIndex(*gIndex);
    QString itemName = item->text();
    if(matchCfg(itemName) == 0)
    {
        gWindow->ui->textBrowser_logShower->append("请选中cfg name一栏后再右键操作！");
        return;
    }
    cfgTemp* lCfgTemp = matchCfg(itemName);
    displayCfg(lCfgTemp);
}

unsigned int getPcFromCoff(MainWindow *window,QString coffFileName)
{
    unsigned int pcOffSet = 0,pcAddr = 0,temp;
    int i;
    QFile coffFile(coffFileName);
    if(!coffFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        window->ui->textBrowser_logShower->append("coff文件打开失败！");
    }
    pcOffSet = gCurrentCfg.coreCfg[gCurrentCfg.currentCore].pcOffsetInCoff;
    coffFile.seek(pcOffSet);
    QByteArray arrTemp;
    bool ok;
    for(i = 0;i<4;i++)
    {
        arrTemp = coffFile.read(1);
        temp = arrTemp.toHex().toUInt(&ok,16);
        pcAddr += (temp<<(8*i));
    }

    return pcAddr;
}
int outFileOpen(QString strFileName)
{
    //存储命令
    QString toolCmd;
    bool elfFileSign = true;
    //将文件路径显示
    gWindow->ui->textBrowser_logShower->append("out读取中...");
    QApplication::processEvents();

    if (strFileName.isEmpty())
    {
        gWindow->ui->textBrowser_logShower->append("未选中文件！");
        return -1;
    }
    else
    {
        bool ok = false;

        elfFileSign = checkElfFormate(strFileName);

        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].parserName.compare("default")!=0)//自定义解析
        {
            QString toolPath = gCurrentCfg.coreCfg[gCurrentCfg.currentCore].coreLibPath;
            toolPath.append(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].parserName);
            toolCmdGenerate(toolPath,strFileName,&toolCmd,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].parserSrcCmd,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].parserDestCmd);
            executeCommandWait(toolCmd);
        }
        else if(elfFileSign)//elf文件默认解析
        {
            switch(elfToHex(strFileName,gDefaultFilePath,&gPcStartAddr))
            {
            case 0:gWindow->ui->textBrowser_logShower->append("转换完成！");break;
            case 1:gWindow->ui->textBrowser_logShower->append("找不到源文件！");return -1;
            case 2:gWindow->ui->textBrowser_logShower->append("无法打开文件！");return -1;
            }
        }
        else//coff文件默认解析
        {
            switch(coffToHex(strFileName,gDefaultFilePath))
            {
            case 0:gWindow->ui->textBrowser_logShower->append("转换完成！");break;
            case 1:gWindow->ui->textBrowser_logShower->append("找不到源文件！");return -1;
            case 2:gWindow->ui->textBrowser_logShower->append("无法打开文件！");return -1;
            case 3:gWindow->ui->textBrowser_logShower->append("源文件非coff格式！");return -1;
            }
        }

        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].pcFromMap == false)
            goto noMap;
        strFileName.chop(4);
        strFileName.append(".map");
        QFile mapFile(strFileName);
        if(!mapFile.open(QIODevice::ReadOnly|QIODevice::Text)){
            gWindow->ui->textBrowser_logShower->append("未找到同名map文件！");
            goto noMap;
        }
        QTextStream in(&mapFile);
        QString line;
        while(in.readLineInto(&line))
        {
            if(line.contains("address:"))
            {
                int index = line.lastIndexOf(':',-1);
                QString temp = line.right(line.length()-index-2);//windows的换行符
                gPcStartAddr = temp.toUInt(&ok,16);
                gWindow->ui->textBrowser_logShower->append("读取完成!");
                temp.prepend("PC起始地址:0x");
                gWindow->ui->textBrowser_logShower->append(temp);
                break;
            }
        }
        mapFile.close();
    }
    if(0)
    {
noMap:
        strFileName.chop(4);
        strFileName.append(".out");
        if(elfFileSign)
            gPcStartAddr = getPcFromElf(strFileName);
        else
            gPcStartAddr = getPcFromCoff(gWindow,strFileName);
        QString temp = QString::number(gPcStartAddr,16);
        gWindow->ui->textBrowser_logShower->append("从out中读取pc地址");
        temp.prepend("PC起始地址:0x");
        gWindow->ui->textBrowser_logShower->append(temp);
        gWindow->ui->textBrowser_logShower->append("读取完成!");
    }
    gSign_outFileImport = true;//已选中out文件
    gSign_csvFileImport = false;
    for(Uint32 i =0;i<gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeqCount;i++)
    {
        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[i].generalReplaceData.compare("PC",Qt::CaseInsensitive) == 0)
        {
            gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[i].data = gPcStartAddr;
            break;
        }
    }
    return 0;
}
void MainWindow::action_openOut_clicked()
{
    if(gCurrentCfg.coreCount == 0)
    {
        gWindow->ui->textBrowser_logShower->append("请先启用配置文件！");
        return;
    }
    QFileDialog *pFileName = new QFileDialog(this, Qt::Dialog);
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";
    pFileName->setDirectory(gStrFileName);//设置打开的默认目录
    pFileName->setNameFilter(tr("out(*.out);;coff(*.coff)"));//设置限定文件类型
    pFileName->setOption(QFileDialog::DontResolveSymlinks);//可以加载链接路径而不会跳转

    if (pFileName->exec()!=QFileDialog::Accepted)
    {
        return;
    }

    //存储选中文件路径
    gStrFileName = pFileName->selectedFiles().first();
    gChoosedFileName = gStrFileName;
    delete pFileName;
    outFileOpen(gStrFileName);
}
int  excelCmdLoad(QString filePath)
{
    //如果链表不为空，清空链表
    if(gJtagCmdList)
    {
        gJtagCmdList->removeAll();
    }
    //打开excel
    ExcelBase *excelHandle = new ExcelBase();
    excelHandle->excel = new QAxObject(gWindow);
    if (excelHandle->excel->setControl("Excel.Application"))//连接Excel控件
    {}
    else
    {
        excelHandle->excel->setControl("ket.Application");  //连接WPS控件
    }
    excelHandle->excel->setProperty("Visible", false);  //不显示窗体
    excelHandle->workBooks = excelHandle->excel->querySubObject("WorkBooks");  //获取工作簿集合
    excelHandle->workBooks->dynamicCall("Open(const QString&)", filePath); //打开打开已存在的工作簿
    excelHandle->workBook = excelHandle->excel->querySubObject("ActiveWorkBook"); //获取当前工作簿
    excelHandle->workSheet = excelHandle->workBook->querySubObject("WorkSheets(int)", 1);//获取工作表集合的工作表1，即sheet1


    /********处理打开的文件*******/
    QVariant values;//用于读取单元格的值
    QList<QList<QVariant>> excelData;//存储整个excel表

    values = excelHandle->readAll();
    excelHandle->castVariant2ListListVariant(values,excelData);
    int length = excelData.length();
    if(length <= 1)
    {
        QMessageBox::information(gWindow,"Error Message", "Empty Excel File Selected");
        return -1;
    }

    gWindow->ui->textBrowser_logShower->append("excel读取完成！\n数据处理中...");
    QApplication::processEvents();

    gJtagCmdList = new JtagList();//创建jtag命令链表
    gLongCmdList = new LongCmdList();
    switch(cmdReadIn(excelData,length,gJtagCmdList,gLongCmdList))
    {
    case 0 :
        gWindow->ui->textBrowser_logShower->append("文件导入完成！");
        break;
    case -1:
        gWindow->ui->textBrowser_logShower->append("所选文件中无bitCount栏！");
        return -1;
    }

    gSign_csvFileImport = true;
    gSign_outFileImport = false;
    if(gLongCmdList->head == gLongCmdList->tail)
    {
        delete gLongCmdList;
        gLongCmdList = nullptr;
    }

    excelHandle->workBooks->dynamicCall("Close(Boolean)",false);
    excelHandle->excel->dynamicCall("Quit(void)");
    delete excelHandle->excel;
    excelHandle->excel = nullptr;
    delete excelHandle;//删除excel对象
    excelHandle = nullptr;
    return 0;
}
int csvCmdLoad(QString filePath)
{
    //如果链表不为空，清空链表
    if(gJtagCmdList)
    {
        gJtagCmdList->removeAll();
    }
    QList<QStringList> data;
    csvRead(filePath,data);
    gWindow->ui->textBrowser_logShower->append("csv读取完成！\n数据处理中...");
    QApplication::processEvents();
    gJtagCmdList = new JtagList();//创建jtag命令链表
    gLongCmdList = new LongCmdList();
    switch(csvCmdReadIn(data,data.length(),gJtagCmdList,gLongCmdList))
    {
    case 0 :
        gWindow->ui->textBrowser_logShower->append("文件导入完成！");
        break;
    case -1:
        gWindow->ui->textBrowser_logShower->append("所选文件中无bitCount栏！");
        return -1;
    }

    gSign_csvFileImport = true;
    gSign_outFileImport = false;
    if(gLongCmdList->head == gLongCmdList->tail)
    {
        delete gLongCmdList;
        gLongCmdList = nullptr;
    }
    return 0;
}
int txtFileLoad(QString filePath)
{
    gWindow->ui->textBrowser_logShower->append("txt读取中...");
    QApplication::processEvents();
    QString srcPath = filePath;
    QString tarPath = gDefaultFilePath+"/out.txt";
    tarPath.replace("/","\\");
    srcPath.replace("/","\\");
    QString winCmd = QString("copy %1 %2 /y").arg(srcPath).arg(tarPath);
    executeCommandWait(winCmd);

    QFile outFile(tarPath);
    if(!outFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        gWindow->ui->textBrowser_logShower->append("txt文件拷贝失败！");
        return -1;
    }
    QString pcSign = gCurrentCfg.coreCfg[gCurrentCfg.currentCore].pcInTxt;
    if(pcSign.isEmpty())
    {
        gWindow->ui->textBrowser_logShower->append("未配置txt文件中PC的位置，PC将置为0！");
        gPcStartAddr = 0;
    }
    else
    {
        QTextStream out(&outFile);
        QString line;
        bool signGetPC = false;
        while(out.readLineInto(&line))
            if(line.contains(pcSign))
            {
                gPcStartAddr=0;
                uint32_t byteCount = 4;
                QString dataTemp;
                bool ok;
                out.readLineInto(&line);//获取下一行
                if(line.length()<11)//4*3,默认最大取4字节（32位）
                    byteCount = (line.length()+1)/3;

                for(uint32_t i=0;i<byteCount;i++)
                {
                    dataTemp = line.left(2);
                    line.remove(0,3);
                    if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].dataEndian == LITTLE_ENDIAN)//小端
                        gPcStartAddr = gPcStartAddr + (dataTemp.toUInt(&ok,16)<<(i*8));
                    else//大端
                        gPcStartAddr = (gPcStartAddr<<8) + dataTemp.toUInt(&ok,16);
                }
                signGetPC = true;
                break;
            }

        if(signGetPC)
        {
            gWindow->ui->textBrowser_logShower->append("读取到PC为:0x"+QString::number(gPcStartAddr,16));
        }
        else
            gWindow->ui->textBrowser_logShower->append("在txt文件中未找到PC段的信息！");
    }

    outFile.close();
    gSign_outFileImport = true;
    gSign_csvFileImport = false;
    gWindow->ui->textBrowser_logShower->append("读取完成！");
    return 0;
}
int hexFileLoad(QString filePath)
{
    gWindow->ui->textBrowser_logShower->append("hex读取中...");
    QApplication::processEvents();

    QFile hexSrcFile(filePath);
    if(!hexSrcFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        gWindow->ui->textBrowser_logShower->append("hex文件无法读取");
        return -1;
    }
    QString outPath = gDefaultFilePath+"/out.txt";
    QFile outFile(outPath);
    outFile.open(QIODevice::WriteOnly|QIODevice::Text);

    QTextStream in(&hexSrcFile);
    QTextStream out(&outFile);
    QString line;
    stHexLineData hexLineData;
    //第一行数据直接下发
    if(in.readLineInto(&line))
    {
        getHexLineData(line,&hexLineData);
        lineDataIntoOut(hexLineData,&out);
    }
    //第二行数据含有相关信息，进行处理
    if(in.readLineInto(&line))
    {
        getHexLineData(line,&hexLineData);
        for(uint32_t i=0;i<gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeqCount;i++)//提取信息
        {
            for(uint32_t j =0;j<5;j++)
            {
                gHexData[j] = 0;
                if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[i].generalReplaceData.compare(gGeneralReplaceDataSign[j],Qt::CaseInsensitive) == 0)//找到需要读取的信息
                {
                    for(uint32_t k=0;k<gCurrentCfg.coreCfg[gCurrentCfg.currentCore].dataByteCountInHex[j];k++)
                        gHexData[j] += (hexLineData.data[gCurrentCfg.coreCfg[gCurrentCfg.currentCore].dataOffsetInHex[j]+k]<<k*8);
                    gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[i].data = gHexData[j];
                }
            }
        }

        lineDataIntoOut(hexLineData,&out);
    }
    while(in.readLineInto(&line))
    {
        getHexLineData(line,&hexLineData);
        lineDataIntoOut(hexLineData,&out);
    }
    hexSrcFile.close();
    outFile.flush();
    outFile.close();
    gSign_outFileImport = true;//已选中out文件
    gSign_csvFileImport = false;
    gWindow->ui->textBrowser_logShower->append("读取完成！");
    return 0;
}
void MainWindow::action_openExcel_clicked()
{
    /*excel处理模块中的导入excel文件操作*/
    QFileDialog *pFileName = new QFileDialog(this, Qt::Dialog);
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";
    pFileName->setDirectory(gStrFileName);//设置打开的默认目录
    pFileName->setNameFilter(tr("excel(*.csv *.xls *.xlsx)"));//设置限定文件类型
    pFileName->setOption(QFileDialog::DontResolveSymlinks);//可以加载链接路径而不会跳转

    if (pFileName->exec()!=QFileDialog::Accepted)
    {
        return;
    }
    gStrFileName = pFileName->selectedFiles().first();
    gChoosedFileName = gStrFileName;
    ui->textBrowser_logShower->append("excel读取中...");
    QApplication::processEvents();
    delete pFileName;

    if (gStrFileName.isEmpty())
    {
        QMessageBox::information(this,"Warning", "未选中文件！");
        return;
    }

    if(gStrFileName.contains(".csv"))
        csvCmdLoad(gStrFileName);
    else
        excelCmdLoad(gStrFileName);

}
void MainWindow::action_openHex_clicked()
{
    /*excel处理模块中的导入excel文件操作*/
    QFileDialog *pFileName = new QFileDialog(this, Qt::Dialog);
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";
    pFileName->setDirectory(gStrFileName);//设置打开的默认目录
    pFileName->setNameFilter(tr("excel(*.hex)"));//设置限定文件类型
    pFileName->setOption(QFileDialog::DontResolveSymlinks);//可以加载链接路径而不会跳转

    if (pFileName->exec()!=QFileDialog::Accepted)
    {
        return;
    }
    gStrFileName = pFileName->selectedFiles().first();
    gChoosedFileName = gStrFileName;
    ui->textBrowser_logShower->append("hex读取中...");
    QApplication::processEvents();
    delete pFileName;

    if (gStrFileName.isEmpty())
    {
        QMessageBox::information(this,"Warning", "未选中文件！");
        return;
    }
    hexFileLoad(gStrFileName);

}
int dataConvertForTest(QFile* srcFile,QFile* outFile,QFile* outDataFile,QFile* outAddrFile)
{
    QTextStream in(srcFile);
    QTextStream outData(outDataFile);
    QTextStream outAddr(outAddrFile);
    QString line,strTemp;
    bool signBypass = false;
    bool ok;
    uint32_t count = 0,localTemp = 0,local = 0,lineLength,temp,div;
    int addrDiff = 0,addrRecord = 0;
    switch(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].ramAddressMode)
    {
    case ram8bitMode:div=1;break;
    case ram16bitMode:div=2;break;
    case ram32bitMode:div=4;break;
    case ram64bitMode:div=8;break;
    }
    while(in.readLineInto(&line))
    {
        //处理line中的数据
        if(line.contains("@"))//文件中的地址部分
        {
            if(line.compare(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].pcInTxt,Qt::CaseInsensitive) == 0)
            {
                signBypass = true;
                continue;
            }
            else
                signBypass = false;

            //将地址数据存储在地址数组中
            line.replace("@","0x");
            temp = line.toUInt(&ok,16);

            if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].parserName.compare("default")!=0)//default时不需要管地址位宽
                temp/=div;
            addrDiff = temp-(addrRecord+count/div);
            if(addrDiff<0)
            {
                gWindow->ui->textBrowser_logShower->append("地址有重叠！");
                addrDiff = 0;
            }
            if((count != 0)&&(addrDiff>gCurrentCfg.coreCfg[gCurrentCfg.currentCore].sectionAddrDiffInByte))
            {
                strTemp = QString("%1, %2},\n").arg(count).arg(localTemp);
                outAddr<<strTemp;
                count = 0;
                addrRecord = temp;
            }
            else if(count != 0)//需要填0
            {
                QString temp="";
                if(addrDiff  != 0)
                {
                    for(int i = 0;i<addrDiff;i++)
                    {
                        temp.append("0x00,");
                        count++;local++;
                    }
                    temp.append("\n");
                    outData << temp;//先输出到jtagData.h
                }
                continue;
            }
            else//首次地址
                addrRecord = temp;

            localTemp = local;
            line = QString::number(temp,16);
            line.prepend("{0x");
            line.append(", ");
            outAddr << line;
        }
        else if(line.contains("q"))//文件结束
        {
            strTemp = QString("%1, %2}\n").arg(count).arg(localTemp);
            outAddr<<strTemp;
            count = 0;
            outData.seek(0);
            outAddr.seek(0);
        }
        else//数据部分
        {
            if(signBypass)
                continue;
            lineLength = line.length();
            if(lineLength%3 == 2 )
            {
                lineLength += 1;
                line.append(" ");
            }
            count += (lineLength/3);//以8bit计
            local += (lineLength/3);//以8bit计
            line.replace(" ",",0x");
            line.prepend("0x");
            line.chop(2);
            line.append("\n");
            outData << line;//先输出到jtagData.h
        }
    }
    if(outFileGenerate(outFile,outDataFile,outAddrFile,gPcStartAddr,gCurrentCfg)==-1)
    {
        gWindow->ui->textBrowser_logShower->append("请确认lib中包含所设头文件！");
        return -2;
    }
    return 0;
}

void addrReplace(JtagList* localCmdList,LongCmdList* longCmdList,JtagList* tempList,LongCmdList* tempLongList,uint32_t* replaceAddr,uint8_t sign,cfgTemp localCfg,generalReplace localGeneralReplace)
{
    *tempList = *localCmdList;
    *tempLongList = *longCmdList;
    uint32_t replaceCount;
    QString addrReplaceSign;
    uint32_t replaceBitLocal[5];
    uint32_t replaceBitCount[5];

    if(sign == DMA_ADDR_REPLACE)
    {
        replaceCount = localCfg.coreCfg[gCurrentCfg.currentCore].dmaReplaceCount;
        addrReplaceSign = localCfg.coreCfg[gCurrentCfg.currentCore].dmaLoadAddrReplaceSign;
        for(uint32_t i = 0;i<replaceCount;i++)
        {
            replaceBitLocal[i] = localCfg.coreCfg[gCurrentCfg.currentCore].dmaReplaceBitLocal[i];
            replaceBitCount[i] = localCfg.coreCfg[gCurrentCfg.currentCore].dmaReplaceBitCount[i];
        }
    }
    else
    {
        replaceCount = localGeneralReplace.generalReplaceCount;
        addrReplaceSign = localGeneralReplace.generalAddrReplaceSign;
        for(uint32_t i = 0;i<replaceCount;i++)
        {
            replaceBitLocal[i] = localGeneralReplace.generalReplaceBitLocal[i];
            replaceBitCount[i] = localGeneralReplace.generalReplaceBitCount[i];
        }
    }

    for(uint32_t i=0;i<replaceCount;i++)
    {
        uint64_t addrMask = 0;
        for(uint32_t j=replaceBitLocal[i];j<replaceBitLocal[i]+replaceBitCount[i];j++)//
            addrMask |= ((uint64_t)1<<j);

        changeCmdListValue(tempList,tempLongList,addrReplaceSign,(*(replaceAddr+i)&addrMask)>>replaceBitLocal[i],replaceBitCount[i],localCfg);
    }
}

void dataConvertNoDmaForAte(QFile* srcFile,QFile* outFile,cfgTemp localCfg)
{
    QString line;
    Uint32 desAddress=0;
    uint32_t cmdValueDataTemp;
    uint64_t cmdValueTemp;
    bool dataNotAlian = false;
    uint32_t dataNotAlianCount = 0;
    QString remainData = "";
    int dataAlianBase = 4;
    uint32_t byteCount = (localCfg.coreCfg[localCfg.currentCore].dmaDataLength/8);
    QString dataTemp;
    QTextStream in(srcFile);
    bool signBypass = false;
    bool ok;
    while(in.readLineInto(&line))
    {
        //处理line中的数据
        if(line.contains("@"))//文件中的地址部分
        {
            if(line.compare(localCfg.coreCfg[localCfg.currentCore].pcInTxt,Qt::CaseInsensitive)==0)//如果此段中包含PC信息
            {
                signBypass = true;
                continue;
            }
            else
            {
                signBypass = false;
            }
            line.replace("@","0x");
            desAddress = line.toUInt(&ok,16);//获取地址
            if(localCfg.coreCfg[localCfg.currentCore].parserName.compare("default")!=0)//default时不需要管地址位宽
                switch(localCfg.coreCfg[localCfg.currentCore].ramAddressMode)
                {
                case ram8bitMode:desAddress /= 1;break;
                case ram16bitMode:desAddress /= 2;break;
                case ram32bitMode:desAddress /= 4;break;
                case ram64bitMode:desAddress /= 8;break;
                }
            if(localCfg.coreCfg[localCfg.currentCore].sectionAddrSub != 0)
                desAddress -= localCfg.coreCfg[localCfg.currentCore].sectionAddrSub;

            dataAlianBase = qPow(2,(localCfg.coreCfg[localCfg.currentCore].ramAdressAlignMode - localCfg.coreCfg[localCfg.currentCore].ramAddressMode));
            if(dataAlianBase<1)
                dataAlianBase = 1;
            if((desAddress%dataAlianBase) != 0)//地址未对齐
            {
                dataNotAlian = true;
                remainData = "";
                dataNotAlianCount = (desAddress%dataAlianBase)*(localCfg.coreCfg[localCfg.currentCore].ramAddressMode+1);
                for(uint32_t i = 0;i < dataNotAlianCount;i++)
                    remainData.append("00 ");
                desAddress -= (desAddress%dataAlianBase);//取对齐的地址，在后续进行数据的填充
            }
            else
                dataNotAlian = false;
        }
        else if(line.contains("q"))
        {
            return;
        }
        else//数据部分
        {
            if(signBypass)
                continue;
            if(dataNotAlian)//补数据
            {
                line.prepend(remainData);
                remainData="";
                for(uint32_t i = 0;i < dataNotAlianCount;i++)
                {
                    dataTemp = line.right(3);
                    line.remove(line.length()-3,3);
                    remainData.prepend(dataTemp);
                }
            }
            while(line.length())
            {
                cmdValueDataTemp = 0;
                for(uint32_t i=0;i<byteCount;i++)
                {
                    dataTemp = line.left(2);
                    line.remove(0,3);
                    if(localCfg.coreCfg[localCfg.currentCore].dataEndian == LITTLE_ENDIAN)//小端
                        cmdValueDataTemp = cmdValueDataTemp + (dataTemp.toUInt(&ok,16)<<(i*8));
                    else//大端
                        cmdValueDataTemp = (cmdValueDataTemp<<8) + dataTemp.toUInt(&ok,16);
                }
                if(localCfg.coreCfg[localCfg.currentCore].dataLeftShiftBitCount)
                    cmdValueTemp = ((Uint64)cmdValueDataTemp<<localCfg.coreCfg[localCfg.currentCore].dataLeftShiftBitCount) + localCfg.coreCfg[localCfg.currentCore].regWriteDataHead;//写数据
                else
                    cmdValueTemp = (Uint64)cmdValueDataTemp + localCfg.coreCfg[localCfg.currentCore].regWriteDataHead;//写数据

                jtagCmdToBin(cmdValueTemp,outFile,IR,localCfg.coreCfg[localCfg.currentCore].coreCmdLength,localCfg);
                if(localCfg.coreCfg[localCfg.currentCore].addrLeftShiftBitCount)
                    cmdValueTemp = localCfg.coreCfg[localCfg.currentCore].regWriteAddrHead + ((Uint64)desAddress<<localCfg.coreCfg[localCfg.currentCore].addrLeftShiftBitCount);
                else
                    cmdValueTemp = localCfg.coreCfg[localCfg.currentCore].regWriteAddrHead + (Uint64)desAddress;

                desAddress += byteCount;
                jtagCmdToBin(cmdValueTemp,outFile,IR,localCfg.coreCfg[localCfg.currentCore].coreCmdLength,localCfg);//写地址
            }
        }
    }
}
void dataIntoCmd(QFile* outFile,uint32_t cmdValueDataTemp,Uint32& dataCount,Uint32& desAddress,int addrDiv,cfgTemp& localCfg,JtagList* dmaStartCmdList,LongCmdList* dmaStartLongCmdList,JtagList* dmaStopCmdList,LongCmdList* dmaStopLongCmdList,bool& sign_dmaStartNeedReplaceAndSendInDataSection,bool& sign_dmaStartNeedSend)
{
    uint64_t cmdValueTemp;
    if(localCfg.coreCfg[localCfg.currentCore].dataLeftShiftBitCount)//数据位移有赋值时
        cmdValueTemp = ((Uint64)cmdValueDataTemp<<localCfg.coreCfg[localCfg.currentCore].dataLeftShiftBitCount) + localCfg.coreCfg[localCfg.currentCore].dmaCmdHead;
    else
        cmdValueTemp = (Uint64)cmdValueDataTemp + localCfg.coreCfg[localCfg.currentCore].dmaCmdHead;

    jtagCmdToBin(cmdValueTemp,outFile,localCfg.coreCfg[localCfg.currentCore].dmaDataInstructionTapState,localCfg.coreCfg[localCfg.currentCore].coreCmdLength,localCfg);//send Cmd
    if(localCfg.coreCfg[localCfg.currentCore].dmaMaxLengthInOneSeq && (dataCount>=localCfg.coreCfg[localCfg.currentCore].dmaMaxLengthInOneSeq))//判断是否超过一次DMA长度的限制
    {
        jtagCmdListToBin(dmaStopCmdList,dmaStopLongCmdList,outFile,&localCfg);//dma stop
        if(localCfg.coreCfg[localCfg.currentCore].parserName.compare("default")!=0)//default时不需要管地址位宽
            dataCount /= addrDiv;
        desAddress += dataCount;
        dataCount = 0;
        if(!sign_dmaStartNeedReplaceAndSendInDataSection)
        {
            JtagList* tempList = new JtagList;
            LongCmdList* tempLongList = new LongCmdList;
            uint32_t addrArray[5] = {desAddress,desAddress,desAddress,desAddress,desAddress};
            addrReplace(dmaStartCmdList,dmaStartLongCmdList,tempList,tempLongList,addrArray,DMA_ADDR_REPLACE,localCfg,localCfg.coreCfg[localCfg.currentCore].generalReplaceSeq[0]);
            jtagCmdListToBin(tempList,tempLongList,outFile,&localCfg);
        }
        else
            sign_dmaStartNeedSend = true;

    }
}
uint32_t remainDataFill(QString* remainData,int dmaDataByteLength,cfgTemp localCfg)
{
    bool ok;
    uint32_t cmdValueDataTemp = 0;
    QString dataTemp;
    int fillCount = dmaDataByteLength - (remainData->length()/3);
    for(int i = 0;i < fillCount;i++)
        remainData->append("00 ");

    for(int i=0;i<dmaDataByteLength;i++)
    {
        dataTemp = remainData->left(2);
        remainData->remove(0,3);
        if(localCfg.coreCfg[localCfg.currentCore].dataEndian == LITTLE_ENDIAN)//小端
            cmdValueDataTemp = cmdValueDataTemp + (dataTemp.toUInt(&ok,16)<<(i*8));
        else//大端
            cmdValueDataTemp = (cmdValueDataTemp<<8) + dataTemp.toUInt(&ok,16);
    }
    return cmdValueDataTemp;
}
void dataConvertForAte(QFile* srcFile,QFile* outFile,JtagList* dmaStartCmdList,LongCmdList* dmaStartLongCmdList,JtagList* dmaStopCmdList,LongCmdList* dmaStopLongCmdList,cfgTemp& localCfg)
{
    QString line;
    int blockCount = 0;
    Uint32 desAddress = 0;
    Uint32 dataCount = 0;//用于一次DMA长度的限制
    Uint32 dataAddrCount = 0;//用于确定两段之间的地址差值
    uint32_t cmdValueDataTemp;
    QString dataTemp;
    //    bool dataNotAlian = false;
    uint32_t dataNotAlianCount = 0;
    QTextStream in(srcFile);
    bool ok;
    bool signBypass = false;
    bool sign_dmaStartNeedReplaceAndSendInDataSection = false;
    bool sign_dmaStartNeedSend = false;
    QString remainData = "";
    int dataAlianBase = 4;
    int addrDiv = 1;
    int dmaDataByteLength = localCfg.coreCfg[localCfg.currentCore].dmaDataLength/8;
    switch(localCfg.coreCfg[localCfg.currentCore].ramAddressMode)
    {
    case ram8bitMode:addrDiv = 1;break;
    case ram16bitMode:addrDiv = 2;break;
    case ram32bitMode:addrDiv = 4;break;
    case ram64bitMode:addrDiv = 8;break;
    default: addrDiv = 1;break;
    }
    for(uint32_t i = 0;i < localCfg.coreCfg[localCfg.currentCore].dmaReplaceCount;i++)
    {
        if(localCfg.coreCfg[localCfg.currentCore].dmaReplaceData[i]>DMA_ADDR)//判断dma start中是否有程序段的内容
            sign_dmaStartNeedReplaceAndSendInDataSection = true;
    }
    while(in.readLineInto(&line))
    {
        //处理line中的数据
        if(line.contains("@"))//文件中的地址部分
        {
            dataCount = 0;

            if(line.compare(localCfg.coreCfg[localCfg.currentCore].pcInTxt,Qt::CaseInsensitive)==0)//如果此段中包含PC信息
            {
                signBypass = true;
                if(blockCount != 0)
                    jtagCmdListToBin(dmaStopCmdList,dmaStopLongCmdList,outFile,&localCfg);
                continue;
            }
            else
                signBypass = false;

            line.replace("@","0x");
            desAddress = line.toUInt(&ok,16);
            if(localCfg.coreCfg[localCfg.currentCore].parserName.compare("default")!=0)//default时不需要管地址位宽
                desAddress /= addrDiv;

            if(localCfg.coreCfg[localCfg.currentCore].sectionAddrSub != 0)
                desAddress -= localCfg.coreCfg[localCfg.currentCore].sectionAddrSub;

            dataAlianBase = qPow(2,(localCfg.coreCfg[localCfg.currentCore].ramAdressAlignMode - localCfg.coreCfg[localCfg.currentCore].ramAddressMode));
            if(dataAlianBase<1)
                dataAlianBase = 1;

            int addressDiffInByte = (desAddress-dataAddrCount)*addrDiv;//所差的byte数
            if(((blockCount != 0)&&(addressDiffInByte==0))||((!remainData.isEmpty())&&(addressDiffInByte+remainData.length()/3 == dmaDataByteLength)))//地址刚好衔接上,说明上一次不存在remain数据，且此地址不是奇数地址
                continue;
            else if((blockCount != 0) && addressDiffInByte>localCfg.coreCfg[localCfg.currentCore].sectionAddrDiffInByte)//两端之间地址超过了设置的上限
            {
                if(!remainData.isEmpty())
                {
                    cmdValueDataTemp = remainDataFill(&remainData,dmaDataByteLength,localCfg);
                    dataCount += dmaDataByteLength;
                    dataIntoCmd(outFile,cmdValueDataTemp,dataCount,desAddress,addrDiv,localCfg,dmaStartCmdList,dmaStartLongCmdList,dmaStopCmdList,dmaStopLongCmdList,sign_dmaStartNeedReplaceAndSendInDataSection,sign_dmaStartNeedSend);
                }
                jtagCmdListToBin(dmaStopCmdList,dmaStopLongCmdList,outFile,&localCfg);
            }
            else if(blockCount != 0)//差值的地方补零
            {
                if(!remainData.isEmpty())
                {
                    addressDiffInByte -= dmaDataByteLength;
                    cmdValueDataTemp = remainDataFill(&remainData,dmaDataByteLength,localCfg);
                    dataCount += dmaDataByteLength;
                    dataAddrCount += dmaDataByteLength/addrDiv;
                    dataIntoCmd(outFile,cmdValueDataTemp,dataCount,desAddress,addrDiv,localCfg,dmaStartCmdList,dmaStartLongCmdList,dmaStopCmdList,dmaStopLongCmdList,sign_dmaStartNeedReplaceAndSendInDataSection,sign_dmaStartNeedSend);
                }
                for(int j = 0;j<addressDiffInByte/dmaDataByteLength;j++)
                {
                    cmdValueDataTemp = 0x0;
                    dataCount += dmaDataByteLength;
                    dataAddrCount += dmaDataByteLength/addrDiv;
                    dataIntoCmd(outFile,cmdValueDataTemp,dataCount,desAddress,addrDiv,localCfg,dmaStartCmdList,dmaStartLongCmdList,dmaStopCmdList,dmaStopLongCmdList,sign_dmaStartNeedReplaceAndSendInDataSection,sign_dmaStartNeedSend);
                }
                if(addressDiffInByte%dmaDataByteLength != 0)
                {
                    dataNotAlianCount = dmaDataByteLength - (addressDiffInByte%dmaDataByteLength);
                    for(uint32_t i = 0;i < dataNotAlianCount;i++)
                        remainData.append("00 ");
                }

                continue;
            }

            if((desAddress%dataAlianBase) != 0)//地址未对齐
            {
                if(!remainData.isEmpty())
                {
                    //将剩余数据先下发
                    for(uint32_t i = 0;i < dataNotAlianCount;i++)
                        remainData.append("00 ");
                    remainData="";
                }
                dataNotAlianCount = (desAddress%dataAlianBase)*(localCfg.coreCfg[localCfg.currentCore].ramAddressMode+1);
                for(uint32_t i = 0;i < dataNotAlianCount;i++)
                    remainData.append("00 ");
                desAddress -= (desAddress%dataAlianBase);//取对齐的地址，在后续进行数据的填充
            }
            dataAddrCount = desAddress;//取经过了对齐、减、及位宽操作后的下载地址
            if(!sign_dmaStartNeedReplaceAndSendInDataSection)
            {
                JtagList* tempList = new JtagList;
                LongCmdList* tempLongList = new LongCmdList;
                uint32_t addrArray[5] = {desAddress,desAddress,desAddress,desAddress,desAddress};
                addrReplace(dmaStartCmdList,dmaStartLongCmdList,tempList,tempLongList,addrArray,DMA_ADDR_REPLACE,localCfg,localCfg.coreCfg[localCfg.currentCore].generalReplaceSeq[0]);
                jtagCmdListToBin(tempList,tempLongList,outFile,&localCfg);
            }
            else
                sign_dmaStartNeedSend = true;
            blockCount++;
        }
        else if(line.contains("q"))
        {
            dataCount = 0;
            if(!remainData.isEmpty())//将最后一个非对齐下发
            {
                cmdValueDataTemp = remainDataFill(&remainData,dmaDataByteLength,localCfg);
                dataIntoCmd(outFile,cmdValueDataTemp,dataCount,desAddress,addrDiv,localCfg,dmaStartCmdList,dmaStartLongCmdList,dmaStopCmdList,dmaStopLongCmdList,sign_dmaStartNeedReplaceAndSendInDataSection,sign_dmaStartNeedSend);
            }
            jtagCmdListToBin(dmaStopCmdList,dmaStopLongCmdList,outFile,&localCfg);
        }
        else//数据部分
        {
            uint32_t lineDataCount = 0;
            uint32_t dataRecord[5] = {0};
            if(signBypass)
                continue;
            if(!remainData.isEmpty())//补数据
            {
                line.prepend(remainData);
                remainData="";
            }
            if(line.length()%3 != 0)//不满足3对齐
            {
                line.append(" ");
            }
            int cutCount = (line.length()/3)%dmaDataByteLength;
            for(int i = 0;i < cutCount;i++)//一行中存在未对齐的
            {
                dataTemp = line.right(3);
                line.remove(line.length()-3,3);
                remainData.prepend(dataTemp);
            }

            while(line.length())
            {
                cmdValueDataTemp = 0;
                for(int i=0;i<dmaDataByteLength;i++)
                {
                    dataTemp = line.left(2);
                    line.remove(0,3);
                    if(localCfg.coreCfg[localCfg.currentCore].dataEndian == LITTLE_ENDIAN)//小端
                        cmdValueDataTemp = cmdValueDataTemp + (dataTemp.toUInt(&ok,16)<<(i*8));
                    else//大端
                        cmdValueDataTemp = (cmdValueDataTemp<<8) + dataTemp.toUInt(&ok,16);
                    dataCount += 1;
                }
                dataAddrCount += dmaDataByteLength/addrDiv;

                if(sign_dmaStartNeedReplaceAndSendInDataSection && sign_dmaStartNeedSend)//在此进行数据的替换以及下发dma start
                {
                    if(line.length() == 0)//已经取到了全部的数据,进行数据替换及下发,提取数据不能超过4
                    {
                        uint32_t i,maxNum = 0;
                        uint32_t dataRecordArray[5] = {0};
                        dataRecord[lineDataCount++] = cmdValueDataTemp;//记录数据
                        for(i = 0;i<localCfg.coreCfg[localCfg.currentCore].dmaReplaceCount;i++)//进行排序
                        {
                            switch(localCfg.coreCfg[localCfg.currentCore].dmaReplaceData[i])
                            {
                            case DMA_ADDR:
                                dataRecordArray[i] = desAddress;
                                break;
                            case PROGRAM_DATA0:
                                maxNum = maxNum<PROGRAM_DATA0?PROGRAM_DATA0:maxNum;
                                dataRecordArray[i] = dataRecord[0];
                                break;
                            case PROGRAM_DATA1:
                                maxNum = maxNum<PROGRAM_DATA1?PROGRAM_DATA1:maxNum;
                                dataRecordArray[i] = dataRecord[1];
                                break;
                            case PROGRAM_DATA2:
                                maxNum = maxNum<PROGRAM_DATA2?PROGRAM_DATA2:maxNum;
                                dataRecordArray[i] = dataRecord[2];
                                break;
                            case PROGRAM_DATA3:
                                maxNum = maxNum<PROGRAM_DATA3?PROGRAM_DATA3:maxNum;
                                dataRecordArray[i] = dataRecord[3];
                                break;
                            }
                        }
                        JtagList* tempList = new JtagList;
                        LongCmdList* tempLongList = new LongCmdList;
                        addrReplace(dmaStartCmdList,dmaStartLongCmdList,tempList,tempLongList,dataRecordArray,DMA_ADDR_REPLACE,localCfg,localCfg.coreCfg[localCfg.currentCore].generalReplaceSeq[0]);
                        jtagCmdListToBin(tempList,tempLongList,outFile,&localCfg);
                        for(uint32_t j = maxNum;j<lineDataCount;j++)//剩余数据下发
                            dataIntoCmd(outFile,dataRecord[j],dataCount,desAddress,addrDiv,localCfg,dmaStartCmdList,dmaStartLongCmdList,dmaStopCmdList,dmaStopLongCmdList,sign_dmaStartNeedReplaceAndSendInDataSection,sign_dmaStartNeedSend);

                        sign_dmaStartNeedSend = false;
                    }
                    else
                    {
                        dataRecord[lineDataCount++] = cmdValueDataTemp;//记录数据
                    }
                }
                else
                    dataIntoCmd(outFile,cmdValueDataTemp,dataCount,desAddress,addrDiv,localCfg,dmaStartCmdList,dmaStartLongCmdList,dmaStopCmdList,dmaStopLongCmdList,sign_dmaStartNeedReplaceAndSendInDataSection,sign_dmaStartNeedSend);
            }
        }
    }
}
void excelDataConvertForAte(JtagList* jtagCmdList,QFile* outFile,cfgTemp localCfg)
{
    QTextStream out(outFile);
    Uint32 length = jtagCmdList->length;

    while(length)
    {
        jtagCmdToBin(jtagCmdList->current->getValue(),outFile,jtagCmdList->current->getType(),jtagCmdList->current->getBitCount(),localCfg);
        jtagCmdList->current = jtagCmdList->current->getNext();
        length--;
    }
    jtagCmdList->current = jtagCmdList->head;

}
int fileConvertForATE(QFile* srcFile,QFile* outFile )//返回值：-1：打不开临时文件 -2：找不到配置文件指明的文件或路径 -3:找不到头文件
{
    QString filePath =gCurrentCfg.coreCfg[gCurrentCfg.currentCore].coreLibPath;
    QString operaFilePath = filePath;
    JtagList* jtagCmdList = new JtagList;
    LongCmdList* longCmdList = new LongCmdList;
    JtagList* dmaStartCmdList = new JtagList;
    LongCmdList* dmaStartLongCmdList = new LongCmdList;
    JtagList* dmaStopCmdList = new JtagList;
    LongCmdList* dmaStopLongCmdList = new LongCmdList;
    QFile outTempFile(gDefaultFilePath+"/tempFile.c");
    if(!outTempFile.open(QIODevice::ReadWrite|QIODevice::Text)){
        return -1;
    }
    outTempFile.resize(0);
    //打开excel
    ExcelBase *excelHandle;
    excelHandle= new ExcelBase();
    excelHandle->excel = new QAxObject(gWindow);
    //只有机测、仿真时，使用DMA才去取dma指令，实装只提供数据段
    if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].useDma && gSign_outFileImport)//使用dma的情况,仅out导入时需要
    {
        QList<QStringList> data;

        //存储dma启动的命令路径
        operaFilePath.append(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].dmaStartSeq);
        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].dmaStartSeq.contains(".csv"))//csv的情况
        {
            if(csvRead(operaFilePath,data)==-1)
            {
                gWindow->ui->textBrowser_logShower->append("找不到配置文件指明的库文件，请检查路径及库是否正确！");
                return -2;
            }
            csvCmdReadIn(data,data.length(),dmaStartCmdList,dmaStartLongCmdList);
        }
        else//excel的情况
        {
            if(excelRead(operaFilePath,excelHandle,dmaStartCmdList,dmaStartLongCmdList)==-1)
            {
                gWindow->ui->textBrowser_logShower->append("找不到配置文件指明的库文件，请检查路径及库是否正确！");
                return -2;
            }
        }
        data.clear();
        operaFilePath = filePath;
        //存储dma结束的命令路径
        operaFilePath.append(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].dmaStopSeq);
        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].dmaStopSeq.contains(".csv"))//csv的情况
        {
            if(csvRead(operaFilePath,data)==-1)
            {
                gWindow->ui->textBrowser_logShower->append("找不到配置文件指明的库文件，请检查路径及库是否正确！");
                return -2;
            }
            csvCmdReadIn(data,data.length(),dmaStopCmdList,dmaStopLongCmdList);
        }
        else//excel的情况
        {
            if(excelRead(operaFilePath,excelHandle,dmaStopCmdList,dmaStopLongCmdList)==-1)
            {
                gWindow->ui->textBrowser_logShower->append("找不到配置文件指明的库文件，请检查路径及库是否正确！");
                return -2;
            }
        }
    }

    //指令格式为csv或者excel文件
    for(uint32_t i=0;i<gCurrentCfg.coreCfg[gCurrentCfg.currentCore].seqCount;i++)//按照流程下发指令
    {
        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].seqName[i].compare("DATA") == 0)
        {
            //data段,下发DMA指令及数据
            if(gSign_outFileImport)
            {
                if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].useDma)//使用dma，多次dma过程
                    dataConvertForAte(srcFile,&outTempFile,dmaStartCmdList,dmaStartLongCmdList,dmaStopCmdList,dmaStopLongCmdList,gCurrentCfg);
                else//不使用dma
                    dataConvertNoDmaForAte(srcFile,&outTempFile,gCurrentCfg);
            }
            else
                excelDataConvertForAte(gJtagCmdList,&outTempFile,gCurrentCfg);
        }
        else
        {
            //指令段，读指令文件并下发
            operaFilePath = filePath;
            operaFilePath.append(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].seqName[i]);
            jtagCmdList->removeAll();
            longCmdList->removeAll();
            if(operaFilePath.contains(".csv"))//文件为csv格式
            {
                bool replacedSign=false;
                QList<QStringList> data;
                if(csvRead(operaFilePath,data)==-1)
                {
                    gWindow->ui->textBrowser_logShower->append("找不到配置文件指明的库文件，请检查路径及库是否正确！");
                    return -2;
                }
                csvCmdReadIn(data,data.length(),jtagCmdList,longCmdList);

                for(Uint32 j=0;j<gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeqCount;j++)//查看是否需要进行地址替换操作
                {
                    //重复确认关键字替换
                    if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].generalReplaceData.compare("PC",Qt::CaseInsensitive) == 0)
                        gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data = gPcStartAddr;

                    for(int k =0;k<5;k++)
                        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].generalReplaceData.compare(gGeneralReplaceDataSign[k],Qt::CaseInsensitive) == 0)
                        {
                            gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data = gHexData[k];
                            break;
                        }

                    if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].seqName[i].compare(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].seqName) == 0)//需要地址替换的seq
                    {
                        replacedSign = true;
                        JtagList* tempList = new JtagList;
                        LongCmdList* tempLongList = new LongCmdList;
                        uint32_t dataArray[5] = {gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data};
                        addrReplace(jtagCmdList,longCmdList,tempList,tempLongList,dataArray,GENERAL_ADDR_REPLACE,gCurrentCfg,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j]);
                        jtagCmdListToBin(tempList,tempLongList,&outTempFile,&gCurrentCfg);
                        break;
                    }
                }
                if(!replacedSign)//如果未替换，正常下发
                    jtagCmdListToBin(jtagCmdList,longCmdList,&outTempFile,&gCurrentCfg);
            }
            else //文件为xlsx格式
            {
                bool replacedSign=false;
                if(excelRead(operaFilePath,excelHandle,jtagCmdList,longCmdList)==-1)
                {
                    gWindow->ui->textBrowser_logShower->append("找不到配置文件指明的库文件，请检查路径及库是否正确！");
                    return -2;
                }
                for(Uint32 j=0;j<gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeqCount;j++)//查看是否需要进行地址替换操作
                {
                    if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].seqName[i].compare(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].seqName) == 0)//需要地址替换的seq
                    {
                        replacedSign = true;
                        JtagList* tempList = new JtagList;
                        LongCmdList* tempLongList = new LongCmdList;
                        uint32_t dataArray[5] = {gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j].data};
                        addrReplace(jtagCmdList,longCmdList,tempList,tempLongList,dataArray,GENERAL_ADDR_REPLACE,gCurrentCfg,gCurrentCfg.coreCfg[gCurrentCfg.currentCore].generalReplaceSeq[j]);
                        jtagCmdListToBin(tempList,tempLongList,&outTempFile,&gCurrentCfg);
                        break;
                    }
                }
                if(!replacedSign)//如果未替换，正常下发
                    jtagCmdListToBin(jtagCmdList,longCmdList,&outTempFile,&gCurrentCfg);
            }
        }
    }

    //将数据从临时文件转移至结果文件中
    if(outFileGenerate(outFile,&outTempFile,nullptr,0,gCurrentCfg)==-1)
    {
        gWindow->ui->textBrowser_logShower->append("请确认lib中包含所设头文件！");
        return -3;
    }
    outTempFile.close();
    excelHandle->excel->dynamicCall("Quit(void)");
    delete excelHandle->excel;
    excelHandle->excel = nullptr;
    delete excelHandle;//删除excel对象
    excelHandle = nullptr;
    return 0;
}
void readFile(QFile* outFile)
{
    QString line;
    QTextStream in(outFile);
    gWindow->ui->textBrowser_resShower->clear();
    line = in.readAll();
    gWindow->ui->textBrowser_resShower->append(line);
}
void MainWindow::on_toolButton_fileConvert_clicked()
{
    int returnState = 0;
    ui->textBrowser_resShower->clear();//清空
    if(gCurrentCfg.coreCount == 0)//未启用配置
    {
        gWindow->ui->textBrowser_logShower->append("请先启用配置！");
        return;
    }
    else
    {
        //打开目标文件
        QFile outFile((gDefaultFilePath+"/jtagCmd.h"));
        if(!outFile.open(QIODevice::WriteOnly|QIODevice::Text)){
            QMessageBox::information(this, tr("warnning"),tr("打开jtagCmd.h失败！"), QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        outFile.resize(0);

        if(gCurrentCfg.coreCfg[gCurrentCfg.currentCore].autoRefreshSourceFile)
        {
            gWindow->ui->textBrowser_logShower->append("source file:"+gChoosedFileName.mid(gChoosedFileName.lastIndexOf("/")+1));
            if(sourceFileOpen(gChoosedFileName) == -1)
                return;
        }

        if(gSign_csvFileImport)//需要转换的文件为excel
        {
            if(gCurrentCfg.codeFormate==TEST_CODE)//选择指令生成格式为hex格式
            {
                this->ui->textBrowser_logShower->append("暂不支持将导入的excel转为实装码...");
                return;
            }
            else if((gCurrentCfg.codeFormate == ATE_CODE)||(gCurrentCfg.codeFormate == EMU_CODE))//选择指令生成格式为bin格式
            {
                this->ui->textBrowser_logShower->append("转换中...");
                QApplication::processEvents();

                returnState = fileConvertForATE(nullptr,&outFile);
            }
            else
            {
                QMessageBox::information(this, tr("warnning"),tr("请指定指令生成格式！"), QMessageBox::Ok, QMessageBox::Ok);
                return;
            }
        }
        else if(gSign_outFileImport)
        {
            QString strFileName = gDefaultFilePath + "/out.txt";
            QFile srcFile(strFileName);
            if(!srcFile.open(QIODevice::ReadOnly|QIODevice::Text)){
                QMessageBox::information(this, tr("warnning"),tr("转换失败，未找到转换文件！请先选中.out文件"), QMessageBox::Ok, QMessageBox::Ok);
                return;
            }
            ui->textBrowser_logShower->append("文件转换中...");
            QApplication::processEvents();

            if(gCurrentCfg.codeFormate == TEST_CODE)//实装码
            {
                QFile outDataFile((gDefaultFilePath+"/jtagData.h"));
                QFile outAddrFile((gDefaultFilePath+"/jtagAddr.h"));
                //打开目标data文件
                if(!outDataFile.open(QIODevice::ReadWrite|QIODevice::Text)){
                    QMessageBox::information(this, tr("warnning"),tr("打开jtagData.h失败！"), QMessageBox::Ok, QMessageBox::Ok);
                    return;}
                if(!outAddrFile.open(QIODevice::ReadWrite|QIODevice::Text)){
                    QMessageBox::information(this, tr("warnning"),tr("打开jtagAddr.h失败！"), QMessageBox::Ok, QMessageBox::Ok);
                    return;}
                outDataFile.resize(0);
                outAddrFile.resize(0);

                returnState = dataConvertForTest(&srcFile,&outFile,&outDataFile,&outAddrFile);
                outDataFile.close();
                outAddrFile.close();
            }
            else if((gCurrentCfg.codeFormate == ATE_CODE)||(gCurrentCfg.codeFormate == EMU_CODE))//机测仿真码
            {
                returnState = fileConvertForATE(&srcFile,&outFile);
            }
            else
            {
                gWindow->ui->textBrowser_logShower->append("请指定生成码的类型！");
                srcFile.close();outFile.close();
                return;
            }
            srcFile.close();
        }
        else
        {
            gWindow->ui->textBrowser_logShower->append("未导入文件，请先导入out或者excel文件！");
            outFile.close();return;
        }

        outFile.close();
        QString fileOpt = gDefaultFilePath+"/jtagCmd.h";
        if(returnState == 0)
        {
            if((gCurrentCfg.codeFormate==ATE_CODE)||(gCurrentCfg.codeFormate==TEST_CODE))
            {
                outFile.open(QIODevice::ReadOnly|QIODevice::Text);
                readFile(&outFile);
                outFile.close();
            }
            else
            {
                fileOpt.prepend("notepad ");
                executeCommandNotWait(fileOpt);
            }
            //复制至剪贴板
            copyToClip(fileOpt);
            //自动另存为
            if(gCurrentCfg.resultFileAutoSave)
            {
                QString tarPath = gCurrentCfg.resultFileAutoSavePath;
                int num = gChoosedFileName.lastIndexOf("/");
                QString fileName = gChoosedFileName.mid(num+1);
                num = fileName.lastIndexOf(".");
                fileName.remove(num,(fileName.length()-num));
                QString coreNumName = QString("_core%1").arg(gCurrentCfg.currentCore);
                fileName.append(coreNumName);
                switch(gCurrentCfg.codeFormate)
                {
                case ATE_CODE: fileName.append("_"+gCurrentCfg.coreCfg[gCurrentCfg.currentCore].ateHeadFileName);break;
                case TEST_CODE: fileName.append("_"+gCurrentCfg.coreCfg[gCurrentCfg.currentCore].testHeadFileName);break;
                case EMU_CODE:fileName.append("_"+gCurrentCfg.coreCfg[gCurrentCfg.currentCore].emuHeadFileName); break;
                default:fileName.append("_jtagCmd.h"); break;
                }
                if(tarPath.right(0).compare("/")==0)
                    tarPath.append(fileName);
                else
                    tarPath.append("/"+fileName);

                tarPath.replace("/","\\");

                QString srcPath = gDefaultFilePath+"/jtagCmd.h";
                srcPath.replace("/","\\");
                QString winCmd = QString("copy %1 %2 /y").arg(srcPath).arg(tarPath);
                executeCommandNotWait(winCmd);
            }
            this->ui->textBrowser_logShower->append("转换完成");

            this->ui->textBrowser_resShower->moveCursor(this->ui->textBrowser_resShower->textCursor().Start);
        }
        else
            this->ui->textBrowser_logShower->append("转换失败！");
    }
}
void MainWindow::action_fileConvert_clicked()
{
    MainWindow::on_toolButton_fileConvert_clicked();
}
void MainWindow::action_emuFileConvert_clicked()
{
    //    generateCodeFormate temp = gCurrentCfg.codeFormate;//保存原来状态
    gCurrentCfg.codeFormate = EMU_CODE;
    cfgTemp* tempCfg = matchCfg(gCurrentCfg.cfgName);
    tempCfg->codeFormate = EMU_CODE;

    changeConvertBottomText(EMU_CODE);
    MainWindow::on_toolButton_fileConvert_clicked();
    //    gCurrentCfg.codeFormate = temp;//恢复原来状态
}
void MainWindow::action_ateFileConvert_clicked()
{
    //    generateCodeFormate temp = gCurrentCfg.codeFormate;//保存原来状态
    gCurrentCfg.codeFormate = ATE_CODE;
    cfgTemp* tempCfg = matchCfg(gCurrentCfg.cfgName);
    tempCfg->codeFormate = ATE_CODE;
    changeConvertBottomText(ATE_CODE);
    MainWindow::on_toolButton_fileConvert_clicked();
    //    gCurrentCfg.codeFormate = temp;//恢复原来状态
}
void MainWindow::action_testFileConvert_clicked()
{
    //    generateCodeFormate temp = gCurrentCfg.codeFormate;//保存原来状态
    gCurrentCfg.codeFormate = TEST_CODE;
    cfgTemp* tempCfg = matchCfg(gCurrentCfg.cfgName);
    tempCfg->codeFormate = TEST_CODE;
    changeConvertBottomText(TEST_CODE);
    MainWindow::on_toolButton_fileConvert_clicked();
    //    gCurrentCfg.codeFormate = temp;//恢复原来状态
}
int sourceFileOpen(QString filePath)
{
    //-2:文件后缀名错误,-1: error,0:correct
    if(filePath.contains(".out")||filePath.contains(".coff")||filePath.contains(".elf"))
    {
        return(outFileOpen(filePath));
    }
    else if(filePath.contains(".csv"))
    {
        return(csvCmdLoad(filePath));
    }
    else if(filePath.contains(".xls")||filePath.contains(".xlsx"))
    {
        return(excelCmdLoad(filePath));
    }
    else if(filePath.contains(".hex"))
    {
        return(hexFileLoad(filePath));
    }
    else if(filePath.contains(".txt"))
    {
        return(txtFileLoad(filePath));
    }
    gWindow->ui->textBrowser_logShower->append("未找到对应文件后缀名的处理器！");
    return -1;
}
void quickSort(int array[], int low, int high) {
    int i = low;
    int j = high;
    if(i >= j) {
        return;
    }

    int temp = array[low];
    while(i != j) {
        while(array[j] >= temp && i < j) {
            j--;
        }
        while(array[i] <= temp && i < j) {
            i++;
        }
        if(i < j) {
            std::swap(array[i], array[j]);
        }
    }

    //将基准temp放于自己的位置，（第i个位置）
    std::swap(array[low], array[i]);
    quickSort(array, low, i - 1);
    quickSort(array, i + 1, high);
}
void outFileSort(QString filePath)
{
    QString line;
    int addrList[30];
    int count = 0;
    bool ok;
    QString outPath = filePath+"/outBack.txt";
    QString srcPath = filePath+"/out.txt";
    QFile srcFile(srcPath);
    srcFile.open(QIODevice::ReadWrite|QIODevice::Text);
    QTextStream in(&srcFile);
    QFile outFile(outPath);
    outFile.open(QIODevice::ReadWrite|QIODevice::Text);
    outFile.resize(0);
    QTextStream out(&outFile);
    while(in.readLineInto(&line))
    {
        if(line.contains("@"))//文件中的地址部分
        {
            line.replace("@","0x");
            addrList[count++] = line.toUInt(&ok,16);
        }
    }

    //快排
    quickSort(addrList,0,count-1);
    //根据排序后内容对文件排序
    QString localTemp;
    for(int i= 0;i<count;i++)
    {
        localTemp = QString::number(addrList[i],16);
        in.seek(0);
        while(in.readLineInto(&line))
        {
            if(line.contains(localTemp,Qt::CaseInsensitive))
                break;
        }
        out<<line<<"\n";
        while(in.readLineInto(&line))
        {
            if(line.contains("@")||line.contains("q"))
                break;
            out<<line<<"\n";
        }
    }
    out<<"q";
    srcFile.close();
    outFile.close();
    outPath.replace("/","\\");
    srcPath.replace("/","\\");
    fileCopy(outPath,srcPath);
    //    fileDel(outPath);
}
void MainWindow::on_pushButton_openSource_clicked()
{
    if(gCurrentCfg.coreCount == 0)
    {
        gWindow->ui->textBrowser_logShower->append("请先启用配置文件！");
        return;
    }
    QFileDialog *pFileName = new QFileDialog(this, Qt::Dialog);
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";
    pFileName->setDirectory(gStrFileName);//设置打开的默认目录
    //    pFileName->setNameFilter(tr("out(*.out *.coff);;excel(*.csv *.xls *.xlsx)"));//设置限定文件类型
    pFileName->setNameFilter(tr("file(*.out *.elf *.coff *.csv *.xls *.xlsx *.hex *.txt)"));//设置限定文件类型
    pFileName->setOption(QFileDialog::DontResolveSymlinks);//可以加载链接路径而不会跳转

    if (pFileName->exec()!=QFileDialog::Accepted)
    {
        return;
    }

    //存储选中文件路径
    gStrFileName = pFileName->selectedFiles().first();
    gChoosedFileName = gStrFileName;//存储选中的文件路径，用于在转换时更新源文件
    delete pFileName;

    sourceFileOpen(gChoosedFileName);
    if(gChoosedFileName.contains(".out")||gChoosedFileName.contains(".coff")||gChoosedFileName.contains(".hex")||gChoosedFileName.contains(".txt"))//对out进行排序
        outFileSort(gDefaultFilePath);

}
void MainWindow::on_pushButton_saveFileAs_clicked()
{
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), tr("jtagCmd.h"),tr("*.h"));

    QString tarPath = fileName;
    QString srcPath = gDefaultFilePath+"/jtagCmd.h";
    tarPath.replace("/","\\");
    srcPath.replace("/","\\");
    QString winCmd = QString("copy %1 %2 /y").arg(srcPath).arg(tarPath);
    executeCommandNotWait(winCmd);
}
void MainWindow::action_saveFileAs_clicked()
{
    MainWindow::on_pushButton_saveFileAs_clicked();
}
void MainWindow::action_showCfgMethod_clicked()
{
    // 指定编码格式
    QTextCodec *codec=QTextCodec::codecForName("utf-8");
    QFile readMe(gDefaultFilePath+"/read_me.txt");
    if(readMe.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        gWindow->ui->textBrowser_resShower->clear();
        QByteArray line = readMe.readAll();
        QString str=codec->toUnicode(line);
        gWindow->ui->textBrowser_resShower->append(str);
    }
    readMe.close();
}
void MainWindow::action_saveSpace_clicked()
{
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), tr("lib.ini"),tr("*.ini"));

    QString tarPath = fileName;
    QString srcPath = gDefaultFilePath+"/log.ini";
    tarPath.replace("/","\\");
    srcPath.replace("/","\\");
    fileCopy(srcPath,tarPath);
}
void MainWindow::action_switchSpace_clicked()
{
    /*excel处理模块中的导入excel文件操作*/
    QFileDialog *pFileName = new QFileDialog(this, Qt::Dialog);
    if(gStrFileName.isEmpty())
        gStrFileName = "D:/";
    pFileName->setDirectory(gStrFileName);//设置打开的默认目录
    pFileName->setNameFilter(tr("lib(*.ini)"));//设置限定文件类型
    pFileName->setOption(QFileDialog::DontResolveSymlinks);//可以加载链接路径而不会跳转

    if (pFileName->exec()!=QFileDialog::Accepted)
    {
        return;
    }
    gStrFileName = pFileName->selectedFiles().first();
    QString srcPath = gStrFileName;
    gCfgList.removeAll();
    this->model->removeRows(0,gItemNum);
    logFileRead(this,gStrFileName);

    QString tarPath = gDefaultFilePath+"/log.ini";
    tarPath.replace("/","\\");
    srcPath.replace("/","\\");
    fileCopy(srcPath,tarPath);
}


void MainWindow::on_action_enableAteConvert_triggered()
{
    gCurrentCfg.codeFormate = ATE_CODE;
    cfgTemp* tempCfg = matchCfg(gCurrentCfg.cfgName);
    tempCfg->codeFormate = ATE_CODE;
    changeConvertBottomText(ATE_CODE);
}

void MainWindow::on_action_enableTestConvert_triggered()
{
    gCurrentCfg.codeFormate = TEST_CODE;
    cfgTemp* tempCfg = matchCfg(gCurrentCfg.cfgName);
    tempCfg->codeFormate = TEST_CODE;
    changeConvertBottomText(TEST_CODE);
}

void MainWindow::on_action_enableEmuConvert_triggered()
{
    gCurrentCfg.codeFormate = EMU_CODE;
    cfgTemp* tempCfg = matchCfg(gCurrentCfg.cfgName);
    tempCfg->codeFormate = EMU_CODE;
    changeConvertBottomText(EMU_CODE);
}

