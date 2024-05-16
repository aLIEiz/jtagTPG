#ifndef EXECL_H
#define EXECL_H
#include <QtWidgets/QMessageBox>
#include <QAxObject>
#include <QFile>
#include "jtag.h"
#include <QCoreApplication>

class ExcelBase : public QAxObject
{
public:
    QAxObject* workSheet;
    QAxObject* workBooks;
    QAxObject* workBook;
    QAxObject *excel;

    ExcelBase();
    QVariant readAll();
    void castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res);

};

typedef struct
{
    QString type;
    uint32_t bitCount;
    uint64_t tdiValue;
    uint8_t isTap;
    QString longTdiValue;

}jtagCmdContentTemp;

int excelRead(QString filePath,ExcelBase *excelHandle,JtagList* jtagCmdList,LongCmdList* longCmdList);
void fetchJtagCmd(ExcelBase *excelHandle,QString filePath,JtagList* localCmdList);
int cmdReadIn(QList<QList<QVariant> > excelData,int length,JtagList* localCmdList,LongCmdList* localLongCmdList);
int csvCmdReadIn(QList<QStringList>& data,int length,JtagList* localCmdList,LongCmdList* localLongCmdList);
int csvRead(QString filePath,QList<QStringList>& data);
extern QString gDefaultFilePath;






#endif // EXECL_H
