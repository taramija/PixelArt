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

    // mutator
    void setPixelCube (const int i, const int j, const PixelCube &cube)  { /*assert ( i < n_rows && j < n_cols );*/ grid[i][j] = cube; }

    // accessor
    PixelCube& getPixelCube (int i, int j) { /*assert ( i < n_rows && j < n_cols );*/ return grid[i][j]; }

private slots:
    void on_btnSave_clicked();

    void on_btnLoad_clicked();

    void on_btnPixelize_clicked();

    void on_btnArt_clicked();

private:
    Ui::MainWindow *ui;
    QImage *img;
    QPixmap *pixmap; // pointer to pixamp
    QString filePath; // path to the selected image
    QStringList filePaths;  // list to store multiple selected images
    QSize alignParam;   // to center the image in mainwindow (will be optimized later)

    int iniPos; // preset left offset of the image (will be calculated using alignParam
    int cubeSize;   // preset pixel cube size
    int topOffset;  // preset top offset of the image

    QVector<QImage> *imgList;   // list to store multiple images

    std :: vector < std :: vector < PixelCube > > grid;  // store blocks of pixel

    //    std :: vector < QPixmap* > tile_pixmaps;

};

#endif // MAINWINDOW_H
