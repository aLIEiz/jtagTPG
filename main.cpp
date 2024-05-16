#include "mainwindow.h"
#include <QSplashScreen>
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;

    QSplashScreen *splash = new QSplashScreen();
    splash->setPixmap(QPixmap(":/img/start.png"));
    splash->show();
    QThread::sleep(2); //延时窗口加载
    delete splash;

    window.show();
    return a.exec();
}
