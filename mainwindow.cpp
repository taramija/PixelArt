#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pixelcube.h"
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
    iniPos = alignParam.width()/2 - 400/2;
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

    if(pixmap) delete pixmap;

    pixmap = new QPixmap;
    *pixmap = QPixmap ::fromImage(*img);

    if(pixmap) {

        // spit the painting process for 2 cases
        if(true){  // img load & pixelize function

            // paint a square 400x400 pixels size, located at a certain position
            QRect R( iniPos, topOffset, 400, 400 );
            painter.scale(1,1);
            painter.drawPixmap(R, *pixmap);
        }else{  // img pixelart function (need different set of position)

            QRect R( iniPos, topOffset, cubeSize, cubeSize );   // still working on this
            painter.scale(1,1);
            painter.drawPixmap(R, *pixmap);
        }
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

    update();
}

void MainWindow::on_btnPixelize_clicked() {

    // duplicate the  image (not yet implemented)

    // loop through every pixels of the image cubewise
    for(int i = 0; i < img->width(); i += cubeSize)
        for(int j = 0; j < img->height(); j += cubeSize){

            int count = 0, r=0,g=0,b=0,a=0;

            // loop through every pixels of the pixel cube
            for(int k = 0; k < cubeSize; ++k)
                for(int l = 0; l < cubeSize; ++l){

                    // stopping criterion in case the last cube
                    // that compute data out of image boundary
                    if (i+k < img->width() && j+l <img->height()) {

                        // convert the QRgb to QColor for color extracting process
                        QColor color(img->pixel(i+k,j+l));

                        // extract color channels using QColor built-in functions
                        r += color.red();
                        g += color.green();
                        b += color.blue();
                        a += color.alpha();

                        // count the iteration to get the total pixel of the cube
                        count++;
                    }
                }

            // calculate mean color value of every channels
            r /= count; g /= count; b /= count; a /= count;

            // combine 4 channels into QRgb data type
            QRgb meanColor = qRgba(r,g,b,a);

            // replace the pixel of the cube by the new color (the same loop above)
            for(int k = 0; k < cubeSize; ++k)
                for(int l = 0; l < cubeSize; ++l)

                    // same stopping criterion as above
                    if (i+k < img->width() && j+l <img->height())
                        img->setPixel(i+k, j+l, meanColor);

            // add this block of pixel as a PixelCube in the grid of mainwindow for PixelArt function

            PixelCube *newcube = new PixelCube(r,g,b,a);
            setPixelCube(i, j, *newcube);

            delete newcube;
        }

    update();
}

void MainWindow::on_btnArt_clicked()
{
    // open multiple files, the user select a set of images
    // that he wants to be involed in pixelart process
    filePaths = QFileDialog::getOpenFileNames(
                this,
                "BMP files"
                ,QDir::currentPath(),
                "Bitmap files (*.bmp);;All files (*.*)" );

    if( filePaths.isEmpty() ) return;

    // creat the images using the file paths
    // and add every images to a QVector<QImage>
    for (int i =0; i < filePaths.count(); i++)
        imgList->append(QImage(filePaths[i]));

    // loop through the grid system (cubewise)
    // that we created previously during pixelizing process

    std :: vector < std :: vector < PixelCube > >::iterator cubeRow;
    std :: vector < PixelCube >::iterator cube;

    for (cubeRow = grid.begin(); cubeRow != grid.end(); ++cubeRow)
        for (cube = cubeRow->begin(); cube != cubeRow->end(); ++cube){
            // find the best match image of this cube and update
            // it to img global variable to paint it after this
            *img = (*cube).findResembleImage(*imgList);

            update(); // draw cube by cube
        }
}

void MainWindow::on_btnSave_clicked()
{

}



