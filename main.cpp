#include "mainwindow.h"
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDesktopWidget dw;
    MainWindow w;
    int x=dw.width()*0.7;
    int y=dw.height()*0.7;
    w.setFixedSize(x,y);
    w.show();
    return a.exec();
}
