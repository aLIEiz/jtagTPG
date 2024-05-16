#ifndef JTAG_H
#define JTAG_H

#include "type.h"
#include <stdio.h>
#include <QMessageBox>
#include <QFile>
#include<QTime>
#include "config.h"


///////////////////////////////////////////////////////
#define TDI_DEFAULET_HIGH
//////////////////////////////////////////////////////

//jtagTemp对象的type
#define DR 0
#define IR 1
#define DELAY 2
#define CLOCK 3
#define SET_EMPTY_CLOCK 4
#define SET_FREQ_MULT   5
#define SET_IDLE_WITH_TCK 6
#define SET_TDI_VALUE 7
#define SET_TDI_PLUSE 8

#define BIG_ENDIAN 0
#define LITTLE_ENDIAN 1

#define DMA_ADDR_REPLACE 0
#define GENERAL_ADDR_REPLACE 1

#define TAP_NUM_MASK (Uint64)0x0F000000
//芯片型号相关的宏定义,此处需与comboBox中芯片型号的序号保持一致
#define TMS66AK2H14_DSP     0
#define TMS320C6657         1
#define TMS320C6455         2
#define TMS320C2812         3
#define TMS320F28035        4
#define TMS320F28377        5
#define TMS320C5510         6
#define JSF169              7
#define JS320F240             8
#define TMS66AK2H14_ARM_TI     9
#define TMS66AK2H14_ARM_FPGA    10

//不同芯片的DMA下载数据的指令头
#define AK_DMACMD_HEAD       ((unsigned long long)0xFFC00000000)
#define C28_DMACMD_HEAD     ((unsigned long long)0x3C00000000)
#define F28377_DMACMD_HEAD       ((unsigned long long)0xFFC00000000)
#define C5510_DMACMD_HEAD   ((unsigned long long)0x3FFC00000000)
//不同芯片DMA下载的数据段长度
#define AK_DMACMD_DATA_LENGTH   32
#define C28_DMACMD_DATA_LENGTH  32
#define F28377_DMACMD_DATA_LENGTH   32
#define C5510_DMACMD_DATA_LENGTH 32
#define F240_DMACMD_DATA_LENGTH 16

#define addr8BitMode        false
#define addr16BitMode       true


#define TEST_LOGIC_STATE 1
#define RUN_TEST_IDLE_STATE 2
#define PAUSE_TEST_DATA_REGISTER_STATE 3
#define PAUSE_INSTRUCTION_REGISTER_STATE 4
#define SHIFT_TEST_DATA_REGISTER_STATE 5
#define SHIFT_INSTRUCTION_REGISTER_STATE 6

//这个类是JTAG指令的基类
class JtagTemp
{
public:
    JtagTemp();
    JtagTemp(Uint64 value,Uint32 bitCount);
    JtagTemp(Uint64 value,Uint32 bitCount,Uint8 type);
    JtagTemp(Uint64 value,Uint32 bitCount,Uint8 type,Uint32 tap);
    JtagTemp(Uint64 value,Uint32 bitCount,Uint8 type,Uint32 tap,Uint8 jtagState);
    void setValue(Uint64 value);
    Uint64 getValue();
    void setBitCount(Uint32 bitCount);
    Uint32 getBitCount();
    void setType(Uint8 type);
    Uint8 getType();
    void setTap(Uint32 tap);
    Uint32 getTap();
    void setNextState(Uint8 nextState);
    Uint8 getNextState();
    JtagTemp* getNext();
    JtagTemp* getPrev();
    void operator=(const JtagTemp& b);
    friend class JtagList;

private:
    Uint64 m_value;
    Uint32 m_bitCount;
    Uint8 m_type;//0 - DR,1 -  IR
    Uint32 m_tap;
    Uint8 m_nextState;
    JtagTemp* next;
    JtagTemp* prev;
};

//这个类用于创建JTAG的指令链表
class JtagList
{
public:
    JtagList();
    JtagList(const JtagTemp* temp);
    void append(const JtagTemp* temp);
    void remove();
    void removeAll();
    JtagTemp* head;
    JtagTemp* tail;
    JtagTemp* current;
    Uint32 length;
    void operator=(const JtagList b);
};
class LongCmdTemp
{
public:
    QString cmdValue;
    LongCmdTemp* next;
    void operator=(const LongCmdTemp &b);
};
class LongCmdList
{
public:
    LongCmdList();
    void append(const LongCmdTemp* temp);
    void removeAll();
    LongCmdTemp* head;
    LongCmdTemp* current;
    LongCmdTemp* tail;
};
class BinCmdTemp
{
public:
    Uint8 cmdValue;//TMS/TCK/TDI/TDO
    double timeStamp;
    Uint32 cycle;//与下一条指令间的时间间隔除上采样频率--重复次数
    BinCmdTemp* next;
    void operator=(const BinCmdTemp &b);
};
class BinCmdList
{
public:
    BinCmdList();
    void append(const BinCmdTemp* temp);
    void removeAll();
    BinCmdTemp* head;
    BinCmdTemp* current;
    BinCmdTemp* tail;
};
class ExcelCmdFor407
{
public:
    double timeStamp;
    QString state;
    ExcelCmdFor407 *next;
    void operator=(const ExcelCmdFor407 &b);
};
class ExcelCmdFor407List
{
public:
    ExcelCmdFor407List();
    void append(const ExcelCmdFor407* temp);
    void removeAll();
    ExcelCmdFor407* head;
    ExcelCmdFor407* current;
    ExcelCmdFor407* tail;
};

void jtagCmdListToBin(JtagList* localCmdList,LongCmdList* longCmdList,QFile* outFile,cfgTemp* localCfg);
void jtagCmdToBin(Uint64 cmdValue,QFile* outFile,bool type,Uint32 cmdLength,cfgTemp localCfg);
void delJtagList(JtagList* localJtagList);
int outFileGenerate(QFile *outFile,QFile *outTempFile,QFile *outAddrTempFile,uint64_t pcAddr,cfgTemp localCfg);
void changeTap(JtagList* localCmdList,Uint32 tapNum);
void changeCmdListPointer(JtagList* cmdList,int num);
Uint32 changeCmdListValue(JtagList* localCmdList,LongCmdList* longCmdList,QString templet,Uint32 changeValue,Uint32 replaceBitCount,cfgTemp localCfg);
void jtagGenerateDelay(coreCfgTemp coreCfg,Uint64 time,QFile* outFile, bool emu);
void jtagGenerateClock(coreCfgTemp coreCfg,Uint64 time,QFile* outFile, bool emu);
void sleep(unsigned int msec);//延时功能


#endif // JTAG_H
