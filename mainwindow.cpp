//QT Mainwindow
#include "mainwindow.h"
#include "ui_mainwindow.h"

//OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core.hpp>
//Basic Function Header
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <vector>

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
                                         tr("Window Size:"), 25, 0, 9999, 1, &ok);
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

void MainWindow::on_actionEdge_Detection_Sobel_triggered(){
    if (pm.isNull()) {
        errorMessage("You must open an image before this action");
    }
    else {
        bool ok=false;
        int threshold = QInputDialog::getInt(this, tr("Edgedetection Threshold Percent(0-100)"),
                                         tr("Percent"), 25, 0, 255, 1, &ok);
        if(threshold<=0){
            errorMessage("You have to set positive integer for windowsize.");
        }
        else{
            doEdgeDetectionSobel(threshold);
        }

    }
}

void MainWindow::doEdgeDetectionSobel(int threshold_percent){
    QImage image = pm.toImage();
    int height = image.height();
    int width  = image.width();
    Mat cv_image(image.height(), image.width(), CV_8UC3);
    vector<vector<int> > arr(height, vector<int>(width, 0));
    QColor color;
    //Initialize Image (QImage to Mat)
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            color= QColor(image.pixel(x,y));
            cv_image.at<Vec3b>(y,x)[B]=(unsigned char)color.blue();
            cv_image.at<Vec3b>(y,x)[G]=(unsigned char)color.green();
            cv_image.at<Vec3b>(y,x)[R]=(unsigned char)color.red();
        }
    }

    //Apply Masks(Convolution)
    const int dy[9] = { 0,0,1,1,1,0,-1,-1,-1 };
    const int dx[9] = { 0,1,1,0,-1,-1,-1,0,1 };

    //Filter
    int filter[4][3][3]={
        {
            {-1,-2,-1},
            {0,0,0},
            {1,2,1}
        },
        {
            {-1,0,1},
            {-2,0,2},
            {-1,0,1}
        },
        {
            {0,1,2},
            {-1,0,1},
            {-2,-1,0}
        },
        {
            {-2,-1,0},
            {-1,0,1},
            {0,1,2}
        }
    };

    int ref_table[256]={0,};
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            bool isEdge[4]={false,};
            for(int f=0;f<4;f++){
                int t_inten=0;
                for(int i=0;i<9;i++){
                    int ddy=y+dy[i], ddx=x+dx[i];
                    if(ddy>=height||ddy<0||ddx<0||ddx>=width)continue;
                    int _b=cv_image.at<Vec3b>(y,x)[B];
                    int _g=cv_image.at<Vec3b>(y,x)[G];
                    int _r=cv_image.at<Vec3b>(y,x)[R];
                    t_inten += (_b+_g+_r)/3;
                }
                t_inten/=9;
                arr[y][x]=max(arr[y][x],t_inten);
            }
            ref_table[arr[y][x]]++;
        }
    }

    //Set the threshold by pixel intensity frequency
    int prefix_sum[256]={0,}; prefix_sum[0]=ref_table[0];
    for(int i=0;i<256;i++){prefix_sum[i]+=prefix_sum[i-1]+ref_table[i];}
    int final_threshold = 0;
    int total_pixels=width*height;
    double ratio=0;
    for(int _th=0;_th<256;_th++){
        ratio=(double)prefix_sum[_th]/(double)total_pixels;
        if(ratio>=((double)threshold_percent/100.0)){
            final_threshold=_th;
            break;
        }
    }
    //cout << final_threshold<<"\n";

    //Threshold apply
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            bool _isedge = (arr[y][x]>=final_threshold);
                cv_image.at<Vec3b>(y,x)[B]=_isedge ? 0 : 255;
                cv_image.at<Vec3b>(y,x)[R]=_isedge ? 0 : 255;
                cv_image.at<Vec3b>(y,x)[G]=_isedge ? 0 : 255;
        }
    }

    //Mat to QImage
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
    arr.clear();
    pm = QPixmap::fromImage(image);
    update();
    updateLabel();

}

