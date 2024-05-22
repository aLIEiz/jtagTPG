#include "config.h"
#include "jtag.h"
#include "mainwindow.h"
#include <QFile>
#include <QSettings>
#include <QDir>
#include <QTextStream>

QString gDataLocalInHex[5] = {"DATA0_OFFSET_IN_HEX","DATA1_OFFSET_IN_HEX","DATA2_OFFSET_IN_HEX","DATA3_OFFSET_IN_HEX","DATA4_OFFSET_IN_HEX"};
QString gDataByteCountInHex[5] = {"DATA0_BYTE_COUNT_IN_HEX","DATA1_BYTE_COUNT_IN_HEX","DATA2_BYTE_COUNT_IN_HEX","DATA3_BYTE_COUNT_IN_HEX","DATA4_BYTE_COUNT_IN_HEX"};
QString gCoreCfgName[20] = {"CORE0_CFG","CORE1_CFG","CORE2_CFG","CORE3_CFG","CORE4_CFG","CORE5_CFG","CORE6_CFG","CORE7_CFG","CORE8_CFG","CORE9_CFG","CORE10_CFG","CORE11_CFG","CORE12_CFG","CORE13_CFG","CORE14_CFG","CORE15_CFG","CORE16_CFG","CORE17_CFG","CORE18_CFG","CORE19_CFG"};
QString gSeqName[20] = {"SEQ0","SEQ1","SEQ2","SEQ3","SEQ4","SEQ5","SEQ6","SEQ7","SEQ8","SEQ9","SEQ10","SEQ11","SEQ12","SEQ13","SEQ14","SEQ15","SEQ16","SEQ17","SEQ18","SEQ19"};
QString gDmaRplaceData[20] = {"DMA_REPLACE_DATA0","DMA_REPLACE_DATA1","DMA_REPLACE_DATA2","DMA_REPLACE_DATA3","DMA_REPLACE_DATA4","DMA_REPLACE_DATA5","DMA_REPLACE_DATA6","DMA_REPLACE_DATA7","DMA_REPLACE_DATA8","DMA_REPLACE_DATA9"};
QString gDmaRplaceLocal[20] = {"DMA_REPLACE_BIT_LOCAL0","DMA_REPLACE_BIT_LOCAL1","DMA_REPLACE_BIT_LOCAL2","DMA_REPLACE_BIT_LOCAL3","DMA_REPLACE_BIT_LOCAL4","DMA_REPLACE_BIT_LOCAL5","DMA_REPLACE_BIT_LOCAL6","DMA_REPLACE_BIT_LOCAL7","DMA_REPLACE_BIT_LOCAL8","DMA_REPLACE_BIT_LOCAL9"};
QString gDmaRplaceCount[20] = {"DMA_REPLACE_BIT_COUNT0","DMA_REPLACE_BIT_COUNT1","DMA_REPLACE_BIT_COUNT2","DMA_REPLACE_BIT_COUNT3","DMA_REPLACE_BIT_COUNT4","DMA_REPLACE_BIT_COUNT5","DMA_REPLACE_BIT_COUNT6","DMA_REPLACE_BIT_COUNT7","DMA_REPLACE_BIT_COUNT8","DMA_REPLACE_BIT_COUNT9"};
QString gCfgListName[20] = {"CFG_FILE_PATH0","CFG_FILE_PATH1","CFG_FILE_PATH2","CFG_FILE_PATH3","CFG_FILE_PATH4","CFG_FILE_PATH5","CFG_FILE_PATH6","CFG_FILE_PATH7","CFG_FILE_PATH8","CFG_FILE_PATH9","CFG_FILE_PATH10","CFG_FILE_PATH11","CFG_FILE_PATH12","CFG_FILE_PATH13","CFG_FILE_PATH14","CFG_FILE_PATH15","CFG_FILE_PATH16","CFG_FILE_PATH17","CFG_FILE_PATH18","CFG_FILE_PATH19"};
QString gCfgName[20] = {"CFG_NAME0","CFG_NAME1","CFG_NAME2","CFG_NAME3","CFG_NAME4","CFG_NAME5","CFG_NAME6","CFG_NAME7","CFG_NAME8","CFG_NAME9","CFG_NAME10","CFG_NAME11","CFG_NAME12","CFG_NAME13","CFG_NAME14","CFG_NAME15","CFG_NAME16","CFG_NAME17","CFG_NAME18","CFG_NAME19"};
QString gErrCfgName[20] = {"","c","cc","ccc","cccc","ccccc","cccccc","ccccccc","cccccccc","ccccccccc","cccccccccc","ccccccccccc","cccccccccccc","ccccccccccccc","cccccccccccccc","ccccccccccccccc","cccccccccccccccc","ccccccccccccccccc","cccccccccccccccccc","ccccccccccccccccccc"};
QString gGeneralReplaceSeqName[5] = {"GENERAL0_REPLACE_SEQ","GENERAL1_REPLACE_SEQ","GENERAL2_REPLACE_SEQ","GENERAL3_REPLACE_SEQ","GENERAL4_REPLACE_SEQ"};
QString gGeneralReplaceData[5] = {"GENERAL0_REPLACE_DATA","GENERAL1_REPLACE_DATA","GENERAL2_REPLACE_DATA","GENERAL3_REPLACE_DATA","GENERAL4_REPLACE_DATA"};
QString gGeneralReplaceCount[5] = {"GENERAL0_REPLACE_COUNT","GENERAL1_REPLACE_COUNT","GENERAL2_REPLACE_COUNT","GENERAL3_REPLACE_COUNT","GENERAL4_REPLACE_COUNT"};
QString gGeneralReplaceSign[5] = {"GENERAL0_REPLACE_SIGN","GENERAL1_REPLACE_SIGN","GENERAL2_REPLACE_SIGN","GENERAL3_REPLACE_SIGN","GENERAL4_REPLACE_SIGN"};
QString gGeneralReplaceDataSign[5] = {"HEX_DATA0","HEX_DATA1","HEX_DATA2","HEX_DATA3","HEX_DATA4"};
QString gGeneralRplaceBitCount[5][5] = {
    {"GENERAL0_REPLACE_BIT_COUNT0","GENERAL0_REPLACE_BIT_COUNT1","GENERAL0_REPLACE_BIT_COUNT2","GENERAL0_REPLACE_BIT_COUNT3","GENERAL0_REPLACE_BIT_COUNT4"},
    {"GENERAL1_REPLACE_BIT_COUNT0","GENERAL1_REPLACE_BIT_COUNT1","GENERAL1_REPLACE_BIT_COUNT2","GENERAL1_REPLACE_BIT_COUNT3","GENERAL1_REPLACE_BIT_COUNT4"},
    {"GENERAL2_REPLACE_BIT_COUNT0","GENERAL2_REPLACE_BIT_COUNT1","GENERAL2_REPLACE_BIT_COUNT2","GENERAL2_REPLACE_BIT_COUNT3","GENERAL2_REPLACE_BIT_COUNT4"},
    {"GENERAL3_REPLACE_BIT_COUNT0","GENERAL3_REPLACE_BIT_COUNT1","GENERAL3_REPLACE_BIT_COUNT2","GENERAL3_REPLACE_BIT_COUNT3","GENERAL3_REPLACE_BIT_COUNT4"},
    {"GENERAL4_REPLACE_BIT_COUNT0","GENERAL4_REPLACE_BIT_COUNT1","GENERAL4_REPLACE_BIT_COUNT2","GENERAL4_REPLACE_BIT_COUNT3","GENERAL4_REPLACE_BIT_COUNT4"}
};
QString gGeneralRplaceBitLocal[5][5] = {
    {"GENERAL0_REPLACE_BIT_LOCAL0","GENERAL0_REPLACE_BIT_LOCAL1","GENERAL0_REPLACE_BIT_LOCAL2","GENERAL0_REPLACE_BIT_LOCAL3","GENERAL0_REPLACE_BIT_LOCAL4"},
    {"GENERAL1_REPLACE_BIT_LOCAL0","GENERAL1_REPLACE_BIT_LOCAL1","GENERAL1_REPLACE_BIT_LOCAL2","GENERAL1_REPLACE_BIT_LOCAL3","GENERAL1_REPLACE_BIT_LOCAL4"},
    {"GENERAL2_REPLACE_BIT_LOCAL0","GENERAL2_REPLACE_BIT_LOCAL1","GENERAL2_REPLACE_BIT_LOCAL2","GENERAL2_REPLACE_BIT_LOCAL3","GENERAL2_REPLACE_BIT_LOCAL4"},
    {"GENERAL3_REPLACE_BIT_LOCAL0","GENERAL3_REPLACE_BIT_LOCAL1","GENERAL3_REPLACE_BIT_LOCAL2","GENERAL3_REPLACE_BIT_LOCAL3","GENERAL3_REPLACE_BIT_LOCAL4"},
    {"GENERAL4_REPLACE_BIT_LOCAL0","GENERAL4_REPLACE_BIT_LOCAL1","GENERAL4_REPLACE_BIT_LOCAL2","GENERAL4_REPLACE_BIT_LOCAL3","GENERAL4_REPLACE_BIT_LOCAL4"}
};


