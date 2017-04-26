#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pixelcube.h"
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QProgressBar>
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pixmap = NULL;
    subDialog = NULL;
    textEdit = NULL;
    bar = NULL;

    filePath = QString("");
    filePaths = QStringList("");

    // set size of the pixel region
    cubeW = 10;
    cubeH = 10;


    // also set this size to the size selector
    ui->inputSizeW->setValue(cubeW);
    ui->inputSizeH->setValue(cubeH);

    status = "new"; // set status of the program

    // set default processing regions (full size)
    p1 = 0; p2 = 100; p3 = 0; p4 = 100;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent*e)
{
    // i dont use this func
}

/***************************** LOAD NEW IMAGE ******************************/

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
    curImg = img;

    // update status to track user behavior
    status = "loaded";

    // create new pixmap using this loaded image
    updatePixmap(*curImg);

}

/***************************** PIXELIZE IMAGE ******************************/

void MainWindow::on_btnPixelize_clicked() {

    // duplicate the  image (not yet implemented)
    pixelizedImg = curImg;

    updateRowNColAmount(*pixelizedImg);

    // loop through every pixels of the image cubewise
    for(int i = 0; i < pixelizedImg->width(); i += cubeW)
        for(int j = 0; j < pixelizedImg->height(); j += cubeH){

            int count = 0, r=0,g=0,b=0,a=0, totalMean=0;

            // loop through every pixels of the pixel cube
            for(int k = 0; k < cubeW; ++k)
                for(int l = 0; l < cubeH; ++l){

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
            int cubeWidth = cubeW, cubeHeight = cubeH;

            // replace the pixel of the cube by the new color (the same loop above)
            for(int k = 0; k < cubeW; ++k)
                for(int l = 0; l < cubeH; ++l)

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
            setPixelCube(i/cubeW, j/cubeH, *newcube);

            // delete the object (prevent memory leak)
            delete newcube;
        }

    // update status to track user behavior
    status = "pixelized";

    // create new pixmap using this new image
    updatePixmap(*pixelizedImg);

}

/***************************** PIXELATE IMAGE ******************************/

void MainWindow::on_btnArt_clicked()
{
    /* ----------------------- Multiple files Selection ----------------------*/

    // open multiple files, the user select a set of images
    // that he wants to be involed in pixelart process
    filePaths = QFileDialog::getOpenFileNames(
                this,
                "Select sample images",
                "d:\\CProject/PixelArt",
                "Images (*.png *.jpg *.jpeg)");

    if( filePaths.isEmpty() ) return;

    // open processing dialog to display progress
    processDialog();

    // opening promt
    textEdit->append("Starting..");

    // clear the previous sample images
    if(!imgList.isEmpty()) imgList.clear();

    // creat the images using the file paths
    // and add every images to a QVector<QImage>
    for (int i =0; i < filePaths.size(); i++){
        QImage *image = new QImage(filePaths.at(i));
        imgList.append(*image);
    }

    /* ----------------------- Sample Images Processing ----------------------*/

    // declare iteratior (for the loop)
    QVector<QImage>::iterator sampleImage;

    // loop through image list using qvector iterator
    for(sampleImage = imgList.begin(); sampleImage != imgList.end(); ++sampleImage){

        int count=0,red=0,green=0,blue=0,alpha=0,totalMean=0;

        // set the marks point (to define the region in the sample
        // image that will be processed to compute the mean) based
        // on the width and height of the image
        int mark1 = sampleImage->width()*p1/100;
        int mark2 = sampleImage->width()*p2/100;
        int mark3 = sampleImage->height()*p3/100;
        int mark4 = sampleImage->height()*p4/100;

        // find the most representative color from images
        // as what happened in pixelizing image function
        for(int m = mark1; m < mark2; ++m)
            for(int n = mark3; n < mark4; ++n){

                // convert the QRgb to QColor for color extracting process
                QColor color(sampleImage->pixel(m,n));

                // extract color channels using QColor built-in functions
                red += color.red();
                green += color.green();
                blue += color.blue();
                alpha += color.alpha();

                // count the iteration to get the total pixel of the cube
                count++;
            }

        // calculate mean color value of every channels
        red /= count; green /= count; blue /= count; alpha /= count;

        // store this combination of color in the sample color list
        sampleColorList.append(QColor::fromRgba(qRgba(red,green, blue,alpha)));

    }

    textEdit->append("Sample images loaded!");

    /* ----------------------- Pixel Cube Processing ----------------------*/

    textEdit->append("Begin processing..");

    // loop through the grid system (cubewise)
    // that we created previously during pixelizing process
    std :: vector < std :: vector < PixelCube > >::iterator cubeRow;
    std :: vector < PixelCube >::iterator cube;

    // use painter to join (by painting) this cube (image) into a big blank
    // image in order to have a big combination image after processing
    QPainter cubePainter;

    // duplicate new image from pixelized image
    artedImg = *curImg;

    // choose image to paint in
    cubePainter.begin(&artedImg);

    // index var for row vector loop (I can't find any proper way)
    int m=0;

    // iterator counter (in percentage) to track progress
    double counter = 0;

    for (cubeRow = grid.begin(); cubeRow != grid.end(); ++cubeRow){

        // index var for col vector loop
        int n=0;

        for (cube = cubeRow->begin(); cube != cubeRow->end(); ++cube){

            // passin the sample image mean list to find the bestMatchIndex
            // (it's also the index of best match image in the sample
            // image list) of this cube so that we can get the best
            // matched image in sample image list using this index
            QImage tempImg = imgList[(*cube).findBestMatchedIndex(sampleColorList)];

//            textEdit->append("Best matched image found!");

            // rotate image randomly to prevent resemble image poses
            // will be pixelated in same color area of the original picture
//            QMatrix rm;
//            int deg = (rand() % 4)*90;
//            rm.rotate(deg); // random between 0, 1pi, 2pi, 3pi
//            tempImg = tempImg.transformed(rm);  // rotate img

            // scale image to fit cube size before start drawing
            tempImg = tempImg.scaled(cubeW, cubeH);

//            textEdit->append("Image random pose processing.. " + QString::number(deg) + " degree rotated");

            // painting process
            cubePainter.drawImage(QRectF(m*cubeW,n*cubeH,cubeW,cubeH),
                              tempImg,
                              QRectF(0,0,cubeW,cubeH));

//            textEdit->append("-> Pixel cube " + QString::number((m+1)*(cols) + (n+1)) + " sucessfully rendered.");

            /* ----------------------- Update Loading bar ----------------------*/
            // update counter base on total number of pixel cubes (iterators)
            counter += 100/((double)(rows*cols));

            // round up the counter to be able to get 100% at the end of the process
            int progress = ceil(counter);

            // set value to progress bar
            bar->setValue(progress);

            /* -----------------------------------------------------------------*/

            // interfere the process to unfreeze UI due to long processing time
            QApplication::processEvents();

            n++; // increase col iterator index
        }
        m++; // increase row iterator index
    }

    // end painting (joint process)
    cubePainter.end();

    // update status to track user behavior
    status = "pixelated";

    // finishing promt
    textEdit->append("... \nfinalizing.. ");
    textEdit->append( QString::number(artedImg.width()) + "x" +
                      QString::number(artedImg.height()) + " dimension image.");
    textEdit->append( QString::number(filePaths.size()) + " sample images have been used.");
    textEdit->append( QString::number(rows*cols) + " pixel cubes have been transformed.");

    // create new pixmap using this big combination image
    updatePixmap(artedImg);

    // update processing dialog title
    subDialog->setWindowTitle("Done!");
    textEdit->append("Done!");

}

/***************************** SAVE IMAGE ******************************/

void MainWindow::on_btnSave_clicked()
{
    QString saveName = QFileDialog::getSaveFileName(this, tr("Save the picture"),
                                "",
                                tr("Images (*.png *.jpeg *.jpg)"));

    curImg->save(saveName);
}

/**************************** MISC. UTILS *****************************/

// create new pixmap for displaying from the processed image
void MainWindow::updatePixmap(QImage &processingImg){

    // update this var in order to track down which kind of image is being showed
    curImg = &processingImg;

    // delete old pixmap and update new one
    if(pixmap) delete pixmap;
    pixmap = new QPixmap;
    *pixmap = QPixmap ::fromImage(processingImg);

    // update viewport with new picture using QGraphicView
    QGraphicsScene* scene = new QGraphicsScene(ui->pictureViewport);
    scene->addPixmap(*pixmap);
    ui->pictureViewport->setScene(scene);
    ui->pictureViewport->show();

    // activate buttons
    if(status == "loaded"){
        ui->btnPixelize->setEnabled(true);
        ui->btnSave->setEnabled(true);
        ui->btnArt->setEnabled(false);
        ui->boxMode->setEnabled(false);
    }else if(status == "pixelized"){
        ui->btnArt->setEnabled(true);
        ui->boxMode->setEnabled(true);
    }
}

// processing dialog with a loading bar to display progress
void MainWindow::processDialog(){

    // kill existing dialog if it exists
//    if(subDialog) delete subDialog;
//    if(textEdit) delete textEdit;
//    if(bar) delete bar;

    // size of the dialog & progress bar
    int wD = 350, hD = 200, wP = 300, hP = 20, wT = 300, hT = 100;

    subDialog = new QDialog;
    subDialog->setWindowTitle("Processing..");
    subDialog->setGeometry(this->width()/2 - wD/2, this->height()/2 - hD/2, wD, hD);
    subDialog->show();

    // set a text box to print progress promt
    textEdit = new QTextEdit;
    textEdit->setParent(subDialog);
    textEdit->setEnabled(false);
    textEdit->setGeometry(subDialog->width()/2 - wT/2, 30, wT, hT);
    textEdit->show();

    // set a progress bar in this dialog
    bar = new QProgressBar;
    bar->setParent(subDialog);
    bar->setGeometry(subDialog->width()/2 - wP/2, subDialog->height() - 50, wP, hP);
    bar->setRange(0, 100);
    bar->show();
}

// input for user to choose size of pixel cubes
void MainWindow::on_inputSizeW_editingFinished()
{
    cubeW = ui->inputSizeW->value();
}

void MainWindow::on_inputSizeH_editingFinished()
{
    cubeH = ui->inputSizeH->value();
}

// set the mark point ratio upon combo box triggering
void MainWindow::on_boxMode_activated(const QString &mode)
{
    // from 0 to 100 (percentage). after passing in pixelcube constructor
    // I will devide them by 100 by then. this way I can avoid double casting here
    // rather than using 0, 1/4, 1/2, 3/4

    if(mode == "Fullsize"){ p1 = 0; p2 = 100; p3 = 0; p4 =100;}
    else if(mode == "Central"){ p1 = 25; p2 = 75; p3 = 25; p4 = 75;}
    else if(mode == "Top left"){ p1 = 0; p2 = 50; p3 = 0; p4 = 50;}
    else if(mode == "Top right"){ p1 = 0; p2 = 50; p3 = 50; p4 = 100;}
    else if(mode == "Bottom left"){ p1 = 50; p2 = 100; p3 = 0; p4 = 50;}
    else if(mode == "Bottom right"){ p1 = 50; p2 = 100; p3 = 50; p4 = 100;}
}

void MainWindow::updateRowNColAmount(QImage &processingImg){

    //set number of pixel cubes will be constructed using this
    //image, on both axises. Ceil will allow the program to count
    //the exception where the cubes are cut by the edge of the window
    //The program will still replace proper proper image onto these cubes normally
    setNumRows(ceil(processingImg.width() / (double)cubeW));
    setNumCols(ceil(processingImg.height() / (double)cubeH));

    //resize (init the size) the grid vector base on the number of cols and rows above
    grid.resize (rows, vector <PixelCube> (cols));
}

// exit button to close program during fullscreen mode
void MainWindow::on_btnExit_clicked()
{
    subDialog->close();
    this->close();
}



