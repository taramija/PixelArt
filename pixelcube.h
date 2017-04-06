#ifndef PIXELCUBE_H
#define PIXELCUBE_H

class PixelCube
{
public:
    PixelCube();
    ~PixelCube();

    void setRed(int red){ r = red; }
    void setGreen(int green){ g = green; }
    void setBlue(int blue){ b = blue; }
    void setAlpha(int alpha){ a = alpha; }

    int getRed(){ return r; }
    int getGreen(){ return g; }
    int getBlue(){ return b; }
    int getAlpha(){ return a; }



private:
    int r,g,b,a;
};

#endif // PIXELCUBE_H
