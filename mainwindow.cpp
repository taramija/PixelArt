#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pixelcube.h"
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pixmap = NULL;
    filePath = QString("");

    cubeSize = 10; // set size of the pixel region
    status = "new"; // set status of the program

    //    alignParam = this->size();
    //    iniPos = alignParam.width()/2 - 400/2;
    //    topOffset = 50;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent*e)
{
    //we create a Drawing context and attach it to the calling object, namely the main window
//    QPainter painter(this);



//    if(pixmap) {
//        // paint a square 400x400 pixels size, located at a certain position
//        QRect R( iniPos, topOffset, 400, 400 );
//        painter.scale(1,1);
//        painter.drawPixmap(R, *pixmap);

//    }

}


void MainWindow::on_btnLoad_clicked()
{
    //same as drawing in Label: we first get the path to an image to be drawn
    filePath = QFileDialog::getOpenFileName(
                this,
                "Select an image",
                "d:\\CProject/PixelArt",
                "Images (*.png *.jpg *.jpeg)");

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

    // update status to track user behavior
    status = "loaded";

    // create new pixmap using this loaded image
    updatePixmap(*img);

}

void MainWindow::on_btnPixelize_clicked() {

    // duplicate the  image (not yet implemented)
    pixelizedImg = img;

    // loop through every pixels of the image cubewise
    for(int i = 0; i < pixelizedImg->width(); i += cubeSize)
        for(int j = 0; j < pixelizedImg->height(); j += cubeSize){

            int count = 0, r=0,g=0,b=0,a=0;

            // loop through every pixels of the pixel cube
            for(int k = 0; k < cubeSize; ++k)
                for(int l = 0; l < cubeSize; ++l){

                    // stopping criterion in case the last cube
                    // that compute data out of image boundary
                    if (i+k < pixelizedImg->width() && j+l <pixelizedImg->height()) {

                        // convert the QRgb to QColor for color extracting process
                        QColor color(pixelizedImg->pixel(i+k,j+l));

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
                    if (i+k < pixelizedImg->width() && j+l <pixelizedImg->height())
                        pixelizedImg->setPixel(i+k, j+l, meanColor);

                    // calculate width for abnormal cube
                    else if(i+k < pixelizedImg->width())
                        cubeWidth = pixelizedImg->width() - i;

                    // calculate height for abnormal cube
                    else if(j+l <pixelizedImg->height())
                        cubeHeight = pixelizedImg->height() - j;


            // add this block of pixel as a PixelCube in the grid of mainwindow for PixelArt function
            PixelCube *newcube = new PixelCube(r,g,b,a);

            // set width height for this cube
            newcube->setWidth(cubeWidth);
            newcube->setHeight(cubeHeight);


            // assign the element for the Grid using the "normalized" i and j indexes
            setPixelCube(i/cubeSize, j/cubeSize, *newcube);

            delete newcube;
        }

    // update status to track user behavior
    status = "pixelized";

    // create new pixmap using this new image
    updatePixmap(*pixelizedImg);

}

void MainWindow::on_btnArt_clicked()
{
    // open multiple files, the user select a set of images
    // that he wants to be involed in pixelart process
    filePaths = QFileDialog::getOpenFileNames(
                this,
                "Select sample images",
                "d:\\CProject/PixelArt",
                "Images (*.png *.jpg *.jpeg)");

    if( filePaths.isEmpty() ) return;

    // creat the images using the file paths
    // and add every images to a QVector<QImage>
    for (int i =0; i < filePaths.size(); i++){
        QImage *image = new QImage(filePaths.at(i));
        imgList.append(*image);
    }

    // loop through the grid system (cubewise)
    // that we created previously during pixelizing process

    std :: vector < std :: vector < PixelCube > >::iterator cubeRow;
    std :: vector < PixelCube >::iterator cube;

    // use painter to join (by painting) this cube (image) into a big blank
    // image in order to have a big combination image after processing
    QPainter cubePainter;

    // duplicate new image from pixelized image
    artedImg = *pixelizedImg;

    // choose image to paint in
    cubePainter.begin(&artedImg);

    // index var for row vector loop (I can't find any proper way)
    int m=0;

    for (cubeRow = grid.begin(); cubeRow != grid.end(); ++cubeRow){

        // index var for col vector loop
        int n=0;

        for (cube = cubeRow->begin(); cube != cubeRow->end(); ++cube){

            // find the best match image of this cube and update
            // it to img global variable to paint it after this
            QImage tempImg = (*cube).findResembleImage(imgList);

            // painting process
            cubePainter.drawImage(QRectF(m*cubeSize,n*cubeSize,cubeSize,cubeSize),
                              tempImg,
                              QRectF(0,0,cubeSize,cubeSize));

            n++; // increase col iterator index
        }
        m++; // increase row iterator index
    }

    // end painting (joint process)
    cubePainter.end();

    // update status to track user behavior
    status = "pixelated";

    // create new pixmap using this big combination image
    updatePixmap(artedImg);

}

void MainWindow::on_btnSave_clicked()
{
    QString saveName = QFileDialog::getSaveFileName(this, tr("Save the picture"),
                                "",
                                tr("Images (*.png *.jpeg *.jpg)"));

    curImg->save(saveName);
}

void MainWindow::updatePixmap(QImage &processingImg){

    curImg = &processingImg;

    if(pixmap) delete pixmap;

    pixmap = new QPixmap;

    *pixmap = QPixmap ::fromImage(processingImg);


    // update viewport with new picture
    QGraphicsScene* scene = new QGraphicsScene(ui->pictureViewport);

    scene->addPixmap(*pixmap);

    ui->pictureViewport->setScene(scene);

    ui->pictureViewport->show();

    // activate buttons
    if(status == "loaded"){
        ui->btnPixelize->setEnabled(true);
        ui->btnSave->setEnabled(true);
        ui->btnArt->setEnabled(false);
    }else if(status == "pixelized"){
        ui->btnArt->setEnabled(true);

    }
}


