#include "hex.h"

bool gSign_firstDataLine=false;

const QString HexTypeTable[6] =
{
    "00","01","02","03","04","05",
};


char convertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return (ch-0x30);
    else if((ch >= 'A') && (ch <= 'F'))
        return ((ch-'A')+10);
    else if((ch >= 'a') && (ch <= 'f'))
        return ((ch-'a')+10);
    else return (-1);
}

hexErrorCode getHexLineData(QString lineData,stHexLineData *p)
{
    quint8 i = 0;
    quint8 crcTemp = 0;
    char *pcdata;
    QByteArray ba = lineData.toLatin1(); // must
    pcdata=ba.data();

    quint32 lineLen = lineData.size();
    if((lineLen < MIN_HEX_LINE_COUNT_LENGHT)) {return HEX_LENGHT_ERROR;}
    if(*pcdata != 0x3A) {return HEX_FORMAT_ERROR;}//必须以":"号开始
    //获取Type
    QString type = lineData.mid(7,2);
    for(i = 0; i < RECORD_HEX_MAX; i++)
    {
        if(type == HexTypeTable[i])
        {
            p->type = (emHexType)i;
            break;
        }
    }
    if(i == RECORD_HEX_MAX) return HEX_FORMAT_ERROR;//type无法识别
    crcTemp += (convertHexChar(*(pcdata + 7)) << 4) | convertHexChar(*(pcdata + 8));//type,校验用

    p->count = (convertHexChar(*(pcdata + 1)) << 4) | convertHexChar(*(pcdata + 2));//获取count
    crcTemp += p->count;
    if(p->count != (((lineLen - 1) / 2) - 5)) return HEX_FORMAT_ERROR;
    //获取address
    p->address = (convertHexChar(*(pcdata + 3)) << 12) | (convertHexChar(*(pcdata + 4)) << 8) | (convertHexChar(*(pcdata + 5)) << 4) | convertHexChar(*(pcdata + 6));
    crcTemp += (p->address >> 8) & 0xFF;
    crcTemp += p->address & 0xFF;
    //获取data
    for(i = 0; i < p->count; i++)
    {
        p->data[i] = (convertHexChar(*(pcdata + 2*i + 9)) << 4) | convertHexChar(*(pcdata + 2*i + 10));
        crcTemp += p->data[i];
    }
    p->checksum = (convertHexChar(*(pcdata + 2*i + 9)) << 4) | convertHexChar(*(pcdata + 2*i + 10));
    if(p->checksum != ((0x100 - crcTemp) & 0xFF))
        return HEX_VERIFY_ERROR;
    p->datalen = p->count;

    return HEX_NO_ERROR;
}
//void lineDataIntoOut(stHexLineData hexLineData, QTextStream* out,uint64_t* pcStartAddr)
void lineDataIntoOut(stHexLineData hexLineData, QTextStream* out)
{
    QString temp;
    uint64_t dataTemp;
    switch(hexLineData.type)
    {
    case RECORD_DATA:
        if(gSign_firstDataLine)//load addr lower 16-bit
        {
            gSign_firstDataLine=false;
            temp = QString("%1").arg(hexLineData.address,4,16,QLatin1Char('0'));
            *out<<temp<<"\n";
        }
        for(int i =0;i<hexLineData.datalen;i++)
        {
            temp = QString("%1 ").arg(hexLineData.data[i],2,16,QLatin1Char('0'));
            *out<<temp;
        }
        *out<<"\n";
        break;
    case RECORD_END_OF_FILE:
        *out<<"q";
        break;
    case RECORD_EXTENDED_SEGMENT_ADDRESS:

        break;
    case RECORD_START_SEGMENT_ADDRESS:

        break;
    case RECORD_EXTENDED_LINEAR_ADDRESS://load higher 16-bit address
        *out<<"@";
        dataTemp = (hexLineData.data[0]<<8) +hexLineData.data[1];
        temp = QString::number(dataTemp,16);
        *out<<temp;
        gSign_firstDataLine = true;
        break;
    case RECORD_START_LINEAR_ADDRESS://start PC
//        *pcStartAddr = (hexLineData.data[0]<<24)+(hexLineData.data[1]<<16)+(hexLineData.data[2]<<8)+(hexLineData.data[3]);
        break;
    default:

        break;
    }
}










