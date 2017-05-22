#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pixelcube.h"
#include <QDebug>
#include <QFileDialog>
#include <QDirIterator>
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){

    ui->setupUi(this);

    /* ------------- ini starting values ------------ */

    pixmap = NULL;
    pictureViewport = NULL;
    subDialog = NULL;
    textEdit = NULL;
    bar = NULL;
    dir = NULL;
    rndPose = false;
    keepSampleImage = false;

    status = "";
    filePath = QString("");

    // set size of the pixel region
    cubeW = 5;
    cubeH = 5;

    // also set this size to the size selector
    ui->inputSizeW->setValue(cubeW);
    ui->inputSizeH->setValue(cubeH);

    // set status of the program
    status = "new";

    // set default processing regions (full size)
    p1 = 0; p2 = 100; p3 = 0; p4 = 100;

    /* --------------- show about page ---------------- */

    QImage *aboutPage = new QImage();

    aboutPage->load("D://CProject/PixelArt/extras/about.jpg");

    aboutPage->scaled(496,470);

    ui->aboutLabel->setPixmap(QPixmap::fromImage(*aboutPage));

}

MainWindow::~MainWindow(){
    delete ui;
}

/***************************** LOAD NEW IMAGE ******************************/

void MainWindow::on_btnLoad_clicked(){
    // close the dialog
    if(subDialog) subDialog->close();

    // same as drawing in Label: we first get the path to an image to be drawn
    filePath = QFileDialog::getOpenFileName(
                this,
                "Select an image",
                "d:\\CProject/PixelArt",
                "Images (*.png *.jpg *.jpeg)");

    if (filePath.isEmpty()) return;

    // hide about page upon new image loading
    ui->aboutLabel->hide();

    img = new QImage(filePath);

    // update status to track user behavior
    status = "loaded";

    // create new pixmap using this loaded image
    updateViewport(*img);
}

/***************************** PIXELIZE IMAGE ******************************/

