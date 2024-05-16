#include "winCmd.h"


void executeCommandWait( QString command )
{
    QStringList arguments;
    arguments << "/c";
    arguments << command;
    QProcess p;
    p.start("cmd",arguments);
    p.waitForStarted();
    p.waitForFinished();
    p.close();
}
void executeCommandNotWait( QString command )
{
    QStringList arguments;
    arguments << "/c";
    arguments << command;
    QProcess::startDetached( "cmd", arguments);
}
/*set hex6xPath=D:\work_documents\66AK\DSPpdk\ti\ti-cgt-c6000_8.3.2\bin
%hex6xPath%\hex6x --ti_txt 66AK_UART_DSP -o out.txt*/
void toolCmdGenerate(QString toolPath,QString filePath,QString *cmd,QString srcCmd,QString destCmd)
{
    cmd->clear();
    cmd->append(toolPath);
    cmd->append(" ");
    cmd->append(srcCmd);
    cmd->append(" ");
    cmd->append(filePath);
    cmd->chop(4);
    cmd->append(" ");
    cmd->append(destCmd);
    cmd->append(" ");
    cmd->append(gDefaultFilePath);
    cmd->append("/out.txt");
}
void copyToClip(QString fileName)
{
    fileName.prepend("clip < ");
    executeCommandNotWait(fileName);
}
void fileCopy(QString srcFile,QString tarFile)
{
    QString winCmd = QString("copy %1 %2 /y").arg(srcFile).arg(tarFile);
    executeCommandWait(winCmd);
}
void fileDel(QString file)
{
    QString winCmd = QString("del /f \"%1\"").arg(file);
    executeCommandNotWait(winCmd);
}
