#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagelist.h"
#include "betafacepoints.h"
#include "dlibxml.h"
#include <iostream>
#include <QPixmap>
#include <glob.h>
#include <cmath>
#include <QDir>
#include <QCloseEvent>

using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setFixedSize(700,700);
    ui->setupUi(this);
    ui->centralWidget->layout()->setMargin(0);
    ui->centralWidget->layout()->setSpacing(0);
    ui->label_image->setAlignment(Qt::AlignTop);
    ImageList imagelist;
    namesList = imagelist.getNameList();
    name = namesList.back();
    int size = namesList.size();
    namesList.pop_back();
    int size1 = namesList.size();
}

void MainWindow :: mousePressEvent(QMouseEvent *e)
{
    mFirstX=0;
    mFirstY=0;
    mFirstClick=true;
    mpaintflag=false;

    if(e->button() == Qt::LeftButton)
            {
                if(1)
                {
                    QPointF point;
                    point.rx() = e->x();
                    point.ry() = e->y();
                    modifyNewPoints(point);
                    update();
                }

            }
}

void MainWindow :: paintEvent(QPaintEvent * e)
{
    QMainWindow::paintEvent(e);
        if(1)
        {
           QImage uiImage = displayImage(name);
           QPainter painter(&uiImage);
           QPen paintpen(Qt::red);
           paintpen.setWidth(5);
           painter.setPen(paintpen);
           getCorrectPoints();
           map<double,QPointF> :: iterator itr;
           for(itr=newPoints.begin();itr!=newPoints.end();++itr){
               QPointF point = itr->second;
               painter.drawPoint(point);
           }
           painter.end();
           ui->label_image->setPixmap(QPixmap::fromImage(uiImage));
         }
}

void MainWindow::getCorrectPoints()
{
    map<double,QPointF>::iterator itr;
    //for first time showing points
    if(newPoints.empty()){
        for(itr=facialPoints.begin();itr!=facialPoints.end();++itr){
            double type = itr->first;
            QPointF point = itr->second;
            double x = point.rx();
            double y = point.ry();
            x = (x-cropLeft)*scalingFactorX;
            y = (y-cropTop)*scalingFactorY;
            point.setX(x);
            point.setY(y);
            newPoints.insert(pair<double,QPointF>(type,point));
        }
    }
}

void MainWindow::modifyNewPoints(QPointF point)
{
    map<double,QPointF>::iterator itr;
    int flag=0;
    if(removePoint==1){
        for(itr=newPoints.begin();itr!=newPoints.end();++itr){
            QPointF oldPoint = itr->second;
            oldPoint = oldPoint-point;
            if(abs(oldPoint.rx())<=2 && abs(oldPoint.ry())<=2 && abs(oldPoint.rx())>=0 && abs(oldPoint.ry())>=0){
                type = itr->first;
                flag=1;
            }
        }
        if(flag==1){
            //qDebug()<<"Type : "<<type;
            removePoint=0;
            newPoints.erase(type);
        }
    }else{
        newPoints.insert(pair<double,QPointF>(type,point));
        removePoint=1;
    }
}

QImage MainWindow::displayImage(string name)
{
    Mat srcImage = getCurrentImage(name);
    BetafacePoints betafacePoints(name);
    facialPoints = betafacePoints.generatePoints();
    QImage scaledImage;
    if(facialPoints.size()==0){
        qDebug()<<"Files not found for "<<QString::fromStdString(name);
        on_saveQuit_clicked();
    }else{
        QImage newImage = getBoundingBoxImage(srcImage,facialPoints);
        int w = this->width();
        int h = 600;
        scaledImage = newImage.scaled(w,h,Qt::KeepAspectRatio);
        scalingFactorX = (double)scaledImage.width()/newImage.width();
        scalingFactorY = (double)scaledImage.height()/newImage.height();
    }
    return scaledImage;
}

QImage MainWindow::getBoundingBoxImage(Mat tempImage,map<double,QPointF> facialPoints)
{
    map<double,QPointF> :: iterator itr;
    double topLeftX=99999, topLeftY=99999;
    double width=0, height=0;
    QPointF point;
    for(itr=facialPoints.begin();itr!=facialPoints.end();++itr){
        point = itr->second;
        double pointX = point.rx();
        double pointY = point.ry();
        if(topLeftX > pointX){
            topLeftX = pointX;
        }
        if(topLeftY > pointY){
            topLeftY = pointY;
        }
        if(width<pointX){
            width = pointX;
        }
        if(height<pointY){
            height=pointY;
        }
    }
    if(topLeftX<0){
        topLeftX = 0;
    }
    if(topLeftY<0){
        topLeftY = 0;
    }
    if(topLeftX-10>0){
        topLeftX = topLeftX-10;
    }
    if(topLeftY-20>0){
        topLeftY = topLeftY-20;
    }
    width = width-topLeftX;
    height = height-topLeftY;
    if(width+20<tempImage.size().width){
        width = width+20;
    }
    if(height+20<tempImage.size().height){
        height = height+20;
    }
    Rect rectangle(topLeftX,topLeftY,width,height);
    rect = rectangle;
    cropLeft = topLeftX;
    cropTop = topLeftY;
    Mat subImage(tempImage,rect);
    QImage resultImage = Mat2QImage(subImage);
    return resultImage;
}

Mat MainWindow::getCurrentImage(string name){
    ImageList imageList;
    string path = imageList.getPath();
    path = path + "/" + name + "/shape/input.jpg";
    Mat srcImage = cv::imread(path);
    return srcImage;
}

Mat MainWindow::QImage2Mat(QImage const& src)
{
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}

QImage MainWindow::Mat2QImage(Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeMyWidget()
{
    myWidget = new QWidget();
    myWidget->show();
}

void MainWindow::on_saveNext_clicked()
{
    DlibXML dlibXML(name,newPoints,rect,scalingFactorX,scalingFactorY);
    if(namesList.size()==0){
        dlibXML.generateXML();
        this->close();
    }else{
        dlibXML.createSingleXMLFile();
        name = namesList.back();
        namesList.pop_back();
        newPoints.clear();
    }
}

void MainWindow::on_saveQuit_clicked()
{
    DlibXML dlibXML(name,newPoints,rect,scalingFactorX,scalingFactorY);
    dlibXML.createSingleXMLFile();
    ImageList imageList;
    imageList.createFileWithNames(namesList);
    this->close();
}

void MainWindow::on_generateXML_clicked()
{
    DlibXML dlibXML(name,newPoints,rect,scalingFactorX,scalingFactorY);
    dlibXML.generateXML();
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    on_saveQuit_clicked();
}
