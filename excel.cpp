#include "excel.h"
#include <QTextCodec>

ExcelBase::ExcelBase()
{

}

QVariant ExcelBase::readAll()
{
    QVariant var;
    if (this->workSheet != NULL && ! this->workSheet->isNull())
    {
        QAxObject *usedRange = this->workSheet->querySubObject("UsedRange");
        if(NULL == usedRange || usedRange->isNull())
        {
            return var;
        }
        var = usedRange->dynamicCall("Value");
        delete usedRange;
    }
    return var;
}
//
// \brief 把QVariant转为QList<QList<QVariant> >
// \param var
// \param res
//
void ExcelBase::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res)
{
    QVariantList varRows = var.toList();
    if(varRows.isEmpty())
    {
        return;
    }
    const int rowCount = varRows.size();
    QVariantList rowData;
    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        res.push_back(rowData);
    }
}
void getCmdFromList(QList<QList<QVariant>> excelData,int cmdSeq,int typeColumn,int tdiColumn,int countColumn,int tapColumn,jtagCmdContentTemp* contentTemp)
{
    bool ok;
    contentTemp->type = excelData[cmdSeq][typeColumn].toString();
    contentTemp->bitCount = excelData[cmdSeq][countColumn].toUInt();
    uint16_t temp = excelData[cmdSeq][tapColumn].toUInt();
    if(temp == 0 ||temp == 1)
        contentTemp->isTap = temp;
    else
        contentTemp->isTap = 0;

    if(contentTemp->bitCount <= 64)
    {
        if((contentTemp->type!=IR)&&(contentTemp->type!=DR))
            contentTemp->tdiValue = excelData[cmdSeq][tdiColumn].toString().toULongLong(&ok,10);
        else
            contentTemp->tdiValue = excelData[cmdSeq][tdiColumn].toString().toULongLong(&ok,16);
    }
    else
        contentTemp->longTdiValue = excelData[cmdSeq][tdiColumn].toString();
}
void getCmdFromStrList(QList<QStringList> excelData,int cmdSeq,int typeColumn,int tdiColumn,int countColumn,int tapColumn,jtagCmdContentTemp* contentTemp)
{
    bool ok;
    contentTemp->type = excelData[cmdSeq][typeColumn];
    contentTemp->bitCount = excelData[cmdSeq][countColumn].toUInt();
    uint16_t temp = excelData[cmdSeq][tapColumn].toUShort();
    if(temp == 0 ||temp == 1)
        contentTemp->isTap = temp;
    else
        contentTemp->isTap = 0;

    if(contentTemp->bitCount <= 64)
    {
        if((contentTemp->type.compare("Shift-IR") == 0)||(contentTemp->type.compare("Shift-DR") == 0))
            contentTemp->tdiValue = excelData[cmdSeq][tdiColumn].toULongLong(&ok,16);
        else
            contentTemp->tdiValue = excelData[cmdSeq][tdiColumn].toULongLong(&ok,10);
    }
    else
        contentTemp->longTdiValue = excelData[cmdSeq][tdiColumn];
}
void jtagListAppendCmd(jtagCmdContentTemp contentTemp,JtagTemp* jtagTemp,bool* sign_notFirstEnter,JtagList* localCmdList,LongCmdList* localLongCmdList)
{
    LongCmdTemp temp;
    if(contentTemp.type.compare("Shift-IR") == 0)
    {
        if(*sign_notFirstEnter)
        {
            localCmdList->append(jtagTemp);//读取完一个指令之后添加至链表中
            jtagTemp->setNextState(RUN_TEST_IDLE_STATE);//恢复状态机至默认
        }
        else
            *sign_notFirstEnter = true;
        jtagTemp->setBitCount(contentTemp.bitCount);
        if(contentTemp.bitCount>64)
        {
            temp.cmdValue = contentTemp.longTdiValue;
            temp.next = nullptr;
            localLongCmdList->append(&temp);
        }
        else
            jtagTemp->setValue(contentTemp.tdiValue);
        jtagTemp->setType(IR);
        jtagTemp->setTap(contentTemp.isTap);
    }
    else if(contentTemp.type.compare("Shift-DR") == 0)
    {
        if(*sign_notFirstEnter)
        {
            localCmdList->append(jtagTemp);//读取完一个指令之后添加至链表中
            jtagTemp->setNextState(RUN_TEST_IDLE_STATE);//恢复状态机至默认
        }
        else
            *sign_notFirstEnter = true;
        jtagTemp->setBitCount(contentTemp.bitCount);
        if(contentTemp.bitCount>64)
        {
            temp.cmdValue = contentTemp.longTdiValue;
            temp.next = nullptr;
            localLongCmdList->append(&temp);
        }
        else
            jtagTemp->setValue(contentTemp.tdiValue);

        jtagTemp->setType(DR);
        jtagTemp->setTap(contentTemp.isTap);
    }
    else//控制字段及状态指令
    {
        if(contentTemp.type.compare("Test-Logic-Reset") == 0)
            jtagTemp->setNextState(TEST_LOGIC_STATE);
        else if(contentTemp.type.compare("Pause-DR") == 0)
            jtagTemp->setNextState(PAUSE_TEST_DATA_REGISTER_STATE);
        else if(contentTemp.type.compare("Pause-IR") == 0)
            jtagTemp->setNextState(PAUSE_INSTRUCTION_REGISTER_STATE);
        else if(contentTemp.type.compare("#DELAY") == 0)
        {
            if(*sign_notFirstEnter)//判断延时是否加在开头，即延时前有无读取到的指令
            {
                localCmdList->append(jtagTemp);//将前一个读取到的指令添加至链表中
                jtagTemp->setNextState(RUN_TEST_IDLE_STATE);
            }
            else
                *sign_notFirstEnter = true;
            jtagTemp->setType(DELAY);//此条指令定义为延时类
            jtagTemp->setValue(contentTemp.tdiValue);//将延时时间填到tdi指令值位置，以1ms为单位
        }
        else if(contentTemp.type.compare("#CLOCK") == 0)
        {
            if(*sign_notFirstEnter)//判断空时钟是否加在开头，即延时前有无读取到的指令
            {
                localCmdList->append(jtagTemp);//将前一个读取到的指令添加至链表中
                jtagTemp->setNextState(RUN_TEST_IDLE_STATE);
            }
            else
                *sign_notFirstEnter = true;
            jtagTemp->setType(CLOCK);//此条指令定义为空clock类
            jtagTemp->setValue(contentTemp.tdiValue);//将clock个数填到tdi指令值位置
        }
        else if(contentTemp.type.compare("#SET_EMPTY_CLOCK") == 0)
        {
            if(*sign_notFirstEnter)//判断空时钟是否加在开头，即延时前有无读取到的指令
            {
                localCmdList->append(jtagTemp);//将前一个读取到的指令添加至链表中
                jtagTemp->setNextState(RUN_TEST_IDLE_STATE);
            }
            else
                *sign_notFirstEnter = true;
            jtagTemp->setValue(contentTemp.tdiValue);
            jtagTemp->setType(SET_EMPTY_CLOCK);
        }
        else if(contentTemp.type.compare("#SET_FREQ_MULT") == 0)
        {
            if(*sign_notFirstEnter)//判断空时钟是否加在开头，即延时前有无读取到的指令
            {
                localCmdList->append(jtagTemp);//将前一个读取到的指令添加至链表中
                jtagTemp->setNextState(RUN_TEST_IDLE_STATE);
            }
            else
                *sign_notFirstEnter = true;
            jtagTemp->setValue(contentTemp.tdiValue);
            jtagTemp->setType(SET_FREQ_MULT);
        }
        else if(contentTemp.type.compare("#IDLE_WITH_TCK") == 0)
        {
            if(*sign_notFirstEnter)//判断空时钟是否加在开头，即延时前有无读取到的指令
            {
                localCmdList->append(jtagTemp);//将前一个读取到的指令添加至链表中
                jtagTemp->setNextState(RUN_TEST_IDLE_STATE);
            }
            else
                *sign_notFirstEnter = true;
            jtagTemp->setValue(contentTemp.tdiValue);
            jtagTemp->setType(SET_IDLE_WITH_TCK);
        }
        else if(contentTemp.type.compare("#SET_TDI_STATE") == 0)
        {
            if(*sign_notFirstEnter)//判断空时钟是否加在开头，即延时前有无读取到的指令
            {
                localCmdList->append(jtagTemp);//将前一个读取到的指令添加至链表中
                jtagTemp->setNextState(RUN_TEST_IDLE_STATE);
            }
            if(contentTemp.tdiValue)
                jtagTemp->setValue(1);
            else
                jtagTemp->setValue(0);
            jtagTemp->setType(SET_TDI_VALUE);
        }
        else if(contentTemp.type.compare("#SET_TDI_PLUSE") == 0)
        {
            if(*sign_notFirstEnter)//判断空时钟是否加在开头，即延时前有无读取到的指令
            {
                localCmdList->append(jtagTemp);//将前一个读取到的指令添加至链表中
                jtagTemp->setNextState(RUN_TEST_IDLE_STATE);
            }
            jtagTemp->setValue(contentTemp.tdiValue);
            jtagTemp->setType(SET_TDI_PLUSE);
        }
    }
}
int csvRead(QString filePath,QList<QStringList>& data)
{
    QFile inFile(filePath);
    if(!inFile.open(QIODevice::ReadOnly))
        return -1;

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");   // 使用UTF-8编码
    QByteArray content = inFile.readAll();
    QString text = codec->toUnicode(content);
    QStringList lines = text.split('\n');

    for (int i = 0; i < lines.size(); i++)
    {
        if (!lines.at(i).isEmpty())
        {
            QStringList row = lines.at(i).split(',');
            data.append(row);
        }
    }
    inFile.close();
    return 0;
}
int excelRead(QString filePath,ExcelBase *excelHandle,JtagList* jtagCmdList,LongCmdList* longCmdList)
{
    QFile src(filePath);
    if(src.open(QIODevice::ReadOnly))
        src.close();
    else
        return -1;
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

    cmdReadIn(excelData,length,jtagCmdList,longCmdList);

    excelHandle->workBooks->dynamicCall("Close(Boolean)",false);
    return 0;
}
int cmdReadIn(QList<QList<QVariant>> excelData,int length,JtagList* localCmdList,LongCmdList* localLongCmdList)
{
    int typeColumn=0,tdiColumn=0,countColumn=0,tapColumn = 0,i=0;//分别存储所需数据在excel中的列数
    QString str;//读取单元格的字符
    jtagCmdContentTemp contentTemp;
    int rowLength = excelData[0].length();
    for (i = 0; i < rowLength; i++)
    {
        str = excelData[0][i].toString();

        if(str.contains("state",Qt::CaseInsensitive))
            typeColumn = i;
        else if(str.contains("bitCount",Qt::CaseInsensitive))
            countColumn = i;
        else if(str.contains("TDI",Qt::CaseInsensitive))
            tdiColumn = i;
        else if(str.contains("tap",Qt::CaseInsensitive))
            tapColumn = i;
        else
            continue;
    }
    if(countColumn == 0)//excel中没有bitCount栏
    {
        return -1;//
    }

    JtagTemp* jtagTemp = new JtagTemp();
    bool sign_notFirstEnter = false;//目的是用下一条的指令更改当前指令的JTAG状态机
    for(i = 1;i < length;i++)
    {
        getCmdFromList(excelData,i,typeColumn,tdiColumn,countColumn,tapColumn,&contentTemp);
        jtagListAppendCmd(contentTemp,jtagTemp,&sign_notFirstEnter,localCmdList,localLongCmdList);
    }
    localCmdList->append(jtagTemp);//添加最后一条指令至链表中
    //jtag命令链表生成
    delete(jtagTemp);
    return 0;//文件导入完成
}
int csvCmdReadIn(QList<QStringList>& data,int length,JtagList* localCmdList,LongCmdList* localLongCmdList)
{
    int typeColumn=0,tdiColumn=0,countColumn=0,tapColumn = 0,i=0;//分别存储所需数据在excel中的列数
    QString str;//读取单元格的字符
    jtagCmdContentTemp contentTemp;
    int rowLength = data[0].length();
    for (i = 0; i < rowLength; i++)
    {
        str = data[0][i];

        if(str.contains("state",Qt::CaseInsensitive))
            typeColumn = i;
        else if(str.contains("bitCount",Qt::CaseInsensitive))
            countColumn = i;
        else if(str.contains("TDI",Qt::CaseInsensitive))
            tdiColumn = i;
        else if(str.contains("tap",Qt::CaseInsensitive))
            tapColumn = i;
        else
            continue;
    }
    if(countColumn == 0)//excel中没有bitCount栏
    {
        return -1;//
    }

    JtagTemp* jtagTemp = new JtagTemp();
    bool sign_notFirstEnter = false;//目的是用下一条的指令更改当前指令的JTAG状态机
    bool emptyFile = true;
    for(i = 1;i < length;i++)
    {
        getCmdFromStrList(data,i,typeColumn,tdiColumn,countColumn,tapColumn,&contentTemp);
        jtagListAppendCmd(contentTemp,jtagTemp,&sign_notFirstEnter,localCmdList,localLongCmdList);
        emptyFile = false;
    }
    if(!emptyFile)
        localCmdList->append(jtagTemp);//添加最后一条指令至链表中
    //jtag命令链表生成
    delete(jtagTemp);
    data.clear();
    return 0;//文件导入完成
}
void fetchJtagCmd(ExcelBase *excelHandle,QString filePath,JtagList* localCmdList,LongCmdList* localLongCmdList)
{
    /*****清空链表*******/
    if(localCmdList->length)
    {
        localCmdList->removeAll();
    }
    localCmdList->current = localCmdList->head;
    /******打开excel******/
    excelHandle->workBooks->dynamicCall("Open(const QString&)", filePath); //打开打开已存在的工作簿
    excelHandle->workBook = excelHandle->excel->querySubObject("ActiveWorkBook"); //获取当前工作簿
    excelHandle->workSheet = excelHandle->workBook->querySubObject("WorkSheets(int)", 1);//获取工作表集合的工作表1，即sheet1

    /*********导出excel数据***********/
    QVariant values;//用于读取单元格的值
    QString str;//读取单元格的字符
    QList<QList<QVariant> > excelData;//存储整个excel表

    values = excelHandle->readAll();
    excelHandle->castVariant2ListListVariant(values,excelData);
    int length = excelData.length();
    /*************处理数据*************/
    /*此处数据格式：第0列：指令格式（SHIFT-IR 或者SHIFT-DR）
                 第1列：TDI
                 第2列：bitCount
                 第3列：是否为tap相关指令（1：tap相关，0：无关）*/
    cmdReadIn(excelData,length,localCmdList,localLongCmdList);

    excelHandle->workBooks->dynamicCall("Close(Boolean)",false);
    return;
}

