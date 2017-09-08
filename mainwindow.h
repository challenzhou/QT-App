#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>
#include <QPen>
#include <QPainter>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace Ui {
class MainWindow;
}
using namespace std;
using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QWidget *myWidget;
    void initializeMyWidget();
private:
    Ui::MainWindow *ui;
    QPainter painter;
    int mFirstX;
    int mFirstY;
    bool mFirstClick;
    bool mpaintflag;
    QList<QPoint> m_points;
    QLabel *mLabel;
    QVBoxLayout *mLayout;
    double cropLeft,cropTop;
    string name;
    vector<string> namesList;
    map<double,QPointF>facialPoints;
    double scalingFactorX,scalingFactorY;//should change for different images
    map<double,QPointF>newPoints;//should change for different images
    int removePoint = 1; //flag for removal insertion of points
    double type;//should change for different images
    Rect rect;
    QImage getBoundingBoxImage(Mat image,map<double,QPointF> facialPoints);
    Mat QImage2Mat(QImage const& image2);
    QImage Mat2QImage(Mat const& image1);
    QImage displayImage(string name);
    Mat getCurrentImage(string name);
    void getCorrectPoints();
    void modifyNewPoints(QPointF point);
    void closeEvent (QCloseEvent *event);

private slots:
    void mousePressEvent(QMouseEvent * e);
    void paintEvent(QPaintEvent * e);
    void on_saveNext_clicked();
    void on_saveQuit_clicked();
    void on_generateXML_clicked();
};
#endif // MAINWINDOW_H
