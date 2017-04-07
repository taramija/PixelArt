#ifndef PIXELCUBE_H
#define PIXELCUBE_H
#include <QVector>
#include <QImage>

class PixelCube
{
public:
    // constructors
    PixelCube();
    PixelCube(int &red, int &green, int &blue, int &alpha){
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    // destructor
    ~PixelCube();

    // mutators
    void setRed(const int &red){ r = red; }
    void setGreen(const int &green){ g = green; }
    void setBlue(const int &blue){ b = blue; }
    void setAlpha(const int &alpha){ a = alpha; }
    void setBestMatchedImage(const QImage &img){ bestMatchedImage = img; }

    // accessors
    int getRed() const { return r; }
    int getGreen() const { return g; }
    int getBlue() const { return b; }
    int getAlpha() const { return a; }
    QImage& getBestMatchedImage() const { return bestMatchedImage;}

    // function to find the best matched image in a list of images
    QImage& findResembleImage(QVector<QImage> &il);

private:
    int r,g,b,a;
    QImage bestMatchedImage;
};

#endif // PIXELCUBE_H