cfgTemp::cfgTemp()
{
    next = nullptr;
    prev = nullptr;
    //[MAIN]
    cfgName = "undefinedName";
    libPath = gDefaultFilePath;
    resultFileAutoSave = false;
    //[CFG]
    coreCount = 0;
    currentCore = 0;
    codeFormate = ATE_CODE;
    for(int i = 0;i<20;i++)
    {
        coreCfg[i].withIcepick = false;
        coreCfg[i].useDma = true;

        coreCfg[i].ramAddressMode = ram8bitMode;
        coreCfg[i].ramAdressAlignMode = ram32bitAlian;
        coreCfg[i].pcFromMap = false;
        coreCfg[i].pcOffsetInCoff = 24;
        coreCfg[i].dataEndian = LITTLE_ENDIAN;
        coreCfg[i].irEndian = LITTLE_ENDIAN;
        coreCfg[i].drEndian = LITTLE_ENDIAN;
        coreCfg[i].pcInTxt="";
        coreCfg[i].sectionAddrSub=0;
        coreCfg[i].sectionAddrDiffInByte=0;
        coreCfg[i].codeForEmuWithTck = true;

        coreCfg[i].parserName = "default";
        coreCfg[i].parserSrcCmd = "";
        coreCfg[i].parserDestCmd = "";
        coreCfg[i].autoRefreshSourceFile = true;
        //[CODE]
        coreCfg[i].seqCount = 0;

        coreCfg[i].dmaReplaceCount = 0;
        coreCfg[i].dmaLoadAddrReplaceSign = "0x55AA";
        for(int i = 0;i<5;i++)
        {
            coreCfg[i].dmaReplaceData[i] = DMA_ADDR;
            coreCfg[i].dmaReplaceBitLocal[i] = 0;
            coreCfg[i].dmaReplaceBitCount[i] = 32;
        }
        coreCfg[i].generalReplaceSeqCount = 0;
        coreCfg[i].dmaMaxLengthInOneSeq = 0;
        //[CMD]
        coreCfg[i].coreCmdLength = 44;
        coreCfg[i].dmaDataLength = 32;
        coreCfg[i].dmaCmdHead = 0;
        coreCfg[i].regWriteAddrHead = 0;
        coreCfg[i].regWriteDataHead = 0;
        coreCfg[i].dataLeftShiftBitCount=0;
        coreCfg[i].addrLeftShiftBitCount=0;
        coreCfg[i].dmaDataInstructionTapState = IR;
        //[VAR]
        coreCfg[i].jtagEmptyClockBetweenCmd = 12;
        coreCfg[i].jtagFreqMult = 1;
        coreCfg[i].idleWithTck = true;
        coreCfg[i].tdiIdleState = 0;
        coreCfg[i].tdiInsertPluseInIdle = 0;
    }
}
void cfgTemp::operator=(const cfgTemp& b)
{
    uint32_t i;
    this->prev = b.prev;
    this->next = b.next;
    //[MAIN]
    this->cfgName = b.cfgName;
    this->libPath = b.libPath;
    this->resultFileAutoSave = b.resultFileAutoSave;
    this->resultFileAutoSavePath = b.resultFileAutoSavePath;
    this->codeFormate = b.codeFormate;
    //[CFG]
    this->coreCount = b.coreCount;
    this->currentCore = b.currentCore;
    for(i = 0;i<20;i++)
        this->coreCfg[i] = b.coreCfg[i];

}
cfgList::cfgList()
{
    head = nullptr;
    tail = head;
    current = head;
    length = 0;
}
void cfgList::operator=(const cfgList b)
{
    this->removeAll();
    cfgTemp* temp = b.head;
    while(temp != nullptr)
    {
        this->append(temp);
        temp = temp->next;
    }
}
cfgList::cfgList(const cfgTemp* temp)
{
    cfgTemp* p = new cfgTemp;
    *p = *temp;
    head = p;
    tail = p;
    current = p;
    length=1;
    p = nullptr;
}
void cfgList::append(const cfgTemp* temp)
{
    cfgTemp* p = new cfgTemp;
    *p = *temp;
    if(this->head == nullptr)//空表的情况
    {
        this->head = p;
        this->tail = p;
        this->current = p;
        this->length = 1;
    }
    else
    {
        tail->next = p;
        p->prev = tail;
        tail = p;
        length++;
    }
    p = nullptr;
}
void cfgList::remove()
{
    cfgTemp* p;
    if(this->head == this->tail)//最后一个
    {
        p = this->tail;
        tail = nullptr;
        head = nullptr;
        length = 0;
    }
    else
    {
        p = tail;
        tail = tail->prev;
        tail->next = nullptr;
        length --;
    }
    delete(p);
    p = nullptr;
}
void cfgList::remove(cfgTemp* temp)
{
    if(this->head == temp)//第一个
    {
        if(this->current == this->head)//如果currrent指针指向头，则current也应改变
        {
            this->head = temp->next;
            this->current = this->head;
        }
        else
            this->head = temp->next;

        if(this->length>1)
            this->head->prev = nullptr;
        else if(this->length==1)//既是第一个，也是最后一个
            this->tail = this->head;
        length--;
    }
    else if(this->tail == temp)//最后一个
    {
        this->tail = temp->prev;
        if(this->length>1)
            this->tail->next = nullptr;
        length--;
    }
    else
    {
        temp->next->prev = temp->prev;
        temp->prev->next = temp->next;
        length --;
    }
    delete(temp);
    temp = nullptr;
}
void cfgList::removeAll()
{
    current = nullptr;
    while(this->tail)
    {
        remove();
    }
}
cfgTemp* matchCfg(QString itemName)
{
    while(gCfgList.current)
    {
        if(QString::compare(gCfgList.current->cfgName,itemName,Qt::CaseSensitive) == 0)
        {
            cfgTemp* temp = gCfgList.current;
            gCfgList.current = gCfgList.head;
            return temp;
        }
        else
        {
            gCfgList.current = gCfgList.current->next;
        }
    }
    gCfgList.current = gCfgList.head;
    return nullptr;
}

