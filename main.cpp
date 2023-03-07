#include "widget.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QApplication>
#include<QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

//    QPixmap pm=w.grab(QRect(QPoint(w.min_mapx,w.min_mapy),QSize(qAbs(w.min_mapx)+w.map_w,qAbs(w.min_mapy)+w.map_h)));
//    pm.save("C:\\Users\\dingyanfeng\\Desktop\\pic.png");
    return a.exec();
}
