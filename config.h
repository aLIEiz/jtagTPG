#ifndef CONFIG_H
#define CONFIG_H
#include <QString>

#define DMA_ADDR (uint32_t)0
#define PROGRAM_DATA0 (uint32_t)1
#define PROGRAM_DATA1 (uint32_t)2
#define PROGRAM_DATA2 (uint32_t)3
#define PROGRAM_DATA3 (uint32_t)4
#define PROGRAM_DATA4 (uint32_t)5

typedef enum
{
    ram8bitMode = 0,
    ram16bitMode,
    ram32bitMode,
    ram64bitMode
}addressMode;
typedef enum
{
    ram8bitAlian = 0,
    ram16bitAlian,
    ram32bitAlian,
    ram64bitAlian
}addressAlianMode;
typedef enum
{
    ATE_CODE = 0,
    TEST_CODE,
    EMU_CODE,
    EMPTY_CODE
}generateCodeFormate;
typedef struct
{
    uint32_t data;
    QString seqName;
    QString generalReplaceData;
    uint32_t generalReplaceCount;
    QString generalAddrReplaceSign;
    uint32_t generalReplaceBitLocal[5];
    uint32_t generalReplaceBitCount[5];
}generalReplace;
typedef struct
{
    QString coreName;
    QString coreLibPath;
    uint32_t tapNum;
    //[MAIN]
    QString ateHeadFileName;
    QString testHeadFileName;
    QString emuHeadFileName;
    //[CFG]
    bool withIcepick;
    bool useDma;
//    bool dmaInOneProcess;

    addressMode ramAddressMode;
    addressAlianMode ramAdressAlignMode;

    bool pcFromMap;
    uint32_t pcOffsetInCoff;//out文件中pc地址的偏移量
    uint8_t dataEndian;
    uint8_t irEndian;
    uint8_t drEndian;
    uint32_t dataOffsetInHex[5];
    uint32_t dataByteCountInHex[5];
    QString pcInTxt;
    uint32_t sectionAddrSub;
    int sectionAddrDiffInByte;

    bool codeForEmuWithTck;

    QString parserName;// 解析器名称
    QString parserSrcCmd;// 解析器名称
    QString parserDestCmd;// 解析器名称
    bool autoRefreshSourceFile;

    //[CODE]
    uint32_t seqCount;
    QString seqName[20];
    QString dmaStartSeq;
    QString dmaStopSeq;

    uint32_t dmaReplaceCount;
    QString dmaLoadAddrReplaceSign;
    uint32_t dmaReplaceData[5];
    uint32_t dmaReplaceBitLocal[5];
    uint32_t dmaReplaceBitCount[5];

    uint8_t generalReplaceSeqCount;
    generalReplace generalReplaceSeq[5];

    uint32_t dmaMaxLengthInOneSeq;
    //[CMD]
    uint32_t coreCmdLength;
    uint32_t dmaDataLength;
    uint64_t dmaCmdHead;
    uint64_t regWriteAddrHead;
    uint64_t regWriteDataHead;
    uint32_t dataLeftShiftBitCount;
    uint32_t addrLeftShiftBitCount;
    uint8_t dmaDataInstructionTapState;
    //[VAR]
    uint32_t jtagEmptyClockBetweenCmd;
    uint32_t jtagFreqMult;
    bool idleWithTck;
    uint32_t tdiIdleState;
    uint32_t tdiInsertPluseInIdle;

}coreCfgTemp;
class cfgTemp
{
public:
    cfgTemp();
    void operator=(const cfgTemp& b);

    cfgTemp* next;
    cfgTemp* prev;
    //[MAIN]
    QString cfgName;
    QString libPath;
    bool resultFileAutoSave;
    QString resultFileAutoSavePath;
    //[CFG]
    uint32_t coreCount;
    uint32_t currentCore;
    generateCodeFormate codeFormate;

    coreCfgTemp coreCfg[20];

};

class cfgList
{
public:
    cfgList();
    cfgList(const cfgTemp* temp);
    void append(const cfgTemp* temp);
    void remove();
    void remove(cfgTemp* temp);
    void removeAll();

    cfgTemp* head;
    cfgTemp* tail;
    cfgTemp* current;
    uint32_t length;
    void operator=(const cfgList b);
};
typedef struct
{
    QString strFileName;//文件夹路径
    QString cfgFileScan;
    int itemNum;//item总数
    QString cfgName[20];
    QString cfgFilePath[20];//文件路径
    QString currentCfgName;//当前cfg名
}logCfg;

extern QString gDefaultFilePath;
extern cfgList gCfgList;
extern int gItemNum;
extern QString gStrFileName;
extern cfgTemp gCurrentCfg;


extern QString gLogFileName;

extern QString gGeneralReplaceData[5];
extern QString gGeneralReplaceDataSign[5];
extern void changeConvertBottomText(generateCodeFormate codeFormate);

cfgTemp* matchCfg(QString itemName);
void delCfgFromList(QString itemName);
void cfgNameFilter(QString* cfgName);
int cfgFileRead(QString filePath,cfgTemp& mCfgTemp);
void logFileGetInfo(QString logFileName,logCfg& mLogCfg);
void logFileRefresh(int count,QString cfgName);
void logFileAppend(cfgTemp mCfgTemp,QString filePath);
void logFileEnableCfg(cfgTemp mCfgTemp);
void logFileDeleteCfg(QString delCfgName);




#endif // CONFIG_H
