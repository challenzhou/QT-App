#ifndef DLIBXML_H
#define DLIBXML_H

#include "imagelist.h"
#include <QDebug>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

//need path, name of image, box i.e top,left,width,height
//need to transform the coordinates

using namespace cv;
class DlibXML
{
    string name;
    Rect rect;
    map<double,QPointF>oldPoints;
    double antiScalingFactorX,antiScalingFactorY;
    QString doubleToStr();
    map<double,QPointF> getNewPoints();
    void createXMLFile(vector<string> fileNames,string path);
public:
    DlibXML();
    DlibXML(string name, map<double,QPointF> points, Rect rectangle,double scalingFactorX, double scalingFactorY);
    void generateXML();
    void createSingleXMLFile();
};

#endif // DLIBXML_H