void delCfgFromList(QString itemName)
{
    cfgTemp* delCfg = matchCfg(itemName);
    gCfgList.remove(delCfg);
    gItemNum--;
}

void cfgNameFilter(QString* cfgName)
{
    while(matchCfg(*cfgName))//有重名的文件
    {
        cfgName->append('c');
    }
}
int coreCfgFileRead(Uint32 tapNum,QString filePath,coreCfgTemp *coreCfg)
{
    uint32_t temp=0,i=0;
    coreCfg->generalReplaceSeqCount = 0;
    bool ok;
    QFile cfgFile(filePath);
    if(!cfgFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        return -1;}

    int index = filePath.lastIndexOf("/")+1;
    coreCfg->coreName = filePath.right(filePath.length()-index);
    coreCfg->coreName.remove(".ini");
    coreCfg->coreLibPath = filePath.left(index);

    coreCfg->tapNum = tapNum;

    QSettings *settings = new QSettings(filePath,QSettings::IniFormat);

    settings->beginGroup("MAIN");
    if(!settings->value("HEAD_FILE_FOR_ATE","").toString().isEmpty())
        coreCfg->ateHeadFileName = settings->value("HEAD_FILE_FOR_ATE","").toString();
    if(!settings->value("HEAD_FILE_FOR_TEST","").toString().isEmpty())
        coreCfg->testHeadFileName = settings->value("HEAD_FILE_FOR_TEST","").toString();
    if(!settings->value("HEAD_FILE_FOR_EMU","").toString().isEmpty())
        coreCfg->emuHeadFileName = settings->value("HEAD_FILE_FOR_EMU","").toString();
    settings->endGroup();

    settings->beginGroup("CFG");
    if(!settings->value("WITH_ICEPICK","").toString().isEmpty())
        coreCfg->withIcepick = settings->value("WITH_ICEPICK","").toBool();
    if(!settings->value("USE_DMA","").toString().isEmpty())
        coreCfg->useDma = settings->value("USE_DMA","").toBool();

    if(!settings->value("ADDRESS_BIT_WIDTH","").toString().isEmpty())
    {
        temp = settings->value("ADDRESS_BIT_WIDTH","").toUInt();
        switch (temp)
        {
        case 8:coreCfg->ramAddressMode = ram8bitMode; break;
        case 16:coreCfg->ramAddressMode = ram16bitMode;break;
        case 32:coreCfg->ramAddressMode = ram32bitMode;break;
        case 64:coreCfg->ramAddressMode = ram64bitMode;break;
        default:coreCfg->ramAddressMode = ram8bitMode;break;
        }
    }
    if(!settings->value("ADDRESS_ALIGN_BIT_WIDTH","").toString().isEmpty())
    {
        temp = settings->value("ADDRESS_ALIGN_BIT_WIDTH","").toUInt();
        switch (temp)
        {
        case 8:coreCfg->ramAdressAlignMode = ram8bitAlian; break;
        case 16:coreCfg->ramAdressAlignMode = ram16bitAlian;break;
        case 32:coreCfg->ramAdressAlignMode = ram32bitAlian;break;
        case 64:coreCfg->ramAdressAlignMode = ram64bitAlian;break;
        default:coreCfg->ramAdressAlignMode = ram32bitAlian;break;
        }
    }
    if(!settings->value("PC_FROM_MAP","").toString().isEmpty())
        coreCfg->pcFromMap = settings->value("PC_FROM_MAP","").toBool();
    if(!settings->value("PC_OFFSET_IN_COFF","").toString().isEmpty())
        coreCfg->pcOffsetInCoff = settings->value("PC_OFFSET_IN_COFF","").toUInt();
    if(!settings->value("PC_IN_TXT","").toString().isEmpty())
        coreCfg->pcInTxt = settings->value("PC_IN_TXT","").toString();
    if(!settings->value("SECTION_ADDRESS_SUBTRACT","").toString().isEmpty())
        coreCfg->sectionAddrSub = settings->value("SECTION_ADDRESS_SUBTRACT","").toUInt();
    if(!settings->value("SECTION_ADDRESS_DIFF_IN_BYTE","").toString().isEmpty())
    {
        coreCfg->sectionAddrDiffInByte = settings->value("SECTION_ADDRESS_DIFF_IN_BYTE","").toInt();
        if(coreCfg->sectionAddrDiffInByte<0)
            coreCfg->sectionAddrDiffInByte=0;
    }
    if(!settings->value("DATA_ENDIAN","").toString().isEmpty())
    {
        if(settings->value("DATA_ENDIAN","").toString().contains("big",Qt::CaseInsensitive))
            coreCfg->dataEndian = BIG_ENDIAN;
        else
            coreCfg->dataEndian = LITTLE_ENDIAN;
    }
    if(!settings->value("IR_ENDIAN","").toString().isEmpty())
    {
        if(settings->value("IR_ENDIAN","").toString().contains("big",Qt::CaseInsensitive))
            coreCfg->irEndian = BIG_ENDIAN;
        else
            coreCfg->irEndian = LITTLE_ENDIAN;
    }
    if(!settings->value("DR_ENDIAN","").toString().isEmpty())
    {
        if(settings->value("DR_ENDIAN","").toString().contains("big",Qt::CaseInsensitive))
            coreCfg->drEndian = BIG_ENDIAN;
        else
            coreCfg->drEndian = LITTLE_ENDIAN;
    }
    for(i = 0;i<5;i++)
    {
        if(!settings->value(gDataLocalInHex[i],"").toString().isEmpty())
        {
            coreCfg->dataOffsetInHex[i] = settings->value(gDataLocalInHex[i],"").toUInt();
            coreCfg->dataByteCountInHex[i] = settings->value(gDataByteCountInHex[i],"").toUInt();
        }
    }

    if(!settings->value("CODE_FOR_EMU_WITH_TCK","").toString().isEmpty())
        coreCfg->codeForEmuWithTck = settings->value("CODE_FOR_EMU_WITH_TCK","").toBool();

    if(!settings->value("PARSER_NAME","").toString().isEmpty())
        coreCfg->parserName = settings->value("PARSER_NAME","").toString();
    if(!settings->value("PARSER_SRC_CMD","").toString().isEmpty())
        coreCfg->parserSrcCmd = settings->value("PARSER_SRC_CMD","").toString();
    if(!settings->value("PARSER_DEST_CMD","").toString().isEmpty())
        coreCfg->parserDestCmd = settings->value("PARSER_DEST_CMD","").toString();
    if(!settings->value("AUTO_REFRESH_SOURCE_FILE","").toString().isEmpty())
        coreCfg->autoRefreshSourceFile = settings->value("AUTO_REFRESH_SOURCE_FILE","").toBool();

    settings->endGroup();

    settings->beginGroup("CODE");
    if(!settings->value("SEQ_COUNT","").toString().isEmpty())
        coreCfg->seqCount = settings->value("SEQ_COUNT","").toUInt();
    for(i =0;i<coreCfg->seqCount;i++)
        coreCfg->seqName[i] = settings->value(gSeqName[i],"").toString();
    if(!settings->value("DMA_START_SEQ","").toString().isEmpty())
        coreCfg->dmaStartSeq = settings->value("DMA_START_SEQ","").toString();
    if(!settings->value("DMA_STOP_SEQ","").toString().isEmpty())
        coreCfg->dmaStopSeq = settings->value("DMA_STOP_SEQ","").toString();

    if(!settings->value("DMA_REPLACE_COUNT","").toString().isEmpty())
        coreCfg->dmaReplaceCount = settings->value("DMA_REPLACE_COUNT","").toUInt();
    if(!settings->value("DMA_ADDRESS_REPLACE_SIGN","").toString().isEmpty())
        coreCfg->dmaLoadAddrReplaceSign = settings->value("DMA_ADDRESS_REPLACE_SIGN","").toString();
    for(i =0;i<coreCfg->dmaReplaceCount;i++)
    {
        if(!settings->value(gDmaRplaceData[i],"").toString().isEmpty())
        {
            QString temp = settings->value(gDmaRplaceData[i],"").toString();
            if(temp.compare("DMA_ADDR") == 0)
                coreCfg->dmaReplaceData[i] = DMA_ADDR;
            else if(temp.contains("PROGRAM_DATA"))
                 coreCfg->dmaReplaceData[i] = temp.rightRef(1).toUInt() + 1;
            else
                coreCfg->dmaReplaceData[i] = DMA_ADDR;
        }
        else
        {
            coreCfg->dmaReplaceData[i] = DMA_ADDR;
        }

        if(!settings->value(gDmaRplaceLocal[i],"").toString().isEmpty())
            coreCfg->dmaReplaceBitLocal[i] = settings->value(gDmaRplaceLocal[i],"").toUInt();
        else
            coreCfg->dmaReplaceBitLocal[i] = 0;
        if(!settings->value(gDmaRplaceCount[i],"").toString().isEmpty())
            coreCfg->dmaReplaceBitCount[i] = settings->value(gDmaRplaceCount[i],"").toUInt();
        else
            coreCfg->dmaReplaceBitCount[i] = 32;
    }
    for(i=0;i<5;i++)
    {
        if(!settings->value(gGeneralReplaceSeqName[i],"").toString().isEmpty())
        {
            coreCfg->generalReplaceSeqCount++;
            coreCfg->generalReplaceSeq[i].seqName = settings->value(gGeneralReplaceSeqName[i],"").toString();
            coreCfg->generalReplaceSeq[i].generalReplaceData = settings->value(gGeneralReplaceData[i],"").toString();
            coreCfg->generalReplaceSeq[i].generalReplaceCount = settings->value(gGeneralReplaceCount [i],"").toUInt();
            coreCfg->generalReplaceSeq[i].generalAddrReplaceSign = settings->value(gGeneralReplaceSign[i],"").toString();
            for(Uint32 j =0;j<coreCfg->generalReplaceSeq[i].generalReplaceCount;j++)
            {
                coreCfg->generalReplaceSeq[i].generalReplaceBitLocal[j] = settings->value(gGeneralRplaceBitLocal[i][j],"").toUInt();
                coreCfg->generalReplaceSeq[i].generalReplaceBitCount[j] = settings->value(gGeneralRplaceBitCount[i][j],"").toUInt();
            }
        }
    }
    if(!settings->value("DMA_MAX_LENGTH_IN_ONE_SEQ","").toString().isEmpty())
        coreCfg->dmaMaxLengthInOneSeq = settings->value("DMA_MAX_LENGTH_IN_ONE_SEQ","").toString().toULongLong(&ok,16);

    settings->endGroup();

    settings->beginGroup("CMD");
    if(!settings->value("CORE_CMD_LENGTH","").toString().isEmpty())
        coreCfg->coreCmdLength = settings->value("CORE_CMD_LENGTH","").toUInt();
    if(!settings->value("DMA_DATA_LENGTH","").toString().isEmpty())
        coreCfg->dmaDataLength = settings->value("DMA_DATA_LENGTH","").toUInt();
    if(!settings->value("DMA_CMD_HEAD","").toString().isEmpty())
        coreCfg->dmaCmdHead = settings->value("DMA_CMD_HEAD","").toString().toULongLong(&ok,16);
    if(!settings->value("REG_WRITE_ADDRESS_HEAD","").toString().isEmpty())
        coreCfg->regWriteAddrHead = settings->value("REG_WRITE_ADDRESS_HEAD","").toString().toULongLong(&ok,16);
    if(!settings->value("REG_WRITE_DATA_HEAD","").toString().isEmpty())
        coreCfg->regWriteDataHead = settings->value("REG_WRITE_DATA_HEAD","").toString().toULongLong(&ok,16);
    if(!settings->value("DATA_LEFT_SHIFT_COUNT","").toString().isEmpty())
        coreCfg->dataLeftShiftBitCount = settings->value("DATA_LEFT_SHIFT_COUNT","").toString().toUInt();
    if(!settings->value("ADDRESS_LEFT_SHIFT_COUNT","").toString().isEmpty())
        coreCfg->addrLeftShiftBitCount = settings->value("ADDRESS_LEFT_SHIFT_COUNT","").toString().toUInt();
    if(!settings->value("DMA_DATA_INSTRUCTION_TAP_STATE","").toString().isEmpty())
        coreCfg->dmaDataInstructionTapState = settings->value("DMA_DATA_INSTRUCTION_TAP_STATE","").toString().contains("IR",Qt::CaseInsensitive)?IR:DR;
    settings->endGroup();

    settings->beginGroup("VAR");
    if(!settings->value("JTAG_EMPTY_CLOCK_BETWEEN_CMD","").toString().isEmpty())
        coreCfg->jtagEmptyClockBetweenCmd = settings->value("JTAG_EMPTY_CLOCK_BETWEEN_CMD","").toUInt();
    if(!settings->value("JTAG_FREQ_MULT","").toString().isEmpty())
        coreCfg->jtagFreqMult = settings->value("JTAG_FREQ_MULT","").toUInt();
    if(!settings->value("IDLE_WITH_TCK","").toString().isEmpty())
        coreCfg->idleWithTck = settings->value("IDLE_WITH_TCK","").toBool();
    if(!settings->value("TDI_IDLE_STATE","").toString().isEmpty())
    {
        coreCfg->tdiIdleState = settings->value("TDI_IDLE_STATE","").toUInt();
        if(coreCfg->tdiIdleState>=1)
            coreCfg->tdiIdleState=1;
        else
            coreCfg->tdiIdleState=0;
    }
    if(!settings->value("TDI_INSERT_PULSE_IN_IDLE","").toString().isEmpty())
        coreCfg->tdiInsertPluseInIdle = settings->value("TDI_INSERT_PULSE_IN_IDLE","").toUInt();
    settings->endGroup();
    cfgFile.close();
    return 0;
}
int cfgFileRead(QString filePath,cfgTemp& mCfgTemp)//-1:选取文件打不开 -2：总cfg中配置的路径有问题 -3:无效的配置文件
{
    uint32_t i;
    QString pathTemp,temp;
    QFile cfgFile(filePath);
    mCfgTemp.coreCount = 0;
    if(!cfgFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        return -1;
    }

    QSettings *settings = new QSettings(filePath,QSettings::IniFormat);
    int index = filePath.lastIndexOf("/");
    mCfgTemp.cfgName = filePath.right(filePath.length()-index-1);
    mCfgTemp.cfgName.remove(".ini");
    //Read
    if(!settings->value("LIB_PATH","").toString().isEmpty())
        mCfgTemp.libPath = settings->value("LIB_PATH","").toString();
    else
        mCfgTemp.libPath ="";

    mCfgTemp.libPath.prepend(gDefaultFilePath);

    if(!settings->value("RESULT_FILE_AUTO_SAVE_PATH","").toString().isEmpty())
    {
        mCfgTemp.resultFileAutoSave = true;
        mCfgTemp.resultFileAutoSavePath = settings->value("RESULT_FILE_AUTO_SAVE_PATH","").toString();
        if(mCfgTemp.resultFileAutoSavePath.contains("defaultPath",Qt::CaseInsensitive))
        {
            mCfgTemp.resultFileAutoSavePath.replace("defaultPath",gDefaultFilePath);
        }
    }
    else
        mCfgTemp.resultFileAutoSave = false;

    if((!settings->value("CODE_FOR_ATE","").toString().isEmpty())&&(settings->value("CODE_FOR_ATE","").toBool()))
        mCfgTemp.codeFormate = ATE_CODE;
    if((!settings->value("CODE_FOR_TEST","").toString().isEmpty())&&(settings->value("CODE_FOR_TEST","").toBool()))
        mCfgTemp.codeFormate = TEST_CODE;
    if((!settings->value("CODE_FOR_EMU","").toString().isEmpty())&&(settings->value("CODE_FOR_EMU","").toBool()))
        mCfgTemp.codeFormate = EMU_CODE;

    for(i=0;i<20;i++)
    {
        if(!settings->value(gCoreCfgName[i],"").toString().isEmpty())
        {
            pathTemp = settings->value(gCoreCfgName[i],"").toString();
            pathTemp.prepend(mCfgTemp.libPath);//获得子核配置文件的路径
            if(coreCfgFileRead(i,pathTemp,&mCfgTemp.coreCfg[mCfgTemp.coreCount])==-1)//读取核的配置文件
                return -2;
            mCfgTemp.coreCount++;
        }
    }
    if(mCfgTemp.coreCount==0)//无效的配置文件
    {
        return -3;
    }
    if(!settings->value("CURRENT_CORE","").toString().isEmpty())
    {
        temp = settings->value("CURRENT_CORE","").toString().remove("core",Qt::CaseInsensitive);//获得tap num
        for(i=0;i<mCfgTemp.coreCount;i++)
            if(temp.toUInt() == mCfgTemp.coreCfg[i].tapNum)
            {
                mCfgTemp.currentCore = i;
                break;
            }
    }
    else
        mCfgTemp.currentCore = 0;

    cfgFile.close();

    return 0;
}
void cfgScan(MainWindow* window,QString path)
{
    // 判断路径是否存在
    QDir dir(path);
    if(!dir.exists())
        return;
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();

    int fileCount = list.count();
    if(fileCount <= 0)
        return;

    QStringList fileList;
    for(int i=0; i<fileCount; i++)
    {
        QFileInfo fileInfo = list.at(i);
        QString suffix = fileInfo.suffix();
        if(QString::compare(suffix, QString("ini"), Qt::CaseInsensitive) == 0)
        {
            QString filePath = fileInfo.absoluteFilePath();
            fileList.append(filePath);
        }
    }
    cfgTemp mCfgTemp;
    for(int i =0;i<fileList.length();i++)
        if(!fileList[i].contains("log"))
        {
            cfgFileRead(fileList[i],mCfgTemp);
            if(matchCfg(mCfgTemp.cfgName)==nullptr)
            {
                gCfgList.append(&mCfgTemp);
                treeItemAdd(window,mCfgTemp,gItemNum,"NULL");
                logFileAppend(mCfgTemp,fileList[i]);
                gItemNum++;
            }
        }
}





