#include "betafacepoints.h"
#include "imagelist.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QDebug>
#include <string>

using namespace std;

BetafacePoints::BetafacePoints(string newName)
{
    name = newName;
}

QString BetafacePoints::getXMLPath()
{
    ImageList imageList;
    string path = imageList.getPath();
    path = path + '/'+ name + "/shape/faceData.xml";
    QString filePath = QString::fromStdString(path);
    return filePath;
}

map <string,string> BetafacePoints::getExtraData()
{
    QDomDocument xmlDocument;
    map<string,string> extraData;
    QFile xmlFile(getXMLPath());
    if(!xmlFile.exists()){
        cout << "XML file not found";
        return extraData;
    }
    xmlDocument.setContent(&xmlFile);
    xmlFile.close();
    QDomElement root=xmlDocument.documentElement();
    QDomElement faces = root.firstChildElement("faces");
    QDomElement faceInfo = faces.firstChildElement("FaceInfo");
    QDomElement angle = faceInfo.firstChildElement("angle");
    QDomElement height = faceInfo.firstChildElement("height");
    QDomElement width = faceInfo.firstChildElement("width");
    QDomElement x = faceInfo.firstChildElement("x");
    QDomElement y = faceInfo.firstChildElement("y");
    QDomElement uid = faceInfo.firstChildElement("uid");
    extraData.insert(pair<string,string>("angle",angle.text().toStdString()));
    extraData.insert(pair<string,string>("height",height.text().toStdString()));
    extraData.insert(pair<string,string>("width",width.text().toStdString()));
    extraData.insert(pair<string,string>("x",x.text().toStdString()));
    extraData.insert(pair<string,string>("y",y.text().toStdString()));
    extraData.insert(pair<string,string>("uid",uid.text().toStdString()));
    return extraData;
}

map<double,QPointF> BetafacePoints::generatePoints()
{

    QFile xmlFile(getXMLPath());
    if(!xmlFile.exists()){
        cout << "XML file not found";
        return facialPoints;
    }
    QString errMsg;
    QFileDevice::FileError err = QFileDevice::NoError;
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        errMsg = xmlFile.errorString();
        err = xmlFile.error();
        cout << "Error Message" << errMsg.toStdString() <<" : "<< err;
        return facialPoints;
    }
    QXmlStreamReader Rxml;
    Rxml.setDevice(&xmlFile);
    while(!Rxml.atEnd()){
        if(Rxml.isStartElement()){
            if(Rxml.name()!="PointInfo"){
                Rxml.readNextStartElement();
            }else{
                QPointF point;
                double type;
                Rxml.readNext();
                while(Rxml.name()!="PointInfo"){
                    QString value = Rxml.readElementText();
                    QString node = Rxml.name().toString();
                    if(node=="x"){
                        double val = value.toDouble();
                        point.setX(val);
                        //qDebug() << "XXX" << Rxml.name() <<":"<<val;
                    }else if(node=="y"){
                        float val = value.toDouble();
                        point.setY(val);
                        //qDebug() << "YYY" << Rxml.name() <<":"<<val;
                    }else if(node=="type"){
                        type = value.toFloat();
                        //qDebug() << "Others" << Rxml.name() <<":"<<type;
                    }
                    Rxml.readNextStartElement();
                }
                facialPoints.insert(pair<double,QPointF>(type,point));
            }
        }else{
            Rxml.readNext();
        }

    }
    xmlFile.close();
    return facialPoints;
}

