#include "pixelcube.h"

QImage& PixelCube::findResembleImage(QVector<QImage> *il){

    // the shortest difference amount of color channels
    // with 255*4 as ini value (255 is maximum color value of a channel)
    // 4 is the total of channels
    int currentMatchDiff = 255*4;

    // the image that close to match with the cube color
    QImage currentBestMatchedImg = NULL;

    // declare iteratior (for the loop)
    QVector<QImage>::iterator it;

    // loop through image list using qvector iterator
    for(it = il.begin(); it != il.end(); it++){

        int count,r=0,g=0,b=0,a=0;
        int rDiff,gDiff,bDiff,aDiff,totalDiff;

        // find the most representative color from images
        // as what happened in pixelizing image function
        for(int m = 0; m < it->width(); m++)
            for(int n = 0; n < it->height(); n++){

                // convert the QRgb to QColor for color extracting process
                QColor color(it->pixel(m,n));

                // extract color channels using QColor built-in functions
                r += color.red();
                g += color.green();
                b += color.blue();
                a += color.alpha();

                // count the iteration to get the total pixel of the cube
                count++;
            }

        // calculate mean color value of every channels
        r /= count; g /= count; b /= count; a /= count;

        // calculate the difference on each channel
        // by comparing them with the rgba value of the cube
        rDiff = abs(this->getRed()-r);
        gDiff = abs(this->getGreen()-g);
        bDiff = abs(this->getBlue()-b);
        aDiff = abs(this->getAlpha()-a);

        // calculate sum of 4 differences
        totalDiff = rDiff + gDiff +bDiff +aDiff;

        // set the current difference and most matched image
        // to this case if the difference amount is smaller
        // than the current difference (the smaller the distance is
        // , the more the colors match each others)
        if(totalDiff < currentMatchDiff){
            currentMatchDiff = totalDiff;
             currentBestMatchedImg = il[it];
        }

    }

    // after searching the whole list of images
    // set the best matched image as an attribue of the cube
    this->setBestMatchedImage(currentBestMatchedImg);

    // I like to return it too, so it will be convenient using it in action
    return currentBestMatchedImg;

}
