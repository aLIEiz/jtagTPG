#ifndef HEX_H
#define HEX_H

#include <QFile>
#include <QTextStream>


const quint8 MIN_HEX_LINE_COUNT_LENGHT = 11;

typedef enum __tagHexErrorCode
{
    HEX_NO_ERROR = 0,
    HEX_FORMAT_ERROR,
    HEX_VERIFY_ERROR,
    HEX_LENGHT_ERROR,
    HEX_USERPAPR_EEROR,
}hexErrorCode;

typedef enum __tagHexType
{
    RECORD_DATA = 0,
    RECORD_END_OF_FILE,
    RECORD_EXTENDED_SEGMENT_ADDRESS,
    RECORD_START_SEGMENT_ADDRESS,
    RECORD_EXTENDED_LINEAR_ADDRESS,
    RECORD_START_LINEAR_ADDRESS,
    RECORD_HEX_MAX,
}emHexType;

typedef struct __tagHexLineData
{
    emHexType   type;
    quint8      count;
    quint32     address;
    quint8      data[80];
    quint8      checksum;
    quint8      datalen;
}stHexLineData;


hexErrorCode getHexLineData(QString lineData,stHexLineData *p);

//void lineDataIntoOut(stHexLineData hexLineData, QTextStream* out,uint64_t* pcStartAddr);
void lineDataIntoOut(stHexLineData hexLineData, QTextStream* out);






#endif // HEX_H
