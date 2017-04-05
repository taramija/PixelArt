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
    cubeSize = 10;
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

void MainWindow::on_btnPixelize_clicked()
{

    for(int i = 0; i < img->width(); i+=cubeSize){
        for(int j = 0; j < img->height(); j+=cubeSize){

//          QVector<QRgb> colorVal;

            QList<int> redList, greenList, blueList, alphaList;
            int meanRed, meanGreen, meanBlue, meanAlpha;
            int loopRestricterX, loopRestricterY;

            i+cubeSize > img->width()
                    ? loopRestricterX = i + (i+cubeSize - img->width())
                    : loopRestricterX = i+cubeSize-1;

            j+cubeSize > img->height()
                    ? loopRestricterY = j + (j+cubeSize - img->height())
                    : loopRestricterY = j+cubeSize-1;


            for(int k = i; k <= loopRestricterX; k++){
                for(int l = j; l <= loopRestricterY; l++){
                    int r,g,b,a;

//                  colorVal.append(img->pixel(k,l));
                    QColor color(img->pixel(k,l));
                    color.getRgb(&r, &g, &b, &a);

                    redList.append(r);
                    greenList.append(g);
                    blueList.append(b);
                    alphaList.append(a);
                }
            }

            for(int m = 0; m < redList.length(); m++)
                meanRed += redList[m];
            for(int n = 0; n < redList.length(); n++)
                meanGreen += greenList[n];
            for(int o = 0; o < redList.length(); o++)
                meanBlue += blueList[o];
            for(int p = 0; p < alphaList.length(); p++)
                meanAlpha += alphaList[p];

            meanRed = meanRed/redList.length();
            meanGreen = meanGreen/greenList.length();
            meanBlue = meanBlue/blueList.length();
            meanAlpha = meanAlpha/alphaList.length();

            QRgb meanColor = qRgba(meanRed,meanGreen,meanBlue,meanAlpha);

            for(int q = i; q <= loopRestricterX; q++){
                for(int r = j; r <= loopRestricterY; r++)
                    img->setPixel(q,r,meanColor);
            }

//                for(int m = 0; m < colorVal.length(); m++){
//                    QColor color(colorVal[m]);
//                    pixelVal.append(color.lightness());

//                    temp += colorVal[m];
//                }

//                for(int n = 0; n < pixelVal.length(); n++){
//                    temp += pixelVal[n];
//                }

//                //QString hexadecimal = QString::number( temp/colorVal.size(), 16 );
//                QRgb mean = temp/colorVal.size();
//                int mean = temp/pixelVal.size();

//                QColor color(mean);
//                Qt::BrushStyle style = Qt::SolidPattern;
//                QBrush brush(color, style);
//                QRect rect = QRect(iniPos+i, topOffset+j, cubeSize, cubeSize);

//                painter.setBrush(brush);
//                painter.setPen(Qt::NoPen);
//                painter.drawRect(rect);


        }
    }

    updatePixmap();
}

void MainWindow::on_btnSave_clicked()
{

}

void MainWindow::updatePixmap(){
    if (pixmap) delete pixmap;

    //    QImage *img = new QImage();
    pixmap = new QPixmap::fromImage(img);

    update();
    cout <<"done updating image"<<endl;
}
