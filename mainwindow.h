#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QTextEdit>
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

    /* mutators */
    void setPixelCube (const int i, const int j, const PixelCube &cube)  {
        assert( i < rows && j < cols ); grid[i][j] = cube;
    }
    void setNumRows(const int &numRows){ rows = numRows; }
    void setNumCols(const int &numCols){ cols = numCols; }

    /* accessors */
    PixelCube& getPixelCube (int i, int j) {
        assert( i < rows && j < cols ); return grid[i][j];
    }
    int getNumRows() const { return rows; }
    int getNumCols() const { return cols; }

    /* resize QHBoxLayout */
    void resizeLayout();

private slots:
    /* button actions */
    void on_btnLoad_clicked();                          // load image
    void on_btnPixelize_clicked();                      // pixelizing
    void on_btnArt_clicked();                           // pixelart
    void on_btnSave_clicked();                          // save image
    void on_btnExit_clicked();                          // exit program

    /* util functions */
    void updatePixmap(QImage &processingImg);           // update new pixmap
    void updateRowNColAmount(QImage &processingImg);    // update grid size
    void processDialog();                               // open progress dialog
    void on_inputSizeW_editingFinished();               // cube width input
    void on_inputSizeH_editingFinished();               // cube height input
    void on_boxMode_activated(const QString &mode);     // mode dropdown list

    void on_rdnPose_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    QImage *img;                                        // store original img
    QImage *pixelizedImg;                               // store pixelized img
    QImage artedImg;                                    // store arted img
    QImage *curImg;                                     // store current img being showed
    QPixmap *pixmap;                                    // store current pixmap

    QString filePath;                                   // path to the selected image
    QStringList filePaths;                              // list to store multiple selected images

    QString status;                                     // track user behaviors
    bool rndPose;                                       // track random pose status

    /* ui components */
    QGraphicsScene *pictureViewport;
    QDialog *subDialog;
    QTextEdit *textEdit;
    QProgressBar *bar;

    // points to locate region of the sample image to do avg calculation
    // this one change corresponding to combo box "mode" value in the ui
    int p1,p2,p3,p4;

    int cubeW;                                          // preset pixel cube width
    int cubeH;                                          // preset pixel cube height
    int cols, rows;                                     // col & row number the image cubewise

    QVector < QImage > imgList;                         // list to store multiple images
    QVector < QColor > sampleColorList;                 // list to store color of multiple images
    std :: vector < std :: vector < PixelCube > > grid; // store blocks of pixel

};

#endif // MAINWINDOW_H