void MainWindow::on_actionEdge_Detection_Canny_triggered(){
    if (pm.isNull()) {
        errorMessage("You must open an image before this action");
    }
    else {
        doEdgeDetectionCanny(50,200);
    }
}


void MainWindow::doEdgeDetectionCanny(int upperThreshold, int lowerThreshold){
    QImage image = pm.toImage();
    int height = image.height();
    int width  = image.width();
    Mat cv_image(image.height(), image.width(), CV_8UC1);
    vector<vector<int> > arr(height, vector<int>(width, 0));
    QColor color;
    //Initialize Image (QImage to Mat)
    cout << height << " " << width << "\n";
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            color= QColor(image.pixel(x,y));
            int _r = color.red(), _g=color.green(), _b=color.blue();
            cv_image.at<unsigned char>(y,x)=(_r+_g+_b)/3;
        }
    }
    Mat workImg = cv_image.clone();
    GaussianBlur(cv_image, workImg, Size(5, 5), 1.4);

    Mat magX = Mat(cv_image.rows, cv_image.cols, CV_32F);
    Mat magY = Mat(cv_image.rows, cv_image.cols, CV_32F);


    Sobel(workImg, magX, CV_32F, 1, 0, 3);
    Sobel(workImg, magY, CV_32F, 0, 1, 3);


    Mat direction = Mat(workImg.rows, workImg.cols, CV_32F);

    divide(magY, magX, direction);

    Mat sum = Mat(workImg.rows, workImg.cols, CV_64F);
    Mat prodX = Mat(workImg.rows, workImg.cols, CV_64F);
    Mat prodY = Mat(workImg.rows, workImg.cols, CV_64F);

    multiply(magX, magX, prodX);
    multiply(magY, magY, prodY);

    sum = prodX + prodY;

    sqrt(sum, sum);

    Mat returnImg = Mat(cv_image.rows, cv_image.cols, CV_8U);

    returnImg.setTo(Scalar(0));         // Initialie image to return to zero

    // Initialize iterators
    MatIterator_<float>itMag = sum.begin<float>();
    MatIterator_<float>itDirection = direction.begin<float>();

    MatIterator_<unsigned char>itRet = returnImg.begin<unsigned char>();

    MatIterator_<float>itend = sum.end<float>();

    for(;itMag!=itend;++itDirection, ++itRet, ++itMag){
        const Point pos = itRet.pos();

        float currentDirection = atan(*itDirection) * 180 / 3.142;

        while(currentDirection<0) currentDirection+=180;

        *itDirection = currentDirection;

        if(*itMag<upperThreshold) continue;

        bool flag = true;

        if(currentDirection>112.5 && currentDirection <=157.5) {
            if(pos.y>0 && pos.x<workImg.cols-1 && *itMag<=sum.at<float>(pos.y-1, pos.x+1)) flag = false;
            if(pos.y<workImg.rows-1 && pos.x>0 && *itMag<=sum.at<float>(pos.y+1, pos.x-1)) flag = false;
        }
        else if(currentDirection>67.5 && currentDirection <= 112.5)
        {
            if(pos.y>0 && *itMag<=sum.at<float>(pos.y-1, pos.x)) flag = false;
            if(pos.y<workImg.rows-1 && *itMag<=sum.at<float>(pos.y+1, pos.x)) flag = false;
        }
        else if(currentDirection > 22.5 && currentDirection <= 67.5)
        {
            if(pos.y>0 && pos.x>0 && *itMag<=sum.at<float>(pos.y-1, pos.x-1)) flag = false;
            if(pos.y<workImg.rows-1 && pos.x<workImg.cols-1 && *itMag<=sum.at<float>(pos.y+1, pos.x+1)) flag = false;
        }
        else
        {
            if(pos.x>0 && *itMag<=sum.at<float>(pos.y, pos.x-1)) flag = false;
            if(pos.x<workImg.cols-1 && *itMag<=sum.at<float>(pos.y, pos.x+1)) flag = false;
        }

        if(flag)
        {
            *itRet = 255;
        }
    }

    bool imageChanged = true;
    int i=0;
    while(imageChanged)
    {
        imageChanged = false;
        i++;

        itMag = sum.begin<float>();
        itDirection = direction.begin<float>();
        itRet = returnImg.begin<unsigned char>();
        itend = sum.end<float>();

        for(;itMag!=itend;++itMag, ++itDirection, ++itRet)
        {
            Point pos = itRet.pos();
            if(pos.x<2 || pos.x>cv_image.cols-2 || pos.y<2 || pos.y>cv_image.rows-2)
                continue;
            float currentDirection = *itDirection;
            if(*itRet==255)
            {
                *itRet=(unsigned char)64;
                if(currentDirection>112.5 && currentDirection <= 157.5)
                {
                    if(pos.y>0 && pos.x>0)
                    {
                        if(lowerThreshold<=sum.at<float>(pos.y-1, pos.x-1) &&
                        returnImg.at<unsigned char>(pos.y-1, pos.x-1)!=64 &&
                        direction.at<float>(pos.y-1, pos.x-1) > 112.5 &&
                        direction.at<float>(pos.y-1, pos.x-1) <= 157.5 &&
                        sum.at<float>(pos.y-1, pos.x-1) > sum.at<float>(pos.y-2, pos.x) &&
                        sum.at<float>(pos.y-1, pos.x-1) > sum.at<float>(pos.y, pos.x-2))
                        {
                            returnImg.ptr<unsigned char>(pos.y-1, pos.x-1)[0] = 255;
                            imageChanged = true;
                        }
                    }
                    if(pos.y<workImg.rows-1 && pos.x<workImg.cols-1)
                    {
                        if(lowerThreshold<=sum.at<float>(Point(pos.x+1, pos.y+1)) &&
                        returnImg.at<unsigned char>(pos.y+1, pos.x+1)!=64 &&
                        direction.at<float>(pos.y+1, pos.x+1) > 112.5 &&
                        direction.at<float>(pos.y+1, pos.x+1) <= 157.5 &&
                        sum.at<float>(pos.y-1, pos.x-1) > sum.at<float>(pos.y+2, pos.x) &&
                        sum.at<float>(pos.y-1, pos.x-1) > sum.at<float>(pos.y, pos.x+2))
                        {
                            returnImg.ptr<unsigned char>(pos.y+1, pos.x+1)[0] = 255;
                            imageChanged = true;
                        }
                    }
                }
                else if(currentDirection>67.5 && currentDirection <= 112.5)
                {
                    if(pos.x>0)
                    {
                        if(lowerThreshold<=sum.at<float>(Point(pos.x-1, pos.y)) &&
                        returnImg.at<unsigned char>(pos.y, pos.x-1)!=64 &&
                        direction.at<float>(pos.y, pos.x-1) > 67.5 &&
                        direction.at<float>(pos.y, pos.x-1) <= 112.5 &&
                        sum.at<float>(pos.y, pos.x-1) > sum.at<float>(pos.y-1, pos.x-1) &&
                        sum.at<float>(pos.y, pos.x-1) > sum.at<float>(pos.y+1, pos.x-1))
                        {
                            returnImg.ptr<unsigned char>(pos.y, pos.x-1)[0] = 255;
                            imageChanged = true;
                        }
                    }
                    if(pos.x<workImg.cols-1)
                    {
                        if(lowerThreshold<=sum.at<float>(Point(pos.x+1, pos.y)) &&
                        returnImg.at<unsigned char>(pos.y, pos.x+1)!=64 &&
                        direction.at<float>(pos.y, pos.x+1) > 67.5 &&
                        direction.at<float>(pos.y, pos.x+1) <= 112.5 &&
                        sum.at<float>(pos.y, pos.x+1) > sum.at<float>(pos.y-1, pos.x+1) &&
                        sum.at<float>(pos.y, pos.x+1) > sum.at<float>(pos.y+1, pos.x+1))
                        {
                            returnImg.ptr<unsigned char>(pos.y, pos.x+1)[0] = 255;
                            imageChanged = true;
                        }
                    }
                }
                else if(currentDirection > 22.5 && currentDirection <= 67.5)
                {
                    if(pos.y>0 && pos.x<workImg.cols-1)
                    {
                        if(lowerThreshold<=sum.at<float>(Point(pos.x+1, pos.y-1)) &&
                        returnImg.at<unsigned char>(pos.y-1, pos.x+1)!=64 &&
                        direction.at<float>(pos.y-1, pos.x+1) > 22.5 &&
                        direction.at<float>(pos.y-1, pos.x+1) <= 67.5 &&
                        sum.at<float>(pos.y-1, pos.x+1) > sum.at<float>(pos.y-2, pos.x) &&
                        sum.at<float>(pos.y-1, pos.x+1) > sum.at<float>(pos.y, pos.x+2))
                        {
                            returnImg.ptr<unsigned char>(pos.y-1, pos.x+1)[0] = 255;
                            imageChanged = true;
                        }
                    }
                    if(pos.y<workImg.rows-1 && pos.x>0)
                    {
                        if(lowerThreshold<=sum.at<float>(Point(pos.x-1, pos.y+1)) &&
                        returnImg.at<unsigned char>(pos.y+1, pos.x-1)!=64 &&
                        direction.at<float>(pos.y+1, pos.x-1) > 22.5 &&
                        direction.at<float>(pos.y+1, pos.x-1) <= 67.5 &&
                        sum.at<float>(pos.y+1, pos.x-1) > sum.at<float>(pos.y, pos.x-2) &&
                        sum.at<float>(pos.y+1, pos.x-1) > sum.at<float>(pos.y+2, pos.x))
                        {
                            returnImg.ptr<unsigned char>(pos.y+1, pos.x-1)[0] = 255;
                            imageChanged = true;
                        }
                    }
                }
                else
                {
                    if(pos.y>0)
                    {
                        if(lowerThreshold<=sum.at<float>(Point(pos.x, pos.y-1)) &&
                        returnImg.at<unsigned char>(pos.y-1, pos.x)!=64 &&
                        (direction.at<float>(pos.y-1, pos.x) < 22.5 ||
                        direction.at<float>(pos.y-1, pos.x) >=157.5) &&
                        sum.at<float>(pos.y-1, pos.x) > sum.at<float>(pos.y-1, pos.x-1) &&
                        sum.at<float>(pos.y-1, pos.x) > sum.at<float>(pos.y-1, pos.x+2))
                        {
                            returnImg.ptr<unsigned char>(pos.y-1, pos.x)[0] = 255;
                            imageChanged = true;
                        }
                    }
                    if(pos.y<workImg.rows-1)
                    {
                        if(lowerThreshold<=sum.at<float>(Point(pos.x, pos.y+1)) &&
                        returnImg.at<unsigned char>(pos.y+1, pos.x)!=64 &&
                        (direction.at<float>(pos.y+1, pos.x) < 22.5 ||
                        direction.at<float>(pos.y+1, pos.x) >=157.5) &&
                        sum.at<float>(pos.y+1, pos.x) > sum.at<float>(pos.y+1, pos.x-1) &&
                        sum.at<float>(pos.y+1, pos.x) > sum.at<float>(pos.y+1, pos.x+1))
                        {
                            returnImg.ptr<unsigned char>(pos.y+1, pos.x)[0] = 255;
                            imageChanged = true;
                        }
                    }
                }
            }
        }
    }
    MatIterator_<unsigned char>current = returnImg.begin<unsigned char>();    MatIterator_<unsigned char>final = returnImg.end<unsigned char>();
    for(;current!=final;++current)
    {
        if(*current==64)
            *current = 255;
    }

    //returnImg << CannyFiltered.
    //Mat to QImage
    cout << returnImg.rows << " " << returnImg.cols << "\n";
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            QColor color(image.pixel(x,y));
            int _b=returnImg.at<unsigned char>(y,x);
            int _g=returnImg.at<unsigned char>(y,x);
            int _r=returnImg.at<unsigned char>(y,x);
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
