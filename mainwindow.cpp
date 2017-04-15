#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pixelcube.h"
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
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
    cubeSize = 300;
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

//        // spit the painting process for 2 cases
//        if(artProcess){  // img pixelart function (need different set of position)

//            QRect R( iniPos, topOffset, cubeSize, cubeSize );   // still working on this
//            painter.scale(1,1);
//            painter.drawPixmap(R, *pixmap);

//        }else{  // img load & pixelize function

            // paint a square 400x400 pixels size, located at a certain position
            QRect R( iniPos, topOffset, 400, 400 );
            painter.scale(1,1);
            painter.drawPixmap(R, *pixmap);
//        }
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

    //set number of pixel cubes will be constructed using this
    //image, on both axises. Ceil will allow the program to count
    //the exception where the cubes are cut by the edge of the window
    //The program will still replace proper proper image onto these cubes normally
    setNumRows(ceil(img->width() / (double)cubeSize));
    setNumCols(ceil(img->height() / (double)cubeSize));

    //resize (init the size) the grid vector base on the number of cols and rows above
    grid.resize (rows, vector <PixelCube> (cols));

    // create new pixmap using this loaded image
    updatePixmap(*img);
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

            // var to store size of the cube (for exception case with abnormal size)
            int cubeWidth = cubeSize, cubeHeight = cubeSize;

            // replace the pixel of the cube by the new color (the same loop above)
            for(int k = 0; k < cubeSize; ++k)
                for(int l = 0; l < cubeSize; ++l)

                    // same stopping criterion as above
                    if (i+k < img->width() && j+l <img->height())
                        img->setPixel(i+k, j+l, meanColor);

                    // calculate width for abnormal cube
                    else if(i+k < img->width())
                        cubeWidth = img->width() - i;

                    // calculate height for abnormal cube
                    else if(j+l <img->height())
                        cubeHeight = img->height() - j;


            // add this block of pixel as a PixelCube in the grid of mainwindow for PixelArt function
            PixelCube *newcube = new PixelCube(r,g,b,a);

            // set width height for this cube
            newcube->setWidth(cubeWidth);
            newcube->setHeight(cubeHeight);


            // assign the element for the Grid using the "normalized" i and j indexes
            setPixelCube(i/cubeSize, j/cubeSize, *newcube);

            delete newcube;
        }

    // create new pixmap using this new image
    updatePixmap(*img);
}

void MainWindow::on_btnArt_clicked()
{
    // open multiple files, the user select a set of images
    // that he wants to be involed in pixelart process
    filePaths = QFileDialog::getOpenFileNames(
                this,
                "Open a file",
                QString(),
                "Images (*.png *.gif *.jpg *.jpeg)");

    if( filePaths.isEmpty() ) return;

    // creat the images using the file paths
    // and add every images to a QVector<QImage>
    for (int i =0; i < filePaths.size(); i++){
        QImage *aa = new QImage(filePaths.at(i));
        imgList.append(*aa);
    }

    // loop through the grid system (cubewise)
    // that we created previously during pixelizing process

    std :: vector < std :: vector < PixelCube > >::iterator cubeRow;
    std :: vector < PixelCube >::iterator cube;

    // use painter to join (by painting) this cube (image) into a big blank
    // image in order to have a big combination image after processing
    QPainter cubePainter;

    // choose image to paint in
    QImage artImg(*img);
    cubePainter.begin(&artImg);

    // index vars for vector loop (I can't find any proper way)
    int m=0,n=0;


    for (cubeRow = grid.begin(); cubeRow != grid.end(); ++cubeRow){
        for (cube = cubeRow->begin(); cube != cubeRow->end(); ++cube){

            // find the best match image of this cube and update
            // it to img global variable to paint it after this
            QImage tempImg = (*cube).findResembleImage(imgList);

            // painting process
            cubePainter.drawImage(QRectF(m*cubeSize,n*cubeSize,cubeSize,cubeSize),
                              tempImg,
                              QRectF(0,0,cubeSize,cubeSize));

            n++; // increase cube iterator index
        }
        m++; // increase cubeRow iterator index
    }

    // end painting (joint process)
    cubePainter.end();

    // create new pixmap using this big combination image
    updatePixmap(artImg);
}

void MainWindow::on_btnSave_clicked()
{

}

void MainWindow::updatePixmap(QImage &processingImg){

    if(pixmap) delete pixmap;

    pixmap = new QPixmap;

    *pixmap = QPixmap ::fromImage(processingImg);

}


