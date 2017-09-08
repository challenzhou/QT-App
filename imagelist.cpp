#include "imagelist.h"
#include <glob.h>
#include <QString>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>
#include <fstream>
#include <QDir>
#include <QStandardPaths>

ImageList::ImageList()
{

}

string ImageList::getPath(){
    //QString path  = QDir::homePath();
    //string projectPath = path.toStdString() + "/Desktop/XMLGenerator/input";
    //QString myPath = QString(RESOURCES_PATH);
    QString myPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/catoonize/generated";
    string projectPath = myPath.toStdString();
    return projectPath;
}

vector<string> ImageList::getNameList()
{
    string path = getPath();
    int start_position_to_erase = path.find_last_of("/");
    path.erase(start_position_to_erase, path.length()-start_position_to_erase);
    path = path + "/namesList.txt";
    if (std::ifstream(path)){
        QFile file(QString::fromStdString(path));
        if(!file.open(QIODevice::ReadOnly)) {
            qDebug()<<"Cannot read file";
        }
        QTextStream in(&file);
        while(!in.atEnd()) {
            QString line = in.readLine();
            namesList.push_back(line.toStdString());
        }

        file.close();
    }else{
        glob_t glob_result;
        path = getPath();
        string filePath = path+"/*";
        string names;
        glob(filePath.c_str(),GLOB_TILDE,NULL,&glob_result);
        for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
            names = glob_result.gl_pathv[i];
            names = names.substr(path.length()+1);
            namesList.push_back(names);
        }
    }
    return namesList;
}

//creates a function that stores the names left in a file
void ImageList::createFileWithNames(vector<string> namesLeft){
    string path = getPath();
    int start_position_to_erase = path.find_last_of("/");
    path.erase(start_position_to_erase, path.length()-start_position_to_erase);
    path = path + "/namesList.txt";
    QFile file(QString::fromStdString(path));
    if(file.open(QIODevice::ReadWrite|QIODevice::Truncate))
    {
        QTextStream stream( &file );
        for ( auto &name : namesLeft ) {
            stream << QString::fromStdString(name) << endl;
        }
    }
}
