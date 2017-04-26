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
    void updateRowNColAmount(QImage &processingImg);
    void processDialog();

    // signal for Cube Size input
    void on_inputSizeW_editingFinished();
    void on_inputSizeH_editingFinished();

    // signal for Mode dropdown list input
    void on_boxMode_activated(const QString &mode);

    // signal for Exit button
    void on_btnExit_clicked();

private:
    Ui::MainWindow *ui;

    QImage *img; // store original img
    QImage *pixelizedImg; // store pixelized img
    QImage artedImg; // store arted img
    QImage *curImg; // store current img being showed
    QPixmap *pixmap;

    QString filePath; // path to the selected image
    QStringList filePaths;  // list to store multiple selected images

    QString status;  // track user behaviors

    // ui components
    QGraphicsScene *pictureViewport;
    QDialog *subDialog;
    QTextEdit *textEdit;
    QProgressBar *bar;

    // points to locate region of the sample image to do avg calculation
    // this one change corresponding to combo box "mode" value in the ui
    int p1,p2,p3,p4;

    int cubeW;   // preset pixel cube width
    int cubeH;   // preset pixel cube height
    int cols, rows; // col & row number the image cubewise

    QVector< QImage > imgList;   // list to store multiple images
    QVector< QColor > sampleColorList;   // list to store color of multiple images
    std :: vector < std :: vector < PixelCube > > grid;  // store blocks of pixel

};

#endif // MAINWINDOW_H
