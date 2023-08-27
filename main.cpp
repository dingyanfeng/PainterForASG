#include "widget.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget window;
    window.setWindowTitle("Schematic Generation");
    QVBoxLayout* layout = new QVBoxLayout(&window);
    QHBoxLayout* centralLayout = new QHBoxLayout();
    QFrame* leftFrame = new QFrame(&window);
    leftFrame->setFrameShape(QFrame::Box);
    leftFrame->setLineWidth(1);
    leftFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    Widget* w = new Widget(leftFrame);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftFrame);
    leftLayout->addWidget(w);

    QVBoxLayout* rightLayout = new QVBoxLayout();

    QPushButton* uploadButton = new QPushButton("Upload file");
    QPushButton* button2 = new QPushButton("按钮2");
    QPushButton* button3 = new QPushButton("按钮3");

    rightLayout->addWidget(uploadButton);
    rightLayout->addWidget(button2);
    rightLayout->addWidget(button3);

    centralLayout->addWidget(leftFrame);
    centralLayout->addLayout(rightLayout);

    layout->addLayout(centralLayout);

    QObject::connect(uploadButton, &QPushButton::clicked, w, &Widget::uploadFile);

    window.show();

//    QPixmap pm=w.grab(QRect(QPoint(w.min_mapx,w.min_mapy),QSize(qAbs(w.min_mapx)+w.map_w,qAbs(w.min_mapy)+w.map_h)));
//    pm.save("C:\\Users\\dingyanfeng\\Desktop\\pic.png");
    return a.exec();
}