void filePathDecode(QString &fileName)
{
    if(fileName.contains("defaultPath",Qt::CaseInsensitive))
    {
        fileName.replace("defaultPath",gDefaultFilePath);
    }
}
void filePathEncode(QString &fileName)
{
    if(fileName.contains(gDefaultFilePath))
    {
        fileName.replace(gDefaultFilePath,"defaultPath");
    }
}
void logFileGetInfo(QString logFileName,logCfg& mLogCfg)
{
    QSettings *settings = new QSettings(logFileName,QSettings::IniFormat);
    //Read
    settings->beginGroup("LOCAL");
    mLogCfg.strFileName = settings->value("CURRENT_PATH","").toString();
    mLogCfg.cfgFileScan = settings->value("CFG_FILE_SCAN","").toString();
    mLogCfg.itemNum = settings->value("ITEM_NUM","").toInt();
    mLogCfg.currentCfgName = settings->value("CURRENT_CFG_NAME","").toString();
    for(int i=0;i<mLogCfg.itemNum;i++)
    {
        mLogCfg.cfgName[i] = settings->value(gCfgName[i]).toString();
        mLogCfg.cfgFilePath[i] = settings->value(gCfgListName[i]).toString();
    }
    settings->endGroup();
}
void logFileRefresh(int count,QString cfgName)
{
    int i = 0;
    logCfg mLogCfg;
    logFileGetInfo(gLogFileName,mLogCfg);

    QFile logFile(gLogFileName);
    logFile.open(QIODevice::WriteOnly|QIODevice::Text);
    logFile.resize(0);//清空
    QTextStream out(&logFile);
    out << "[LOCAL]\n";
    out << "CURRENT_PATH=";
    out << mLogCfg.strFileName<<"\n";
    out << "CFG_FILE_SCAN=";
    out<< mLogCfg.cfgFileScan<<"\n";
    out << "ITEM_NUM=";
    out << mLogCfg.itemNum<<"\n";
    out << "CURRENT_CFG_NAME=";
    out << mLogCfg.currentCfgName<<"\n";
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        out << gCfgName[i]<<"=";
        if(i==count)
            out << cfgName<<"\n";
        else
            out << mLogCfg.cfgName[i]<<"\n";
    }
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        out << gCfgListName[i]<<"=";
        out << mLogCfg.cfgFilePath[i]<<"\n";
    }

    logFile.close();
}
void logFileAppend(cfgTemp mCfgTemp,QString filePath)
{
    int i = 0;
    logCfg mLogCfg;
    logFileGetInfo(gLogFileName,mLogCfg);
    filePathEncode(filePath);

    QFile logFile(gLogFileName);
    logFile.open(QIODevice::WriteOnly|QIODevice::Text);
    logFile.resize(0);//清空
    QTextStream out(&logFile);
    out << "[LOCAL]\n";
    out << "CURRENT_PATH=";
    out << filePath<<"\n";
    out << "CFG_FILE_SCAN=";
    out<< mLogCfg.cfgFileScan<<"\n";
    out << "ITEM_NUM=";
    out << mLogCfg.itemNum+1<<"\n";
    out << "CURRENT_CFG_NAME=";
    out << mLogCfg.currentCfgName<<"\n";
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        out << gCfgName[i]<<"=";
        out << mLogCfg.cfgName[i]<<"\n";
    }
    out << gCfgName[i]<<"=";
    out << mCfgTemp.cfgName<<"\n";
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        out << gCfgListName[i]<<"=";
        out << mLogCfg.cfgFilePath[i]<<"\n";
    }
    out << gCfgListName[i]<<"=";
    out << filePath<<"\n";

    logFile.close();
}
void logFileEnableCfg(cfgTemp mCfgTemp)
{
    int i = 0;
    logCfg mLogCfg;
    logFileGetInfo(gLogFileName,mLogCfg);

    QFile logFile(gLogFileName);
    logFile.open(QIODevice::WriteOnly|QIODevice::Text);
    logFile.resize(0);//清空
    QTextStream out(&logFile);
    out << "[LOCAL]\n";
    out << "CURRENT_PATH=";
    out << mLogCfg.strFileName<<"\n";
    out << "CFG_FILE_SCAN=";
    out<< mLogCfg.cfgFileScan<<"\n";
    out << "ITEM_NUM=";
    out << mLogCfg.itemNum<<"\n";
    out << "CURRENT_CFG_NAME=";
    out << mCfgTemp.cfgName<<"\n";
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        out << gCfgName[i]<<"=";
        out << mLogCfg.cfgName[i]<<"\n";
    }
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        out << gCfgListName[i]<<"=";
        out << mLogCfg.cfgFilePath[i]<<"\n";
    }

    logFile.close();
}



