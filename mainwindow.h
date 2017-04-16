#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QProgressBar>
#include <cassert>
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
    void setPixelCube (const int i, const int j, const PixelCube &cube)  { assert( i < rows && j < cols ); grid[i][j] = cube; }
    void setNumRows(const int &numRows)         { rows = numRows; }
    void setNumCols(const int &numCols)         { cols = numCols; }

    // accessor
    PixelCube& getPixelCube (int i, int j) { assert( i < rows && j < cols ); return grid[i][j]; }

    int getNumRows() const        { return rows; }
    int getNumCols() const        { return cols; }

private slots:
    void on_btnSave_clicked();
    void on_btnLoad_clicked();
    void on_btnPixelize_clicked();
    void on_btnArt_clicked();
    void updatePixmap(QImage &processingImg);
    void processDialog();

    void on_inputSize_editingFinished();

    void on_btnExit_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *pictureViewport;
    QImage *img;
    QImage *pixelizedImg;
    QImage artedImg;
    QImage *curImg; // for saving process
    QPixmap *pixmap; // pointer to pixamp
    QString filePath; // path to the selected image
    QStringList filePaths;  // list to store multiple selected images
    QSize alignParam;   // to center the image in mainwindow (will be optimized later)
    QString status;  // track user behaviors
    QDialog *subDialog;
    QProgressBar *bar;

    int iniPos; // preset left offset of the image (will be calculated using alignParam
    int topOffset;  // preset top offset of the image
    int cubeSize;   // preset pixel cube size
    int cols, rows; // col & row number the image cubewise

    QVector<QImage> imgList;   // list to store multiple images
    std :: vector < std :: vector < PixelCube > > grid;  // store blocks of pixel

    //    std :: vector < QPixmap* > tile_pixmaps;

};

#endif // MAINWINDOW_H