void MainWindow::on_btnPixelize_clicked() {

    // cube sizes validators
    // abort the function if cubesize got 0 in values and highlight the cube size selectors
    if(cubeW == 0 || cubeH == 0){   // we don't care about negative cases since QSpinBox takes only positive numbers

        if(cubeW == 0){
            ui->inputSizeW->setStyleSheet("QSpinBox { color : red; }");
            ui->inputSizeH->setStyleSheet("QSpinBox { color : black; }");
        }

        if(cubeH == 0){
            ui->inputSizeH->setStyleSheet("QSpinBox { color : red; }");
            ui->inputSizeW->setStyleSheet("QSpinBox { color : black; }");
        }

        return;
    }else{
        // unhighlight the selectors if the sizes were valid
        ui->inputSizeW->setStyleSheet("QSpinBox { color : black; }");
        ui->inputSizeH->setStyleSheet("QSpinBox { color : black; }");
    }


    // close the dialog
    if(subDialog) subDialog->close();

    // duplicate the  image (not yet implemented)
    pixelizedImg = curImg;

    int imgW = pixelizedImg->width();
    int imgH = pixelizedImg->height();

    // update the size of the grid
    updateRowNColAmount(*pixelizedImg);

    // loop through every pixels of the image cubewise
    for(int i = 0; i < rows; ++i)
        for(int j = 0; j < cols; ++j){

            int count = 0, r = 0, g = 0, b = 0, a = 0;

            // loop through every pixels of the pixel cube
            for(int k = 0; k < cubeH; ++k)
                for(int l = 0; l < cubeW; ++l){

                    // stopping criterion in case the last cube
                    // that compute data out of image boundary
                    if ( j*cubeW+l < imgW && i*cubeH+k < imgH ){

                        // convert the QRgb to QColor for color extracting process
                        QColor color(pixelizedImg->pixel(j*cubeW+l, i*cubeH+k));

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
            int     cubeWidth  = min(cubeW, imgW - j*cubeW),
                    cubeHeight = min(cubeH, imgH - i*cubeH);

            // replace the pixel of the cube by the new color (the same loop above)
            for(int k = 0; k < cubeH; ++k)
                for(int l = 0; l < cubeW; ++l)

                    // same stopping criterion as above
                    if ( j*cubeW+l < imgW && i*cubeH+k < imgH )
                        pixelizedImg->setPixel(j*cubeW+l, i*cubeH+k, meanColor);


            // transform this block of pixel as an instance of PixelCube for PixelArt function
            PixelCube *newcube = new PixelCube(r,g,b,a);

            // set width height for this cube
            newcube->setWidth(cubeWidth);
            newcube->setHeight(cubeHeight);

            // store this PixelCube on the 2 dimensions vector (Grid) of
            // mainWindow using the "normalized" i and j indexes
            setPixelCube(i, j, *newcube);

            // delete the object (prevent memory leak)
            delete newcube;

            // interfere the process to unfreeze UI due to long processing time
            QApplication::processEvents();
        }

    // update status to track user behavior
    status = "pixelized";

    // create new pixmap using this new image
    updateViewport(*pixelizedImg);

}

/***************************** PIXELART IMAGE ******************************/

void MainWindow::on_btnArt_clicked(){

    // lock the buttons until the function ended
    lockButtons(true);

    // close the dialog
    if(subDialog) subDialog->close();


    /* ----------------------- Multiple files Selection ----------------------*/

    // if the sample images has been already loaded before, skip this part
    if(!keepSampleImage){

        // open multiple files, the user select a set of images
        // that he wants to be involed in pixelart process
        QString dirPath = QFileDialog::getExistingDirectory(
                    this,
                    "Select sample images",
                    "d:\\CProject/PixelArt",
                    QFileDialog::ShowDirsOnly);


        // escape the function if the user did not select any file
        if(dirPath.isEmpty()) {
            // unlock the button on this case
            lockButtons(false);
            return;
        }

        // open processing dialog to display progress
        processDialog();

        // print message to dialog
        textEdit->append("Started.");
        textEdit->append("Loading sample images..");

        // create QDir using path extracted from QFileDialog
        dir.setPath(dirPath);

        // filter the files inside this QDir
        dir.setNameFilters(QStringList()<<"*.jpg" <<"*.png");

        // extract relative path of all images inside QDir
        QStringList relativePaths = dir.entryList();

        // clear the previous sample images / mean colors
        if(!imgList.isEmpty()) imgList.clear();
        if(!sampleColorList.isEmpty()) sampleColorList.clear();

        // iterator counter (in percentage) to track progress
        progressCounter = 0;

        // create the images using the file paths and add them to vector imgList
        for (int i =0; i < relativePaths.size(); i++){

            // convert relative paths to absolute path to use it to create QImage
            QString absImgPath = dir.absoluteFilePath(relativePaths[i]);
            QImage *image = new QImage(absImgPath);

            imgList.append(image);

            //delete image;   // delete the dynamical instance to free memory

            // update progress bar value
            updateProgressBar(dir.count());

            // interfere the process to unfreeze UI due to long processing time
            QApplication::processEvents();
        }

        textEdit->append(" -> Sample images loaded!");

        /* ----------------------- Sample Images Processing ----------------------*/

        textEdit->append("Processing sample images..");

        // declare iteratior (for the loop)
        QVector<QImage*>::iterator sampleImage;

        // reset progress bar counter variable
        progressCounter = 0;

        // loop through image list using qvector iterator
        for(sampleImage = imgList.begin(); sampleImage != imgList.end(); ++sampleImage){

            int count = 0, red = 0, green = 0, blue = 0, alpha = 0;

            // set the marks point (to define the region in the sample
            // image that will be processed to compute the mean) based
            // on the width and height of the image
            int mark1 = (*sampleImage)->width()*p1/100;
            int mark2 = (*sampleImage)->width()*p2/100;
            int mark3 = (*sampleImage)->height()*p3/100;
            int mark4 = (*sampleImage)->height()*p4/100;

            // find the most representative color from images
            // as what happened in pixelizing image function
            for(int m = mark1; m < mark2; ++m)
                for(int n = mark3; n < mark4; ++n){

                    // convert the QRgb to QColor for color extracting process
                    QColor color((*sampleImage)->pixel(m,n));

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

            // update progress bar value
            updateProgressBar(dir.count());

            // interfere the process to unfreeze UI due to long processing time
            QApplication::processEvents();

        }

        textEdit->append(" -> Sample images info extracted!");

        // resize vectors to adapt new datas, the program will crash
        // without resizing these dynamic variables on the second attempt
        imgList.resize(dir.count());
        sampleColorList.resize(dir.count());

    } else processDialog();

    /* ----------------------- Pixel Cube Processing ----------------------*/

    textEdit->append("Begin replacement processing..");

    // loop through the grid system (cubewise)
    // that we created previously during pixelizing process
    std :: vector < std :: vector < PixelCube > >::iterator cubeRow;
    std :: vector < PixelCube >::iterator cube;

    // create a painter to join (by painting) best matched image on a big
    // canvas in order to have a big combination image after processing
    QPainter cubePainter;

    // duplicate new image from pixelized image then use it as a canvas
    artedImg = *pixelizedImg;

    // choose the canvas to paint in
    cubePainter.begin(&artedImg);

    // index var for row vector loop (I can't find any proper way)
    int m = 0;

    // reset progress bar counter variable
    progressCounter = 0;

    // loop through the grid (matrix of PixelCube)
    for (cubeRow = grid.begin(); cubeRow != grid.end(); ++cubeRow){

        // index var for col vector loop
        int n=0;

        for (cube = cubeRow->begin(); cube != cubeRow->end(); ++cube){

            // passin the sample image color list to find the bestMatchIndex
            // (it's also the index of best match image in the sample
            // image list) of this cube so that we can get the best
            // matched image in sample image list using this index
            QImage *bestImg = imgList[(*cube).findBestMatchedIndex(sampleColorList)];

            // rotate image randomly to prevent resemble image poses
            // will be pixelated in same color area of the original picture
            if(rndPose){
                QMatrix rm;
                int deg = (rand() % 4)*90;              // random between 0, 1pi, 2pi, 3pi
                rm.rotate(deg);
                *bestImg = bestImg->transformed(rm);    // rotate img using QMatrix
            }

            // scale image to fit cube size before start drawing
            if( bestImg->width()/cubeW <  bestImg->height()/cubeH )
                *bestImg = bestImg->scaledToWidth(cubeW);

            else if( bestImg->width()/cubeW >  bestImg->height()/cubeH )
                *bestImg = bestImg->scaledToHeight(cubeH);

            else
                *bestImg = bestImg->scaled(cubeW,cubeH);

            // painting process
            cubePainter.drawImage(QRectF(n*cubeW,m*cubeH,cubeW,cubeH),
                              *bestImg,
                                  // with this coordinator for source image, painter could be able
                                  // to take the center region of the image that will be processed
                              QRectF(bestImg->width()/2-cubeW/2, bestImg->height()/2-cubeH/2, cubeW, cubeH));

            // update progress bar value
            updateProgressBar(rows*cols);

            // interfere the process to unfreeze UI due to long processing time
            QApplication::processEvents();

            n++; // increase col iterator index
        }
        m++; // increase row iterator index
    }

    /* ---------------------------- Watermark Painting ---------------------------*/

    // in this part I'll load the 2 versions of my watermark based on the
    // average color of the zone where I put the watermark. If the zone is
    // dark, I'll load the white watermark, otherwise I'll load the black one
    QImage *watermark = new QImage();

    int wtmWidth = curImg->width()/20;
    int wtmHeight = curImg->height()/20;

    int wtmPosX = curImg->width()-curImg->width()/10;
    int wtmPosY = curImg->height()-curImg->height()/10;

    // same process to calculate average color
    int wtmCount = 0, wtmR = 0, wtmG = 0, wtmB = 0, wtmA = 0, wtmTotalMean = 0;

    for(int x = wtmPosX; x < wtmPosX + wtmWidth; x++)
        for(int y = wtmPosY; y < wtmPosY + wtmHeight; y++){
            QColor wtmColor(artedImg.pixel(x,y));
            wtmR += wtmColor.red();
            wtmG += wtmColor.green();
            wtmB += wtmColor.blue();
            wtmA += wtmColor.alpha();
            wtmCount++;
        }

    wtmR /= wtmCount; wtmG /= wtmCount; wtmB /= wtmCount; wtmA /= wtmCount;
    wtmTotalMean = wtmR + wtmG + wtmB + wtmA;

    if(wtmTotalMean < 500)
        watermark->load("D://CProject/PixelArt/extras/wtm_w.png");   // load white watermark
    else
        watermark->load("D://CProject/PixelArt/extras/wtm_b.png");   // load black watermark

    // paint watermark onto the image
    cubePainter.drawImage(  QRectF(wtmPosX, wtmPosY, wtmWidth, wtmHeight),
                            watermark->scaled(wtmWidth,wtmHeight),
                            QRectF(0,0,wtmWidth,wtmHeight)                  );

    delete watermark;


    /* --------------------- Ending Process / Paint final image --------------------*/

    // end painting (close the canvas)
    cubePainter.end();

    // update status to track user behavior
    status = "pixelated";

    // finishing promt
    textEdit->append("... \nfinalizing.. ");
    textEdit->append( QString::number(artedImg.width()) + "x" +
                      QString::number(artedImg.height()) + " dimension image.");
    textEdit->append( QString::number(dir.count()) + " sample images have been used.");
    textEdit->append( QString::number(rows*cols) + " pixel cubes have been transformed.");

    // create new pixmap using this big combination image
    updateViewport(artedImg);

    // update processing dialog title
    subDialog->setWindowTitle("Done!");
    textEdit->append("Done!");

    // unlock the buttons
    lockButtons(false);

}


/**************************** MISC. COMPONENTS *****************************/

// pixel cube size input
void MainWindow::on_inputSizeW_editingFinished(){
    cubeW = ui->inputSizeW->value();
}

void MainWindow::on_inputSizeH_editingFinished(){
    cubeH = ui->inputSizeH->value();
}

// random pose option checker
void MainWindow::on_rdnPose_toggled(bool checked){
    checked == true ? rndPose = true : rndPose = false;
}

// reset the original image
void MainWindow::on_btnReset_clicked(){
    // close the dialog
    if(subDialog) subDialog->close();

    img = new QImage(filePath);

    // update status to track user behavior
    status = "loaded";

    // create new pixmap using original image
    updateViewport(*img);
}

// save image button
void MainWindow::on_btnSave_clicked(){
    // close the dialog
    if(subDialog) subDialog->close();

    QString saveName = QFileDialog::getSaveFileName(this, tr("Save the picture"),
                                "",
                                tr("Images (*.png *.jpeg *.jpg)"));

    curImg->save(saveName);
}

// exit program button
void MainWindow::on_btnExit_clicked(){
    QCoreApplication::exit();
}


/**************************** UTILS FUNCTIONS *****************************/

// create new pixmap for displaying from the processed image
void MainWindow::updateViewport(QImage &processingImg){

    // update this var in order to track down which kind of image is being showed
    curImg = &processingImg;

    // delete old pixmap and update new one
    if(pixmap) delete pixmap;
    pixmap = new QPixmap;
    *pixmap = QPixmap ::fromImage(processingImg);

    // update viewport with new picture using QGraphicView
    if(pictureViewport) delete pictureViewport;
    pictureViewport     = new QGraphicsScene;
    pictureViewport     ->addPixmap(*pixmap);
    ui->pictureViewport ->setScene(pictureViewport);
    ui->pictureViewport ->show();

    // activate buttons
    if(status == "loaded"){
        ui->btnPixelize ->setEnabled    (true );
        ui->btnSave     ->setEnabled    (true );
        ui->btnArt      ->setEnabled    (false);
        ui->boxMode     ->setEnabled    (false);

    }else if(status == "pixelized"){
        ui->btnArt      ->setEnabled    (true );
        ui->boxMode     ->setEnabled    (true );
    }
}

// set the mark point ratio upon combo box triggering
void MainWindow::on_boxMode_activated(const QString &mode){
    // 4 mark points have value from 0 to 100 (percentage). But currently they are integers,
    // I will devide them by 100 to get percentage after I pass it in the sample image process.
    // Rather than using 0, 1/4, 1/2, 3/4, I can avoid double casting this way.

    if(mode == "Fullsize")          { p1 = 0;   p2 = 100;   p3 = 0;     p4 =100;}
    else if(mode == "Central")      { p1 = 25;  p2 = 75;    p3 = 25;    p4 = 75;}
    else if(mode == "Top left")     { p1 = 0;   p2 = 50;    p3 = 0;     p4 = 50;}
    else if(mode == "Top right")    { p1 = 0;   p2 = 50;    p3 = 50;    p4 = 100;}
    else if(mode == "Bottom left")  { p1 = 50;  p2 = 100;   p3 = 0;     p4 = 50;}
    else if(mode == "Bottom right") { p1 = 50;  p2 = 100;   p3 = 50;    p4 = 100;}
}

// empty and resize the vectors
void MainWindow::updateRowNColAmount(QImage &processingImg){

    // set number of pixel cubes will be constructed using this
    // image, on both axises. If condition on remainder checking of picture
    // width + cube division will count the rows and cols include the exception
    // case where the cubes are cut by the edge of the window.
    setNumRows(ceil(processingImg.height()  / (double)cubeH));
    setNumCols(ceil(processingImg.width() / (double)cubeW));

    // resize (init the size) the grid vector base on the number of cols and rows above
    if(!grid.empty()) grid.clear();
    grid.resize (rows, vector <PixelCube> (cols));
}

// processing dialog with a loading bar to display progress
void MainWindow::processDialog(){

    // kill existing dialog if it exists
    if(textEdit) delete textEdit;
    if(bar) delete bar;
    if(subDialog) delete subDialog;

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

// update progress bar value
void MainWindow::updateProgressBar(int size){

    // update counter value based on total number of iteration need to be done
    progressCounter +=  100/((double)(size));

    // round up the counter to be able to get 100% at the end of
    // the process and set its value to progress bar
    bar->setValue(ceil(progressCounter));
}

// keep loaded sample images for next processing
void MainWindow::on_sampleKeep_toggled(bool checked){
    checked == true ? keepSampleImage = true : keepSampleImage = false;
}

// lock the ui buttons
void MainWindow::lockButtons(bool lock){

    // invert bool value since setEnabled function
    // has the opposite logic with lock parameter
    ui->btnLoad->setEnabled(!lock);
    ui->btnReset->setEnabled(!lock);
    ui->btnPixelize->setEnabled(!lock);
    ui->btnArt->setEnabled(!lock);
    ui->btnSave->setEnabled(!lock);
}
