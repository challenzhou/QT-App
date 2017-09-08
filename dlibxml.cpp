#include "dlibxml.h"
#include "betafacepoints.h"
#include <QPointF>
#include <fstream>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QTextStream>
#include <string>
#include <glob.h>

//DlibXML right now having image name as the name of the person

DlibXML::DlibXML(string newName, map<double,QPointF> points,Rect rectangle,double scalingFactorX, double scalingFactorY)
{
    name = newName;
    oldPoints = points;
    rect = rectangle;
    antiScalingFactorX = scalingFactorX;
    antiScalingFactorY = scalingFactorY;
}

void DlibXML::createSingleXMLFile(){
    ImageList imagelist;
    string path = imagelist.getPath();
    path = path + "/" + name;
    string nameF = name;
    nameF = "/shape/dlibFaceData.xml";
    QString filename = QString::fromStdString(path) + QString::fromStdString(nameF);
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << "Failed to open file";
    }
    file.close();
    QDomDocument document;
    QDomElement image = document.createElement("image");
    string imageName = name + ".jpg";
    image.setAttribute("file",QString::fromStdString(imageName));
    BetafacePoints betafacePoints(name);
    map <string,string> extraData = betafacePoints.getExtraData();
    QDomElement widthElem = document.createElement("width");
    QDomElement heightElem = document.createElement("height");
    QDomElement x = document.createElement("x");
    QDomElement y = document.createElement("y");
    QDomElement angle = document.createElement("angle");
    QDomElement uid = document.createElement("uid");
    QDomText width_text = document.createTextNode(QString::fromStdString(extraData.at("width")));
    QDomText height_text = document.createTextNode(QString::fromStdString(extraData.at("height")));
    QDomText x_text = document.createTextNode(QString::fromStdString(extraData.at("x")));
    QDomText y_text = document.createTextNode(QString::fromStdString(extraData.at("y")));
    QDomText angle_text = document.createTextNode(QString::fromStdString(extraData.at("angle")));
    QDomText uid_text = document.createTextNode(QString::fromStdString(extraData.at("uid")));
    widthElem.appendChild(width_text);
    heightElem.appendChild(height_text);
    x.appendChild(x_text);
    y.appendChild(y_text);
    angle.appendChild(angle_text);
    uid.appendChild(uid_text);
    image.appendChild(widthElem);
    image.appendChild(heightElem);
    image.appendChild(x);
    image.appendChild(y);
    image.appendChild(angle);
    image.appendChild(uid);

    QDomElement box = document.createElement("box");
    QString top = QString::number(rect.x);
    QString left = QString::number(rect.y);
    QString width = QString::number(rect.width);
    QString height = QString::number(rect.height);
    box.setAttribute("top",top);
    box.setAttribute("left",left);
    box.setAttribute("width",width);
    box.setAttribute("height",height);
    map<double,QPointF> imagePoints = getNewPoints();
    map<double,QPointF> :: iterator itr;
    for(itr=imagePoints.begin();itr!=imagePoints.end();++itr){
        QDomElement part = document.createElement("part");
        string str = std::to_string(itr->first);
        str = str.substr(0,str.find(".", 0));
        QString name = QString::fromStdString(str);
        QPointF point = itr->second;
        QString x = QString::number(point.rx());
        QString y = QString::number(point.ry());
        part.setAttribute("name",name);
        part.setAttribute("x",x);
        part.setAttribute("y",y);
        box.appendChild(part);
    }
    image.appendChild(box);
    document.appendChild(image);

    QFile outFile(filename);
    if( !outFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        qDebug( "Failed to open file for writing." );
    }
    QTextStream stream( &outFile );
    stream << document.toString();

    outFile.close();
}

void DlibXML::generateXML(){
    glob_t glob_result;
    ImageList imagelist;
    string path = imagelist.getPath();
    //int start_position_to_erase = path.find_last_of("/");
    //path.erase(start_position_to_erase, path.length()-start_position_to_erase);
    string filePath = path+"/*";
    string names;
    vector<string> fileNamesList;
    glob(filePath.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        names = glob_result.gl_pathv[i];
        string dlibFilePath = names + "/shape/dlibFaceData.xml";
        if (std::ifstream(dlibFilePath)){
            fileNamesList.push_back(dlibFilePath);
        }
    }
    createXMLFile(fileNamesList,path);
}

void DlibXML::createXMLFile(vector<string> fileNames,string path){
    int start_position_to_erase = path.find_last_of("/");
    path.erase(start_position_to_erase, path.length()-start_position_to_erase);
    path = path + "/dlib.xml";
    QFile file(QString::fromStdString(path));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug<<"Error opening file";
    }
    QDomDocument document;
    QDomElement dataset = document.createElement("dataset");
    QDomElement images = document.createElement("images");
    file.close();
    for(int i=0;i<fileNames.size();i++){
        QFile singleFile(QString::fromStdString(fileNames.at(i)));
        if (!singleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug()<<"Failed to open single file";
        }
        QDomDocument singleDocument;
        if (!singleDocument.setContent(&singleFile))
        {
            qDebug() << "failed to parse file";
            singleFile.close();
        }
        images.appendChild(singleDocument.documentElement());
        singleFile.close();
    }
    dataset.appendChild(images);
    document.appendChild(dataset);
    QFile outFile(QString::fromStdString(path));
    if( !outFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        qDebug( "Failed to open file for writing." );
    }
    QTextStream stream( &outFile );
    stream << document.toString();

    outFile.close();
}

map<double,QPointF> DlibXML::getNewPoints(){
    map<double,QPointF> newPoints;
    double antiCropLeftX = rect.x;
    double antiCropLeftY = rect.y;
    map<double,QPointF> :: iterator itr;
    for(itr=oldPoints.begin();itr!=oldPoints.end();++itr){
        double type = itr->first;
        QPointF point = itr->second;
        double x = point.rx();
        double y = point.ry();
        x = (double)x/antiScalingFactorX + antiCropLeftX;
        y = (double)y/antiScalingFactorY + antiCropLeftY;
        point.setX(x);
        point.setY(y);
        newPoints.insert(pair<double,QPointF>(type,point));
    }
    return newPoints;
}
