#ifndef BETAFACEPOINTS_H
#define BETAFACEPOINTS_H

#include <iostream>
#include <map>
#include <QXmlStreamReader>
#include <QFile>
#include <QString>
#include <QPointF>

using namespace std;

class BetafacePoints
{
    string name;
    QString getXMLPath();
    map <double,QPointF> facialPoints;
public:
    BetafacePoints(string name);
    map <double,QPointF> generatePoints();
    map<string,string> getExtraData();
};

#endif // BETAFACEPOINTS_H
