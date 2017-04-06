#ifndef PIXELCUBE_H
#define PIXELCUBE_H
#include <QVector>
#include <QImage>

class PixelCube
{
public:
    // constructors
    PixelCube();
    PixelCube(int red, int green, int blue, int alpha){
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    // destructor
    ~PixelCube();

    // mutators
    void setRed(int red){ r = red; }
    void setGreen(int green){ g = green; }
    void setBlue(int blue){ b = blue; }
    void setAlpha(int alpha){ a = alpha; }
    void setBestMatchedImage(QImage *img){ bestMatchedImage = img; }

    // accessors
    int getRed(){ return r; }
    int getGreen(){ return g; }
    int getBlue(){ return b; }
    int getAlpha(){ return a; }
    QImage& getBestMatchedImage(){ return bestMatchedImage;}

    // function to find the best matched image in a list of images
    QImage& findResembleImage(QVector<QImage> *il);

private:
    int r,g,b,a;
    QImage *bestMatchedImage;
};

#endif // PIXELCUBE_H
