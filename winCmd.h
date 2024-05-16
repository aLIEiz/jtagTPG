#ifndef WINCMD_H
#define WINCMD_H
#include <QString>
#include <QProcess>


void executeCommandWait( QString command );
void executeCommandNotWait( QString command );
void toolCmdGenerate(QString toolPath,QString filePath,QString *cmd,QString srcCmd,QString destCmd);
void copyToClip(QString fileName);
void fileCopy(QString srcFile,QString tarFile);
void fileDel(QString file);
extern QString gDefaultFilePath;//默认执行位置

#endif // WINCMD_H
