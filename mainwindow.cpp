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

    if(function == 1){
        if(pixmap)
        {
            //we create a rendering square (200x400 pixels size, located at point 300, 50
            QRect R( iniPos, 50, 400, 400 );
    //        *pixmap->scaled(200,100);
            painter.scale(1,0.7);
            painter.drawPixmap(R, *pixmap); //once again, the image will be stretched... How could you do to overcome this issue??
        }
    }else if(function ==2){
        for(int i = 0; i < img->width(); i+5){
            for(int j = 0; j < img->height(); i+5){
                QVector<int> colorVal;
                int temp = 0;

                for(int k = 0; k < i+5; k++){
                    for(int l = 0; l < j+5; l++)
                        colorVal.append(img->pixel(k,l));
                }

                for(int m = 0; m < colorVal.length(); m++){
                    temp += colorVal[m];
                }

                //QString hexadecimal = QString::number( temp/colorVal.size(), 16 );
                QRgb mean = temp/colorVal.size();

                QColor color(mean);
                Qt::BrushStyle style = Qt::SolidPattern;
                QBrush brush(color, style);
                QRect rect = QRect(iniPos+i, 50+j, 5, 5);

                painter.setBrush(brush);
                painter.setPen(Qt::NoPen);
                painter.drawRect(rect);
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

    qDebug() << img->pixel(4,5);

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
