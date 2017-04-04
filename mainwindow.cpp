#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
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
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent*e)
{
    //we create a Drawing context and attach it to the calling object, namely the main window
    QPainter painter(this);
    QSize alignParam = this->size();
    int iniPos = alignParam.width()/2 - 300/2;
    int cubeSize = 10;
    int topOffset = 50;

    if(function == 1){
        if(pixmap)
        {
            //we create a rendering square (200x400 pixels size, located at point 300, 50
            QRect R( iniPos, topOffset, 400, 400 );
            painter.scale(1,1);
            painter.drawPixmap(R, *pixmap); //once again, the image will be stretched... How could you do to overcome this issue??
        }
    }else if(function ==2){

        for(int i = 0; i < img->width(); i+=cubeSize){
            for(int j = 0; j < img->height(); j+=cubeSize){

//                QVector<QRgb> colorVal;
//                QList<int> pixelVal;
//                QRgb temp;

                QList<int> redList, greenList, blueList, alphaList;
                int meanRed, meanGreen, meanBlue, meanAlpha;

                for(int k = i; k < i+cubeSize; k++){
                    for(int l = j; l < j+cubeSize; l++){
                        int r,g,b,a;

//                        colorVal.append(img->pixel(k,l));
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
                QRgb meanColor = qRgb(meanRed,meanGreen,meanBlue);

//                QColor meanColor(meanRed,meanGreen,meanBlue,meanAlpha);

                for(int q = i; q < i+cubeSize; q++){
                    for(int r = j; r < j+cubeSize; r++)
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

//                qDebug() << colorVal.length();
//                qDebug() << colorVal[0];
//                qDebug() << colorVal[1];
//                qDebug() << colorVal[2];
//                qDebug() << colorVal[3];
//                qDebug() << mean;

//                QColor color(mean);
//                Qt::BrushStyle style = Qt::SolidPattern;
//                QBrush brush(color, style);
//                QRect rect = QRect(iniPos+i, topOffset+j, cubeSize, cubeSize);

//                painter.setBrush(brush);
//                painter.setPen(Qt::NoPen);
//                painter.drawRect(rect);


            }
        }
    }

}


void MainWindow::on_btnLoad_clicked()
{
    function = 1;
    //same as drawing in Label: we first get the path to an image to be drawn
    filePath = QFileDialog::getOpenFileName(
                this,
                "Open a file",
                QString(),
                "Images (*.png *.gif *.jpg *.jpeg)");

    if (filePath.isEmpty()) return;

    if (pixmap) delete pixmap;

    //we create a pixmap
    //    QImage *img = new QImage();
    img = new QImage(filePath);
    pixmap = new QPixmap(filePath);

    cout <<pixmap<<endl;
    //we create a rectangle in which we are going to draw the pixmap
    update();
    cout <<"done"<<endl;
}

void MainWindow::on_btnPixelize_clicked()
{
    function = 2;
}

void MainWindow::on_btnSave_clicked()
{

}
