#include "pixelcube.h"
#include <QColor>
#include <QDebug>

PixelCube :: PixelCube() {

    //do things here
}

PixelCube :: ~PixelCube() {

    //do things here
}

QImage& PixelCube::findResembleImage(QVector<QImage> il){

    // the shortest difference amount of color channels
    // with 255*4 as ini value (255 is maximum color value of a channel)
    // 4 is the total of channels
    int currentMatchDiff = 255*4;

    // the image that close to match with the cube color
    QImage currentBestMatchedImg;

    // declare iteratior (for the loop)
    QVector<QImage>::iterator it;

    // loop through image list using qvector iterator
    for(it = il.begin(); it != il.end(); ++it){

        int count=0,red=0,green=0,blue=0,alpha=0;
        int rDiff=0,gDiff=0,bDiff=0,aDiff=0,totalDiff=0;

        // find the most representative color from images
        // as what happened in pixelizing image function
        for(int m = 0; m < it->width(); ++m)
            for(int n = 0; n < it->height(); ++n){

                // convert the QRgb to QColor for color extracting process
                QColor color(it->pixel(m,n));

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

        // calculate the difference on each channel
        // by comparing them with the rgba value of the cube
        rDiff = abs(r-red);
        gDiff = abs(g-green);
        bDiff = abs(b-blue);
        aDiff = abs(a-alpha);

        // calculate sum of 4 differences
        totalDiff = rDiff + gDiff +bDiff +aDiff;
//        qDebug() << r;
//         qDebug() << red;

        // set the current difference and most matched image
        // to this case if the difference amount is smaller
        // than the current difference (the smaller the distance is
        // , the more the colors match each others)
        if(totalDiff < currentMatchDiff){
            currentMatchDiff = totalDiff;
            currentBestMatchedImg = *it;//.at(it - il.begin()); it - il.begin() extract current index
        }

    }

    // after searching the whole list of images
    // set the best matched image as an attribue of the cube
    bestMatchedImage = currentBestMatchedImg;


//    qDebug() << currentMatchDiff;
//    qDebug() << currentBestMatchedImg;

    // I like to return it too, so it will be convenient using it in action
    return bestMatchedImage;

}
