#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pixelcube.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent*);

    void setPixelCube (int i, int j, PixelCube cube)  { /*assert ( i < n_rows && j < n_cols );*/ grid[i][j] = cube; }

    PixelCube& getPixelCube (int i, int j) { /*assert ( i < n_rows && j < n_cols );*/ return grid[i][j]; }

private slots:
    void on_btnSave_clicked();

    void on_btnLoad_clicked();

    void on_btnPixelize_clicked();

    void updatePixmap();

    void on_btnArt_clicked();

private:
    Ui::MainWindow *ui;
    QImage *img;
    QString filePath; // path to the selected image
    QStringList filePaths;
    QPixmap *pixmap;  //pointer to pixamp
    int function;
    QSize alignParam;
    int iniPos;
    int cubeSize;
    int topOffset;
    QVector<QImage> *imgList;
    QVector<QImage>::iterator it;
    QVector<QRgb> *meanColorList;

    std :: vector < std :: vector < PixelCube > > grid;  // a resizable matrix of tiles
//    std :: vector < QPixmap* > tile_pixmaps;        // a resizable vector of pointers to QPixmaps.

};

#endif // MAINWINDOW_H
