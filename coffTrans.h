#ifndef COFFTRANS_H
#define COFFTRANS_H

#include<cstdio>
#include<cstdlib>
#include<QString>
#include<QFile>
#include<QTextStream>

typedef struct
{
    char iVer[2];               //COFF版本
    char iNumOfSecHead[2];      //Section个数
    char iTimeStamp[4];         //文件创建时间
    char fpSymTabStartAddr[4];  //符号表开始地址
    char iNumOfSymEntry[4];     //符号表的个数
    char iSizeOfOptHdr[2];      //optional区域大小，该值为0或者28，如果为0则无optional区域
    char iFlags[2];             //标志
    char iTargetID[2];          //芯片ID号，例如（0097h指示 TMS470R1x系列）
    long FileStarIndex = 0;
    long FileEndIndex = 0;
}fileHdr;
typedef struct
{
    char iVer[2];               //COFF版本
    char iNumOfSecHead[2];      //Section个数
    char iTimeStamp[4];         //文件创建时间
    char fpSymTabStartAddr[4];  //符号表开始地址
    char iNumOfSymEntry[4];     //符号表的个数
    char iSizeOfOptHdr[2];      //optional区域大小，该值为0或者28，如果为0则无optional区域
    char iFlags[2];             //标志
}fileHdrC0;
typedef struct
{
    char iVer[2];               //COFF版本
    char iNumOfSecHead[2];      //Section个数
    char iTimeStamp[4];         //文件创建时间
    char fpSymTabStartAddr[4];  //符号表开始地址
    char iNumOfSymEntry[4];     //符号表的个数
    char iSizeOfOptHdr[2];      //optional区域大小，该值为0或者28，如果为0则无optional区域
    char iFlags[2];             //标志
    char iTargetID[2];          //芯片ID号，例如（0097h指示 TMS470R1x系列）
}fileHdrC1;
typedef struct
{
    char iVer[2];               //COFF版本
    char iNumOfSecHead[2];      //Section个数
    char iTimeStamp[4];         //文件创建时间
    char fpSymTabStartAddr[4];  //符号表开始地址
    char iNumOfSymEntry[4];     //符号表的个数
    char iSizeOfOptHdr[2];      //optional区域大小，该值为0或者28，如果为0则无optional区域
    char iFlags[2];             //标志
    char iTargetID[2];          //芯片ID号，例如（0097h指示 TMS470R1x系列）
}fileHdrC2;

typedef struct
{
    char iMagicNumber[2];           //Optional区域文件头（0108h）
    char iVerStamp[2];              //
    char iSizeOfExeCode[4];	        //可执行代码大小
    char iSizeOfInitData[4];        //已初始化数据大小
    char iSizeOfUinitData[4];       //未初始化数据大小
    char iEntryPoint[4];            //入口指针
    char iBeginAddrOfCode[4];       //可执行代码开始地址
    char iBeginAddrOfInitData[4];   //已初始化数据开始地址
    long FileStarIndex = 0;
    long FileEndIndex = 0;
}optFileHdr;

typedef struct
{
    char szSectName [8];                 //名字
    char lSecPhyAddr [4];                //物理地址
    char lSecVirAddr[4];                //虚拟地址
    char lSizeOfSec[4];	                //存储大小
    char lPointToRawData[4];            //指向RAW数据
    char lPointToRelocateEntries[4];    //指向定位号
    char lPointToLineNumEntries[4];     //指向行数
    char iNumOfRelocateEntries[2];      //定位号
    char iNumOfLineNumEntries[2];       //行数号
    char iFlags[2];                     //标记
    char reserved[1];
    char iMemPageNum[1];
    long FileStarIndex = 0;
    long FileEndIndex = 0;
}SecHdrC01;


typedef struct
{
    char szSectName[8];                 //名字
    char lSecPhyAddr[4];                //物理地址
    char lSecVirAddr[4];                //虚拟地址
    char lSizeOfSec[4];	                //存储大小
    char lPointToRawData[4];            //指向RAW数据
    char lPointToRelocateEntries[4];    //指向定位号
    char lPointToLineNumEntries[4];     //指向行数
    char iNumOfRelocateEntries[4];      //定位号
    char iNumOfLineNumEntries[4];       //行数号
    char iFlags[4];                     //标记
    char reserved[2];
    char iMemPageNum[2];
    long FileStarIndex = 0;
    long FileEndIndex = 0;
}SecHdrC2;

int coffToHex(QString strFileName,QString definedFilePath);

#endif // COFFTRANS_H
