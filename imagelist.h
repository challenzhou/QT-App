#ifndef IMAGELIST_H
#define IMAGELIST_H

#include<string>
#include<vector>
#include<iostream>
using namespace std;
class ImageList
{
    vector<string> namesList;
public:
    ImageList();
    string getPath();
    vector<string> getNameList();
    void createFileWithNames(vector<string> namesLeft);
};

#endif // IMAGELIST_H
