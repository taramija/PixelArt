#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //--------------------- Centering MainWindow ------------------//

    // get geometry parameters of the screen
    QRect desktopScreen = QApplication::desktop()->screenGeometry();

    // new coordinators for mainWindow
    int posX = (desktopScreen.width()-w.width()) / 2;
    int posY = (desktopScreen.height()-w.height()) / 2;

    // move the mainWindow by this coordinators
    w.move(posX, posY);

    //-------------------------------------------------------------//

    w.show();

    return a.exec();
}
