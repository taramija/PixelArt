#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QFileDialog>
#include <QBitArray>
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pixmap = NULL;
    filePath = QString("");

    alignParam = this->size();
    iniPos = alignParam.width()/2 - 300/2;
    cubeSize = 5;
    topOffset = 50;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent*e)
{
    //we create a Drawing context and attach it to the calling object, namely the main window
    QPainter painter(this);

    if(pixmap) {
        //we create a rendering square (200x400 pixels size, located at point 300, 50
        QRect R( iniPos, topOffset, 400, 400 );
        painter.scale(1,1);
        painter.drawPixmap(R, *pixmap); //once again, the image will be stretched... How could you do to overcome this issue??
    }

}


void MainWindow::on_btnLoad_clicked()
{
    //same as drawing in Label: we first get the path to an image to be drawn
    filePath = QFileDialog::getOpenFileName(
                this,
                "Open a file",
                QString(),
                "Images (*.png *.gif *.jpg *.jpeg)");

    if (filePath.isEmpty()) return;

    img = new QImage(filePath);

    updatePixmap();
}

void MainWindow::on_btnPixelize_clicked() {

    //duplicate hte  image
    for(int i = 0; i < img->width(); i += cubeSize)
        for(int j = 0; j < img->height(); j += cubeSize){

            int count = 0, r=0,g=0,b=0,a=0;

            for(int k = 0; k < cubeSize; k++)
                for(int l = 0; l < cubeSize; l++){
                    if (i+k < img->width() && j+l <img->height()) {

                        QColor color(img->pixel(i+k,j+l));

                        r += color.red();
                        g += color.green();
                        b += color.blue();
                        a += color.alpha();
                        count++;
                    }
                }

            r /= count;
            g /= count;
            b /= count;
            a /= count;

            QRgb meanColor = qRgba(r,g,b,a);

            for(int k = 0; k < cubeSize /*loopRestricterX*/; k++)
                for(int l = 0; l < cubeSize /*loopRestricterY*/; l++)
                    if (i+k < img->width() && j+l <img->height())

                        img->setPixel(i+k, j+l, meanColor);


        }



    updatePixmap();
}

void MainWindow::on_btnArt_clicked()
{

    filePaths = QFileDialog::getOpenFileNames(
                this,
                "BMP files"
                ,QDir::currentPath(),
                "Bitmap files (*.bmp);;All files (*.*)" );

    if( filePaths.isEmpty() ) return;

    for (int i =0;i<filenames.count();i++)
        imgList->append(QImage(filePaths[i]));

    //loop through image list
    for(it = imgList.begin(); it != imgList.end(); it++)    {


    }

    for (cubePack = grid.begin(); cubePack != grid.end(); ++cubePack)
        for (cube = cubePack->begin(); cube != cubePack->end(); ++cube)
//            *cube.findResembleImage(imgList);



    //comparition process



    updatePixmap();
}

void MainWindow::on_btnSave_clicked()
{

}

void MainWindow::updatePixmap(){


    if(pixmap) delete pixmap;
    pixmap = new QPixmap;
    *pixmap = QPixmap ::fromImage(*img);
    update();


}


