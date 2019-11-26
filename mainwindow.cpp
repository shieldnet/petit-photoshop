//QT Mainwindow
#include "mainwindow.h"
#include "ui_mainwindow.h"

//OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

//Basic Function Header
#include <algorithm>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace cv;

//BRG channel
//B=0, G=1, R=2 on OpenCV library
#define B 0
#define G 1
#define R 2


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Petit Photoshop");
    //setStyleSheet("background-color: grey;");
    ui->label_image->move(100, 100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void        MainWindow::updateLabel() {
    ui->label_image->setPixmap(pm);
}

void        MainWindow::openImage(const QString &filename) {
    std::cout << filename.toStdString() << std::endl;
    QFile tmp(filename);
    currentFile = filename;
    // TO DO check the extension and others errors..
    setLabelImage(filename);
}

void    MainWindow::openNewImage(const IMAGE_SIZE &format) {
    currentFile = "new_image.jpg";
    setLabelToNew(format);
}

void        MainWindow::setLabelImage(const QString &filename) {
    pm.load(filename);
    updateLabel();
    ui->label_image->resize(pm.width(), pm.height());
}

void    MainWindow::setLabelToNew(const IMAGE_SIZE &format) {
    switch(format) {
            case PORTRAIT:  pm = QPixmap(400, 700);
                            break;
            case DIAPORAMA: pm = QPixmap(800, 400);
                            break;
            case SQUARE:    pm = QPixmap(500, 500);
                            break;
            default:        pm = QPixmap(500, 500);
                            break;
    };
    pm.fill(Qt::white);
    updateLabel();
    ui->label_image->resize(pm.width(), pm.height());
}

void MainWindow::on_actionOpen_triggered()
{
    QString filter = "JPEG files (*.jpg)";
    QString filename = QFileDialog::getOpenFileName(this, "Choose an image", QDir::homePath(), filter);
    if (!filename.isEmpty()) {
        this->openImage(filename);
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString path = QFileDialog::getSaveFileName(0, tr("Save File"), currentFile);
    pm.save(path);
}

void MainWindow::on_actionNew_triggered()
{
    openNewImage(PORTRAIT);
}


void MainWindow::on_label_image_linkActivated(const QString &link)
{

}

void MainWindow::on_actionPortrait_triggered()
{
    openNewImage(PORTRAIT);
}

void MainWindow::on_actionDiaporama_triggered()
{
    openNewImage(DIAPORAMA);
}

void MainWindow::on_actionSquare_triggered()
{
    openNewImage(SQUARE);
}

void MainWindow::doGrayScale() {
    QImage image = pm.toImage();
    for(int y=0; y<image.height(); y++) {
        for(int x=0; x<image.width(); x++) {
            QColor color(image.pixel(x,y));
            int average = (color.blue() + color.red() + color.green()) / 3;
            color.setRgb(average, average, average);
            image.setPixel(x, y, color.rgba());
        }
    }
    pm = QPixmap::fromImage(image);
    updateLabel();
}

void MainWindow::on_actionBlack_White_triggered()
{
    if (pm.isNull()) {
        errorMessage("You must open an image before this action");
    }
    else {
        doGrayScale();
    }
}


void MainWindow::on_actionMosaic_triggered()
{
    if (pm.isNull()) {
        errorMessage("You must open an image before this action");
    }
    else {
        bool ok=false;
        int wsize = QInputDialog::getInt(this, tr("Input Window Size"),
                                         tr("Window Size:"), 25, 0, 100, 1, &ok);
        if(wsize<=0){
            errorMessage("You have to set positive integer for windowsize.");
        }
        else{
            doMosaic(wsize);
        }

    }
}

void MainWindow::doMosaic(int wsize){
    QImage image = pm.toImage();
    int height = image.height();
    int width  = image.width();
    int windowsize = wsize;
    Mat cv_image(image.height(), image.width(), CV_8UC3);

    QColor color;
    //Initialize Image
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            color= QColor(image.pixel(x,y));
            cv_image.at<Vec3b>(y,x)[B]=(unsigned char)color.blue();
            cv_image.at<Vec3b>(y,x)[G]=(unsigned char)color.green();
            cv_image.at<Vec3b>(y,x)[R]=(unsigned char)color.red();
        }
    }

    int y_step = height / windowsize;
    int x_step = width / windowsize;

    bool y_more=false, x_more = false;

    if(y_step*windowsize != height) y_more=true;
    if(x_step*windowsize != width) x_more=true;

    int y_from=0, y_to=0, x_from=0,x_to=0;
    for(int ystp=0;ystp<y_step;ystp++){
        y_from = ystp*windowsize; y_to = std::min(width,(ystp+1)*windowsize);
        x_from=0, x_to=0;
        for(int xstp=0;xstp<x_step;xstp++){
            x_from = xstp * windowsize;
            x_to = min(width,(xstp+1) * windowsize);

            int r_avg=0, b_avg =0, g_avg=0;

            //Sliding window
            for(int y=y_from; y<y_to;y++){
                for(int x = x_from; x<x_to;x++){
                    b_avg += cv_image.at<Vec3b>(y,x)[B];
                    g_avg += cv_image.at<Vec3b>(y,x)[G];
                    r_avg += cv_image.at<Vec3b>(y,x)[R];
                }
            }
            int area=(x_to-x_from)*(y_to-y_from);
            r_avg /= area; g_avg /= area; b_avg /= area;

            for(int y=y_from; y<y_to;y++){
                for(int x = x_from; x<x_to;x++){
                    cv_image.at<Vec3b>(y,x)[B] = b_avg;
                    cv_image.at<Vec3b>(y,x)[G] = g_avg;
                    cv_image.at<Vec3b>(y,x)[R] = r_avg;
                }
            }

        }
    }

    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            QColor color(image.pixel(x,y));
            int _b=cv_image.at<Vec3b>(y,x)[B];
            int _g=cv_image.at<Vec3b>(y,x)[G];
            int _r=cv_image.at<Vec3b>(y,x)[R];
            color.setRgb(_r,_g,_b);
            image.setPixel(x, y, color.rgba());
        }
    }

    pm = QPixmap::fromImage(image);
    update();
    updateLabel();
}


void MainWindow::draw(const QPoint & pos) {
        QPainter painter{&pm};
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen({Qt::blue, 2.0});
        painter.drawLine(lastPos, pos);
        lastPos = pos;
        update();
        updateLabel();
}

void    MainWindow::errorMessage(const QString &error) {
    messageBox.critical(0,"Error", error);
}

void MainWindow::on_actionPen_toggled(bool arg1)
{
    pen = arg1;
}
