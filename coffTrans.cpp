
#include"coffTrans.h"

unsigned int PEFileIndex = 0;
fileHdr FileHeader;
optFileHdr OptFileHeader;
SecHdrC01 SecHeaderC01;
SecHdrC2 SecHeaderC2;

long getLong(char *data,int size)
{
    long back = 0;
    unsigned char temp;
    if (size <= 4)
    {
        for (int i = size - 1; i != -1; i--)
        {
            temp = data[i];
            back =(back<<8) + temp;
        }
    }
    else
    {
        return 0;
    }
    return back;
}
void loadByte(char Data[],int size,QByteArray peFileByte)
{
    for (int i = 0; i != size; i++)
    {
        Data[i] = peFileByte[PEFileIndex];
        PEFileIndex++;
    }
}
void loadHdr(QByteArray peFileByte)//load file header
{
    FileHeader.FileStarIndex = PEFileIndex;
    loadByte(FileHeader.iVer,2,peFileByte);
    loadByte(FileHeader.iNumOfSecHead,2,peFileByte);
    loadByte(FileHeader.iTimeStamp,4,peFileByte);
    loadByte(FileHeader.fpSymTabStartAddr,4,peFileByte);
    loadByte(FileHeader.iNumOfSymEntry,4,peFileByte);
    loadByte(FileHeader.iSizeOfOptHdr,2,peFileByte);
    loadByte(FileHeader.iFlags,2,peFileByte);
    if (getLong(FileHeader.iVer,2) != 0xC0)
    {
        loadByte(FileHeader.iTargetID,2,peFileByte);
    }
    FileHeader.FileEndIndex = PEFileIndex;
}
void LoadOptional(QByteArray peFileByte)
{
    OptFileHeader.FileStarIndex = PEFileIndex;
    loadByte(OptFileHeader.iMagicNumber,2,peFileByte);
    loadByte(OptFileHeader.iVerStamp,2,peFileByte);
    loadByte(OptFileHeader.iSizeOfExeCode,4,peFileByte);
    loadByte(OptFileHeader.iSizeOfInitData,4,peFileByte);
    loadByte(OptFileHeader.iSizeOfUinitData,4,peFileByte);
    loadByte(OptFileHeader.iEntryPoint,4,peFileByte);
    loadByte(OptFileHeader.iBeginAddrOfCode,4,peFileByte);
    loadByte(OptFileHeader.iBeginAddrOfInitData,4,peFileByte);
    OptFileHeader.FileEndIndex = PEFileIndex;
}
/// <summary>
/// 获取节表
/// </summary>
void LoadSection(long desAddr[], unsigned char desbuf[], long deslen[], long storAddr[],long* num,long* allLen,QByteArray peFileByte)
{
    long lSecPhyAddr, lSizeOfSec;
    unsigned int lPointToRawData;
    long i, j;
    char width = 2;
    long Count = getLong(FileHeader.iNumOfSecHead,2);
    *allLen = 0;
    *num = 0;
    //if (cbxWidth.Text == "8") width = 1;
    //if (cbxWidth.Text == "16") width = 2;
    //if (cbxWidth.Text == "32") width = 4;
    if (getLong(FileHeader.iVer,2) == 0xC2)
    {
        SecHeaderC2.FileStarIndex = PEFileIndex;
        for (i = 0; i < Count; i++)
        {

            loadByte(SecHeaderC2.szSectName,8,peFileByte);
            loadByte(SecHeaderC2.lSecPhyAddr,4,peFileByte);
            loadByte(SecHeaderC2.lSecVirAddr,4,peFileByte);
            loadByte(SecHeaderC2.lSizeOfSec,4,peFileByte);
            loadByte(SecHeaderC2.lPointToRawData,4,peFileByte);
            loadByte(SecHeaderC2.lPointToRelocateEntries,4,peFileByte);
            loadByte(SecHeaderC2.lPointToLineNumEntries,4,peFileByte);
            loadByte(SecHeaderC2.iNumOfRelocateEntries,4,peFileByte);
            loadByte(SecHeaderC2.iNumOfLineNumEntries,4,peFileByte);
            loadByte(SecHeaderC2.iFlags,4,peFileByte);
            loadByte(SecHeaderC2.reserved,2,peFileByte);
            loadByte(SecHeaderC2.iMemPageNum,2,peFileByte);

            lPointToRawData = getLong(SecHeaderC2.lPointToRawData,4);
            lSecPhyAddr = getLong(SecHeaderC2.lSecPhyAddr,4) * width;
            lSizeOfSec = getLong(SecHeaderC2.lSizeOfSec,4) * width;
            if (getLong(SecHeaderC2.iMemPageNum,2) == 0x00)
            {
                if (((getLong(SecHeaderC2.iFlags,4) & 0x60) != 0) && ((getLong(SecHeaderC2.iFlags,4) & 0x03) == 0))
                {
                    storAddr[*num] = *allLen;
                    for (j = 0; j < lSizeOfSec; j++)
                    {
                        uint32_t numTemp = lPointToRawData + j;
                        desbuf[(*allLen)++] = peFileByte[numTemp];
                    }
                    desAddr[*num] = lSecPhyAddr / width;
                    deslen[*num] = lSizeOfSec / width;
                    (*num)++;
                }
            }
        }
        SecHeaderC2.FileEndIndex = PEFileIndex;
    }
    else
    {
        SecHeaderC01.FileStarIndex = PEFileIndex;
        for (i = 0; i < Count; i++)
        {
            loadByte(SecHeaderC01.szSectName,8,peFileByte);
            loadByte(SecHeaderC01.lSecPhyAddr,4,peFileByte);
            loadByte(SecHeaderC01.lSecVirAddr,4,peFileByte);
            loadByte(SecHeaderC01.lSizeOfSec,4,peFileByte);
            loadByte(SecHeaderC01.lPointToRawData,4,peFileByte);
            loadByte(SecHeaderC01.lPointToRelocateEntries,4,peFileByte);
            loadByte(SecHeaderC01.lPointToLineNumEntries,4,peFileByte);
            loadByte(SecHeaderC01.iNumOfRelocateEntries,2,peFileByte);
            loadByte(SecHeaderC01.iNumOfLineNumEntries,2,peFileByte);
            loadByte(SecHeaderC01.iFlags,2,peFileByte);
            loadByte(SecHeaderC01.reserved,1,peFileByte);
            loadByte(SecHeaderC01.iMemPageNum,1,peFileByte);

            lPointToRawData = getLong(SecHeaderC01.lPointToRawData,4);
            lSecPhyAddr = getLong(SecHeaderC01.lSecPhyAddr,4) * width;
            lSizeOfSec = getLong(SecHeaderC01.lSizeOfSec,4) * width;
            if (getLong(SecHeaderC01.iMemPageNum,1) == 0x00)
            {
                if (((getLong(SecHeaderC01.iFlags,4) & 0x60) != 0) && ((getLong(SecHeaderC01.iFlags,4) & 0x03) == 0))
                {
                    storAddr[*num] = *allLen;
                    for (j = 0; j < lSizeOfSec; j++)
                    {
                        uint32_t numTemp = lPointToRawData + j;
                        desbuf[(*allLen)++] = peFileByte[numTemp];
                    }
                    desAddr[*num] = lSecPhyAddr;
                    deslen[*num] = lSizeOfSec;
                    (*num)++;
                }
            }
        }
        SecHeaderC01.FileEndIndex = PEFileIndex;
    }
}
void arrayCopy(unsigned char* srcBuf,int srcLocal,unsigned char* destBuf,int destLocal,int length)
{
    for(int i = 0;i<length;i++)
    {
        *(destBuf+destLocal+i) = *(srcBuf+srcLocal+i);
    }
}
int coffToHex(QString strFileName,QString definedFilePath)
{
    PEFileIndex = 0;
    int i,j;
    QFile srcFile(strFileName);
    //|QIODevice::Text
    if(!srcFile.open(QIODevice::ReadOnly)){
        return 1;
    }

    QByteArray peFileByte;
    unsigned char *desbuf = new unsigned char[10000000];
    long *desAddr = new long[256];
    long *deslen = new long[256];
    long *storAddr = new long[256];
    //jtag_Cmd_Des[] cmdDes = new jtag_Cmd_Des[20];
    long num = 0, allLen = 0;
    peFileByte =  srcFile.readAll();
    for (i = 0; i < 1000000; i++)
    {
        desbuf[i] = 0xFF;
    }

    unsigned char readNum0,readNum1;
    readNum0 = peFileByte[0];
    readNum1= peFileByte[1];
    if (((readNum0 == 0xC0) || (readNum0 == 0xC1) || (readNum0 == 0xC2)) && (readNum1 == 0x00))
    {
        loadHdr(peFileByte);
        if (FileHeader.iSizeOfOptHdr[0] == 28)
        {
            LoadOptional(peFileByte);
        }
        LoadSection(desAddr, desbuf, deslen, storAddr,&num,&allLen,peFileByte);
        long desAddrTmp, deslenTmp, storAddrTmp;
        for (i = 0; i < num; i++)
        {
            for (j = i + 1; j < num; j++)
            {
                if (desAddr[j] < desAddr[i])
                {
                    desAddrTmp = desAddr[i];
                    deslenTmp = deslen[i];
                    storAddrTmp =  storAddr[i];
                    desAddr[i] = desAddr[j];
                    deslen[i] = deslen[j];
                    storAddr[i] = storAddr[j];
                    desAddr[j] = desAddrTmp;
                    deslen[j] = deslenTmp;
                    storAddr[j] = storAddrTmp;
                }
            }
        }
        unsigned char* outbuf = new unsigned char[10000000];
        long* outAddr = new long[256];
        long* outlen = new long[256];
        long* outstorAddr = new long[256];
        long outnum = 0, outallLen = 0;
//        long allLen = 0;
        char outFlag = 0;
        for (i = 0; i < num; i++)
        {
            if (desAddr[i] + deslen[i] == desAddr[i + 1])
            {
                if (outFlag == 0)
                {
                    outAddr[outnum] = desAddr[i];
                    outlen[outnum] = deslen[i];
                    outstorAddr[outnum] = outallLen;
                    outFlag = 1;
                }
                else
                {
                    outlen[outnum] += deslen[i];
                }
                arrayCopy(desbuf, storAddr[i], outbuf, outallLen, deslen[i]*2);
                outallLen += deslen[i]*2;
            }
            else
            {
                if (outFlag == 1)
                {
                    outlen[outnum] += deslen[i];
                    arrayCopy(desbuf, storAddr[i], outbuf, outallLen, deslen[i] * 2);
                    outnum++;
                    outFlag = 0;
                }
                else
                {
                    outAddr[outnum] = desAddr[i];
                    outlen[outnum] = deslen[i];
                    outstorAddr[outnum] = outallLen;
                    arrayCopy(desbuf, storAddr[i], outbuf, outallLen, deslen[i] * 2);
                    outallLen += deslen[i] * 2;
                    outnum++;
                }
            }
        }

        QFile outFile((definedFilePath+"/out.txt"));
        if(!outFile.open(QIODevice::WriteOnly|QIODevice::Text)){
            return 2;
        }
        QTextStream out(&outFile);
        int wbit = 16;//位宽


        QString strTemp;
        QString dataTemp;
//        for (i = 0; i < num; i++)
//        {
//            //address
//            dataTemp = QString("%1").arg(desAddr[i]);
//            strTemp = QString::number(dataTemp.toLong(),16);
//            strTemp.prepend("@");
//            out<<strTemp;out<<"\n";
//            //data
//            for (j = storAddr[i]; j < (storAddr[i] + deslen[i]*2); j++)//所有元素个数
//            {
//                dataTemp = QString::number(desbuf[j]);
//                strTemp = QString::number(dataTemp.toUInt()).asprintf("%02X",desbuf[j]);
//                strTemp.append(" ");
//                out<<strTemp;
//                if(((j - storAddr[i] + 1)%wbit) == 0)
//                    out<<"\n";
//            }
//            if(((j - storAddr[i])%wbit) != 0)
//                out<<"\n";
//        }
        for (i = 0; i < outnum; i++)
        {
            //address
            dataTemp = QString("%1").arg(outAddr[i]);
            strTemp = QString::number(dataTemp.toLong(),16);
            strTemp.prepend("@");
            out<<strTemp;out<<"\n";
            //data
            for (j = outstorAddr[i]; j < (outstorAddr[i] + outlen[i]*2); j++)//所有元素个数
            {
                dataTemp = QString::number(outbuf[j]);
                strTemp = QString::number(dataTemp.toUInt()).asprintf("%02X",outbuf[j]);
                strTemp.append(" ");
                out<<strTemp;
                if(((j - outstorAddr[i] + 1)%wbit) == 0)
                    out<<"\n";
            }
            if(((j - outstorAddr[i])%wbit) != 0)
                out<<"\n";
        }
        out<<"q";
        outFile.flush();
        outFile.close();
    }
    else
    {
        return 3;
    }
    srcFile.close();
    PEFileIndex = 0;
    return 0;
}

