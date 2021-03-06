#include "pixelcube.h"
#include <QDebug>

PixelCube :: PixelCube() {

    //do things here
}

PixelCube :: ~PixelCube() {

    //do things here
}

int PixelCube::findBestMatchedIndex(QVector<QColor> sampleColors){

    // this is a temp variable to store the current shortest distance
    // between the color of the cube and color of sample images. This takes
    // 255*4 as ini value since 255*4 is the maximum color value of 4 channels
    int currentDiff = 255*4;

    // the index of the color that most close to the color of the cube
    int currentBestMatchedIndex;

    // declare iteratior (for the loop)
    QVector<QColor>::iterator it;

    // loop through sample color list using qvector iterator
    for(it = sampleColors.begin(); it != sampleColors.end(); ++it){

        int meanR = 0, meanG = 0, meanB = 0, meanA = 0, totalDiff = 0;

        // calculate the difference on all 4 channels
        // by comparing them with these 4 values of the cube
        meanR = abs(r - it->red());
        meanG = abs(g - it->green());
        meanB = abs(b - it->blue());
        meanA = abs(a - it->alpha());

        // sum the differences
        totalDiff = meanR + meanG + meanB + meanA;

        // set the current difference and the index of the iterator to this case
        // if the difference amount is smaller than the current difference (the
        // smaller the distance is, the more the colors match each others)
        if(totalDiff < currentDiff){
            currentDiff = totalDiff;
            currentBestMatchedIndex = it - sampleColors.begin(); // extract current index
        }

    }

    // after searching the whole list of sample color set we find the best matched index
    // (it's also be the index of the best matched image) as an attribue of the cube
    // I prefer to return it rather than store it, so it will be convenient using it in action
    return currentBestMatchedIndex;

}
