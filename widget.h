#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QVector>
#include<QString>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

struct Node{
    float x;
    float y;
};

struct Device{
    Node lh;
    float width;
    float height;
    QString name;
    QString type;
};

struct Segment{
    QString in_out="";
    QString name="";
    Node n1;
    Node n2;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public:
    float min_mapx = 0;
    float min_mapy = 0;
    float map_w = 10;
    float map_h = 10;
    QVector<Device> deviceVec;//保存元件
    QVector<QVector<Segment>> streamVec;//保存线
    QVector<Segment> inoutVec;//输入输出线
private:
    Ui::Widget *ui;
    void inv(QPainter* painter,Device* device);
    void nand2(QPainter* painter,Device* device);
    void DFF(QPainter* painter,Device* device);
    void Latch(QPainter* painter,Device* device);
    void ck_gen(QPainter* painter,Device* device);
public:
    void paintEvent(QPaintEvent *);
    float max_4(float a,float b,float c,float d);
    float min_4(float a,float b,float c,float d);
    bool Cross(Segment s1,Segment s2);
};
#endif // WIDGET_H
