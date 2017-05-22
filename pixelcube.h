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
    void setRed(const int &red){ r = red; }
    void setGreen(const int &green){ g = green; }
    void setBlue(const int &blue){ b = blue; }
    void setAlpha(const int &alpha){ a = alpha; }

    void setWidth(const int &width){ w = width; }
    void setHeight(const int &height){ h = height; }

    // accessors
    int getRed() const { return r; }
    int getGreen() const { return g; }
    int getBlue() const { return b; }
    int getAlpha() const { return a; }

    int getWidth() const { return w; }
    int getHeight() const { return h; }

    // function to find the best matched index in a list of
    // sample colors (extracted from the multiple sample images)
    int findBestMatchedIndex(QVector<QColor> sampleColors);

private:
    int r, g, b, a;
    int w, h;
};

#endif // PIXELCUBE_H
