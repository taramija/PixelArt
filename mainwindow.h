#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_btnSave_clicked();

    void on_btnLoad_clicked();

    void on_btnPixelize_clicked();

private:
    Ui::MainWindow *ui;
    QImage *img;
    QString filePath; // path to the selected image
    QPixmap *pixmap;  //pointer to pixamp
    int function;
};

#endif // MAINWINDOW_H