void logFileDeleteCfg(QString delCfgName)
{
    int i = 0;
    logCfg mLogCfg;
    logFileGetInfo(gLogFileName,mLogCfg);

    QFile logFile(gLogFileName);
    logFile.open(QIODevice::WriteOnly|QIODevice::Text);
    logFile.resize(0);//清空
    QTextStream out(&logFile);
    out << "[LOCAL]\n";
    out << "CURRENT_PATH=";
    out << mLogCfg.strFileName<<"\n";
    out << "CFG_FILE_SCAN=";
    out<< mLogCfg.cfgFileScan<<"\n";
    out << "ITEM_NUM=";
    out << mLogCfg.itemNum-1<<"\n";
    out << "CURRENT_CFG_NAME=";
    if(delCfgName.compare(mLogCfg.currentCfgName,Qt::CaseInsensitive)==0)
        out << "\n";
    else
        out << mLogCfg.currentCfgName<<"\n";
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        if(delCfgName.compare(mLogCfg.cfgName[i],Qt::CaseInsensitive) == 0)
        {
            for(int j = i;j<mLogCfg.itemNum;j++)
            {
                mLogCfg.cfgName[j] = mLogCfg.cfgName[j+1];
                mLogCfg.cfgFilePath[j] = mLogCfg.cfgFilePath[j+1];
            }
            mLogCfg.itemNum--;
        }
        if(i ==  mLogCfg.itemNum)
            break;
        out << gCfgName[i]<<"=";
        out << mLogCfg.cfgName[i]<<"\n";
    }
    for(i = 0;i<mLogCfg.itemNum;i++)
    {
        out << gCfgListName[i]<<"=";
        out << mLogCfg.cfgFilePath[i]<<"\n";
    }

    logFile.close();
}
void logFileRead(MainWindow* window,QString fileName)
{
    int i,missCount = 0;
    bool fileCfgScan;
    QSettings *settings = new QSettings(fileName,QSettings::IniFormat);

    //Read
    settings->beginGroup("LOCAL");
    gStrFileName = settings->value("CURRENT_PATH","").toString();
    filePathDecode(gStrFileName);
    gItemNum = settings->value("ITEM_NUM","").toInt();
    fileCfgScan = settings->value("CFG_FILE_SCAN").toBool();
    QString currentCfgName = settings->value("CURRENT_CFG_NAME","").toString();
    for(i=0;i<gItemNum;i++)
    {
        cfgTemp mCfgTemp;
        QString cfgFilePath = settings->value(gCfgListName[i]).toString();
        filePathDecode(cfgFilePath);
        if(cfgFileRead(cfgFilePath,mCfgTemp) == -1)
        {
            logCfg mLogCfg;
            logFileGetInfo(fileName,mLogCfg);
            logFileDeleteCfg(mLogCfg.cfgName[(i-missCount)]);
            missCount++;
            continue;
        }
        cfgNameFilter(&mCfgTemp.cfgName);
        logFileRefresh((i-missCount),mCfgTemp.cfgName);
        gCfgList.append(&mCfgTemp);
        treeItemAdd(window,mCfgTemp,(i-missCount),currentCfgName);
    }
    gItemNum-=missCount;
    if((!currentCfgName.isEmpty()) && (matchCfg(currentCfgName)!=nullptr))
    {
        gCurrentCfg = *matchCfg(currentCfgName);
        changeConvertBottomText(gCurrentCfg.codeFormate);
    }
    settings->endGroup();

    if(fileCfgScan)
        cfgScan(window,gDefaultFilePath);

}
