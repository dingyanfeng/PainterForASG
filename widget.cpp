#include "widget.h"
#include "ui_widget.h"
#include<QPainter>
#include<QPixmap>
#include<QFile>
#include<QDebug>
#include<QTextStream>
#include <QtSvg/QSvgGenerator>
#include <QtSvg/QSvgRenderer>
#include <QKeyEvent>

float Widget::max_4(float a,float b,float c,float d){
    if(a>=b && a>=c && a>=d)
        return a;
    else if(b>=a && b>=c && b>=d)
        return b;
    else if(c>=a && c>=b && c>=d)
        return c;
    else
        return d;
}

float Widget::min_4(float a,float b,float c,float d){
    if(a<=b && a<=c && a<=d)
        return a;
    else if(b<=a && b<=c && b<=d)
        return b;
    else if(c<=a && c<=b && c<=d)
        return c;
    else
        return d;
}

bool Widget::Cross(Segment s1,Segment s2){
    bool b1=false;
    bool b2=false;
    if(s1.n1.x<s2.n1.x && s1.n2.x>s2.n2.x)
        b1=true;
    else if(s1.n2.x<s2.n1.x && s1.n1.x>s2.n1.x)
        b1=true;
    else
        b1=false;

    if(s2.n1.y<=s1.n1.y && s2.n2.y>=s1.n1.y)
        b2=true;
    else if(s2.n2.y<=s1.n1.y && s2.n1.y>=s1.n1.y)
        b2=true;
    else
        b2=false;

    return (b1&&b2);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_scaleFactor(1.0)
    , m_ctrlPressed(false)
    , m_isDragging(false)
{
    ui->setupUi(this);
    QFile file("C:/Users/dingyanfeng/Desktop/output.txt");//与文件建立联系
    if(!file.exists())//判断是否建立成功
    {
         QString str = "world";
         qDebug()<<"hello "<<str<<"!"<<endl;
    }
    else
    {
        this->setStyleSheet("background-color:rgb(255,255,255)");
        this->showMaximized();//成功则窗口会最大化，这只是我用检测的方法
    }

    if(file.open(QIODevice::ReadOnly|QIODevice::Text))//打开文件，以只读的方式打开文本文件
    {
        QTextStream in(&file);
        bool readDevice=false,readSegment=false,readInOut=false;
        while (!in.atEnd()) {
            QString line=in.readLine();
            if(line=="device:"){
                readDevice=true;
                readSegment=false;
                readInOut=false;
                continue;
            }
            if(line=="segments:"){
                QVector<Segment> temp;
                streamVec.push_back(temp);
                readSegment=true;
                readDevice=false;
                readInOut=false;
                continue;
            }
            if(line=="inout:"){
                readInOut=true;
                readDevice=false;
                readSegment=false;
                continue;
            }
            if(readDevice){
                QStringList list=line.split(" ");
                if(list.size()==4){
                    Device temp;
                    Node lh;
                    lh.x=list[1].toFloat();
                    lh.y=list[0].toFloat();
                    temp.name="NONAME";
                    temp.type="NOTYPE";
                    temp.lh=lh;
                    temp.width=list[3].toFloat();
                    temp.height=list[2].toFloat();
                    deviceVec.push_back(temp);
                    readDevice=false;
                    //判断是否更改截图大小和坐标
                    if((lh.x + temp.width)*20 > map_w){
                        map_w = (lh.x + temp.width + 1)*20;
                    }
                    if((lh.y + temp.height)*20 > map_h){
                        map_h = (lh.y + temp.height + 1)*20;
                    }
                    if(lh.x*20 <= min_mapx){
                        min_mapx = (lh.x-1)*20;
                    }
                    if(lh.y*20 <= min_mapy){
                        min_mapy = (lh.y-1)*20;
                    }
                    continue;
                }
                if(list.size()==6){
                    Device temp;
                    Node lh;
                    lh.x=list[3].toFloat();
                    lh.y=list[2].toFloat();
                    temp.name=list[0];
                    temp.type=list[1];
                    temp.lh=lh;
                    temp.width=list[5].toFloat();
                    temp.height=list[4].toFloat();
                    deviceVec.push_back(temp);
                    readDevice=false;
                    //判断是否更改截图大小
                    if((lh.x + temp.width)*20 > map_w){
                        map_w = (lh.x + temp.width + 1)*20;
                    }
                    if((lh.y + temp.height)*20 > map_h){
                        map_h = (lh.y + temp.height + 1)*20;
                    }
                    if(lh.x*20 <= min_mapx){
                        min_mapx = (lh.x-1)*20;
                    }
                    if(lh.y*20 <= min_mapy){
                        min_mapy = (lh.y-1)*20;
                    }
                    continue;
                }
            }
            if(readSegment){
                QStringList list=line.split(" ");
                Segment tempSegment;
                Node start;
                Node end;
                start.x=list[1].toFloat();
                start.y=list[0].toFloat();
                end.x=list[3].toFloat();
                end.y=list[2].toFloat();
                tempSegment.n1=start;
                tempSegment.n2=end;
                streamVec[streamVec.size()-1].push_back(tempSegment);
                //判断是否更改截图大小
                if(start.x*20 >= map_w){
                    map_w = (start.x + 1)*20;
                }
                if(end.x*20 >= map_w){
                    map_w = (end.x + 1)*20;
                }
                if(start.y*20 >= map_h){
                    map_h = (start.y + 1)*20;
                }
                if(end.y*20 >= map_h){
                    map_h = (end.y + 1)*20;
                }
                if(start.x*20 <= min_mapx){
                    min_mapx = (start.x - 1)*20;
                }
                if(end.x*20 <= min_mapx){
                    min_mapx = (end.x - 1)*20;
                }
                if(start.y*20 <= min_mapy){
                    min_mapy = (start.y - 1)*20;
                }
                if(end.y*20 <= min_mapy){
                    min_mapy = (end.y - 1)*20;
                }
            }
            if(readInOut){
                QStringList list=line.split(" ");
                Segment tempSInOut;
                Node StInOut;
                Node EnInOut;
                if(list.size()==4){
                    StInOut.x=list[1].toFloat();
                    StInOut.y=list[0].toFloat();
                    EnInOut.x=list[3].toFloat();
                    EnInOut.y=list[2].toFloat();
                    tempSInOut.n1=StInOut;
                    tempSInOut.n2=EnInOut;
                    inoutVec.push_back(tempSInOut);
                }
                if(list.size()==6){
                    StInOut.x=list[3].toFloat();
                    StInOut.y=list[2].toFloat();
                    EnInOut.x=list[5].toFloat();
                    EnInOut.y=list[4].toFloat();
                    tempSInOut.n1=StInOut;
                    tempSInOut.n2=EnInOut;
                    tempSInOut.in_out=list[0];
                    tempSInOut.name=list[1];
                    inoutVec.push_back(tempSInOut);
                }

                //判断是否更改截图大小
                if(StInOut.x*20+(tempSInOut.name.size()-1)*9 >= map_w){
                    map_w = (StInOut.x + 1)*20+(tempSInOut.name.size()-1)*9;
                }
                if(EnInOut.x*20+(tempSInOut.name.size()-1)*9 >= map_w){
                    map_w = (EnInOut.x + 1)*20+(tempSInOut.name.size()-1)*9;
                }
                if(StInOut.y*20 >= map_h){
                    map_h = (StInOut.y + 1)*20;
                }
                if(EnInOut.y*20 >= map_h){
                    map_h = (EnInOut.y + 1)*20;
                }
                if(StInOut.x*20-(tempSInOut.name.size()-1)*9 <= min_mapx){
                    min_mapx = (StInOut.x - 1)*20-(tempSInOut.name.size()-1)*9;
                }
                if(EnInOut.x*20-(tempSInOut.name.size()-1)*9 <= min_mapx){
                    min_mapx = (EnInOut.x - 1)*20-(tempSInOut.name.size()-1)*9;
                }
                if(StInOut.y*20 <= min_mapy){
                    min_mapy = (StInOut.y - 1)*20;
                }
                if(EnInOut.y*20 <= min_mapy){
                    min_mapy = (EnInOut.y - 1)*20;
                }
                qDebug()<<map_w<<" "<<map_h<<" "<<min_mapx<<" "<<min_mapy<<endl;
            }
        }
        file.close();
    }
    else{
         qDebug()<<file.errorString();
    }
}

void Widget::inv(QPainter* painter,Device* device){
    QPolygon triangle;
    int p1_x = device->lh.x*20;
    int p1_y = device->lh.y*20+5;
    int p2_x = device->lh.x*20;
    int p2_y = (device->lh.y+device->height)*20-5;
    int p3_x = (device->lh.x + device->width)*20-16;
    int p3_y = device->lh.y*20 + device->height*10;
    triangle.setPoints(3,p1_x,p1_y,p2_x,p2_y,p3_x,p3_y);//三点坐标
    painter->drawPolygon(triangle);
    painter->drawEllipse(QPoint(p3_x+8,p3_y),8,8);
    //painter->drawLine(p3_x-15,p3_y,p3_x,p3_y);
    //painter->drawLine(p1_x,p3_y,p1_x+10,p3_y);
}

void Widget::nand2(QPainter *painter, Device *device){
    int p1_x = device->lh.x*20;
    int p1_y = device->lh.y*20+15;
    int p2_x = device->lh.x*20;
    int p2_y = (device->lh.y+device->height)*20-15;
    int p3_x = (device->lh.x+device->width)*20-25;
    int p3_y = p1_y;
    int p4_x = p3_x;
    int p4_y = p2_y;
    painter->drawLine(p1_x,p1_y,p2_x,p2_y);
    painter->drawLine(p1_x,p1_y,p3_x,p3_y);
    painter->drawLine(p2_x,p2_y,p4_x,p4_y);
    QRectF rectangle(p3_x-18, p3_y, 30.0, 30.0);
    int startAngle = -80 * 16;
    int spanAngle = 170 * 16;
    painter->drawArc(rectangle, startAngle, spanAngle);    //调用绘图命令
    painter->drawEllipse(QPoint(p3_x+20,(p3_y+p4_y)/2),7,7);
}

void Widget::DFF(QPainter* painter,Device* device){
    int y1 = device->lh.y*20 + device->height*10;
    int y2 = device->lh.y*20 + device->height*5;
    painter->drawLine(device->lh.x*20, y1, (device->lh.x)*20+10, y1);
    painter->drawLine(device->lh.x*20, y2, (device->lh.x)*20+10, y2);
    painter->drawLine((device->lh.x+device->width)*20, y1, (device->lh.x+device->width)*20-10, y1);
    painter->drawLine((device->lh.x+device->width)*20, y2, (device->lh.x+device->width)*20-10, y2);
    painter->drawRect(device->lh.x*20,device->lh.y*20,device->width*20,device->height*20);
}

void Widget::Latch(QPainter *painter, Device *device){
    int y1 = device->lh.y*20 + device->height*10;
    int y2 = device->lh.y*20 + device->height*5;
    painter->drawLine(device->lh.x*20, y1, (device->lh.x)*20+10, y1);
    painter->drawLine(device->lh.x*20, y2, (device->lh.x)*20+10, y2);
    painter->drawLine((device->lh.x+device->width)*20, y1, (device->lh.x+device->width)*20-10, y1);
    painter->drawLine((device->lh.x+device->width)*20, y2, (device->lh.x+device->width)*20-10, y2);
    painter->drawRect(device->lh.x*20,device->lh.y*20,device->width*20,device->height*20);
}

void Widget::ck_gen(QPainter *painter, Device *device){
    int y1 = device->lh.y*20 + device->height/3*20;
    painter->drawLine(device->lh.x*20, y1, (device->lh.x)*20+10, y1);
    painter->drawLine((device->lh.x+device->width)*20, y1, (device->lh.x+device->width)*20-10, y1);
    painter->drawRect(device->lh.x*20,device->lh.y*20,device->width*20,device->height*20);
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.scale(m_scaleFactor, m_scaleFactor);
    painter.translate(m_dragOffset);
    //绘制元件
    painter.setBrush(Qt::gray);
    for(int i=0;i<deviceVec.size();i++){
        QPen pen(QColor(138,76,76));//13,238,255
        pen.setWidth(2);
        painter.setPen(pen);
        if(deviceVec[i].type=="inv"){
            this->inv(&painter,&deviceVec[i]);
        }
        else if (deviceVec[i].type=="nand2") {
            this->nand2(&painter,&deviceVec[i]);
        }
        else if (deviceVec[i].type=="DFF") {
            this->DFF(&painter,&deviceVec[i]);
        }
        else if (deviceVec[i].type=="Latch"){
            this->Latch(&painter,&deviceVec[i]);
        }
        else if (deviceVec[i].type=="ck_gen") {
            this->ck_gen(&painter,&deviceVec[i]);
        }
        else {
            painter.drawRect(deviceVec[i].lh.x*20,deviceVec[i].lh.y*20,deviceVec[i].width*20,deviceVec[i].height*20);
        }
        QFont font("宋体",10);
        //设置字母大小写
        //font.setCapitalization(QFont::SmallCaps);
        painter.setFont(font);
        painter.drawText((deviceVec[i].lh.x-1)*20,deviceVec[i].lh.y*20+deviceVec[i].height*20,(deviceVec[i].width+2)*20,17,Qt::AlignCenter,deviceVec[i].type,nullptr);
        painter.drawText((deviceVec[i].lh.x-1)*20,deviceVec[i].lh.y*20-17,(deviceVec[i].width+2)*20,17,Qt::AlignCenter,deviceVec[i].name,nullptr);
        //painter.drawPixmap(deviceVec[i].lh.x*20,deviceVec[i].lh.y*20,deviceVec[i].width*20,deviceVec[i].height*20,QPixmap(":/new/prefix1/NAVD.png"));
    }
    //绘制Segment
    for(int i=0;i<streamVec.size();i++){
        QVector<Segment> portraitStream;//纵向Segment
        QVector<Segment> temp_portraitStream;//纵向未连接Segment
        QVector<QPair<int,int>> pSp;//纵向线端点连线数--决定是否需要绘制point
        QVector<Segment> transverseStream;//横向Segment
        for(int j=0;j<streamVec[i].size();j++){
            QPen pen(QColor(31,0,214));//255,255,13
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(QPoint(streamVec[i][j].n1.x*20,streamVec[i][j].n1.y*20),QPoint(streamVec[i][j].n2.x*20,streamVec[i][j].n2.y*20));
            if(streamVec[i][j].n1.x==streamVec[i][j].n2.x){
                temp_portraitStream.push_back(streamVec[i][j]);
                //QPair<int,int> temp(1,1);
                //pSp.push_back(temp);
            }else {
                transverseStream.push_back(streamVec[i][j]);
            }
        }
        QVector<bool> pb_ornot;
        for(int j=0;j<temp_portraitStream.size();j++){
            pb_ornot.push_back(true);
        }
        for(int j=0;j<temp_portraitStream.size();j++){
            Segment resultEl;
            resultEl.n1.x=temp_portraitStream[j].n1.x;
            resultEl.n1.y=temp_portraitStream[j].n1.y;
            resultEl.n2.x=temp_portraitStream[j].n2.x;
            resultEl.n2.y=temp_portraitStream[j].n2.y;
            for(int k=j+1;k<temp_portraitStream.size();k++){
                if(temp_portraitStream[k].n1.x==temp_portraitStream[j].n1.x){
                    resultEl.n1.x=temp_portraitStream[k].n1.x;
                    resultEl.n1.y=max_4(temp_portraitStream[j].n1.y,temp_portraitStream[j].n2.y,temp_portraitStream[k].n1.y,temp_portraitStream[k].n2.y);
                    resultEl.n2.x=temp_portraitStream[k].n1.x;
                    resultEl.n2.y=min_4(temp_portraitStream[j].n1.y,temp_portraitStream[j].n2.y,temp_portraitStream[k].n1.y,temp_portraitStream[k].n2.y);
                    pb_ornot[k]=false;
                }
            }
            if(pb_ornot[j]){
                portraitStream.push_back(resultEl);
                QPair<int,int> temp(1,1);
                pSp.push_back(temp);
            }
        }
        for(int j=0;j<transverseStream.size();j++){
            int portrait1 = -1;
            int portrait2 = -1;
            for(int k=0;k<portraitStream.size();k++){
                if(transverseStream[j].n1.x==portraitStream[k].n1.x){
                    portrait1 = k;
                }
                if(transverseStream[j].n2.x==portraitStream[k].n1.x){
                    portrait2 = k;
                }
            }
            if(portrait1>-1){
                if((transverseStream[j].n1.y > portraitStream[portrait1].n1.y && transverseStream[j].n1.y < portraitStream[portrait1].n2.y) ||
                      (transverseStream[j].n1.y < portraitStream[portrait1].n1.y && transverseStream[j].n1.y > portraitStream[portrait1].n2.y)){
                    QPen pen(QColor(31,0,214));
                    pen.setWidth(5);
                    painter.setPen(pen);
                    painter.drawPoint(transverseStream[j].n1.x*20,transverseStream[j].n1.y*20);
                }
                if(transverseStream[j].n1.y == portraitStream[portrait1].n1.y){
                    pSp[portrait1].first++;
                }
                if(transverseStream[j].n1.y == portraitStream[portrait1].n2.y){
                    pSp[portrait1].second++;
                }
            }
            if(portrait2>-1){
                if((transverseStream[j].n2.y > portraitStream[portrait2].n1.y && transverseStream[j].n2.y < portraitStream[portrait2].n2.y) ||
                      (transverseStream[j].n2.y < portraitStream[portrait2].n1.y && transverseStream[j].n2.y > portraitStream[portrait2].n2.y)){
                    QPen pen(QColor(31,0,214));
                    pen.setWidth(5);
                    painter.setPen(pen);
                    painter.drawPoint(transverseStream[j].n2.x*20,transverseStream[j].n2.y*20);
                }
                if(transverseStream[j].n2.y == portraitStream[portrait2].n1.y){
                    pSp[portrait2].first++;
                }
                if(transverseStream[j].n2.y == portraitStream[portrait2].n2.y){
                    pSp[portrait2].second++;
                }
            }
        }
        for(int j=0;j<transverseStream.size();j++){
            for(int k=0;k<portraitStream.size();k++){
                if(Cross(transverseStream[j],portraitStream[k])){
                    QPen pen(QColor(31,0,214));
                    pen.setWidth(5);
                    painter.setPen(pen);
                    painter.drawPoint(portraitStream[k].n1.x*20,transverseStream[j].n1.y*20);
                }
            }
        }
        for(int j=0;j<pSp.size();j++){
            if(pSp[j].first>2){
                QPen pen(QColor(31,0,214));
                pen.setWidth(5);
                painter.setPen(pen);
                painter.drawPoint(portraitStream[j].n1.x*20,portraitStream[j].n1.y*20);
            }
            if(pSp[j].second>2){
                QPen pen(QColor(31,0,214));
                pen.setWidth(5);
                painter.setPen(pen);
                painter.drawPoint(portraitStream[j].n2.x*20,portraitStream[j].n2.y*20);
            }
        }
    }
    //绘制InOut
    for(int i=0;i<inoutVec.size();i++){
        QPen pen(QColor(138,76,76));//255,255,13
        pen.setWidth(2);
        painter.setPen(pen);
        //painter.drawRect(inoutVec[i].n1.x*20,inoutVec[i].n1.y*20,20*20,20*20);
        if(inoutVec[i].name!="" && inoutVec[i].in_out!=""){
            if(inoutVec[i].in_out=="in"){
                int minEnd_x=20 * (inoutVec[i].n1.x<inoutVec[i].n2.x ? inoutVec[i].n1.x : inoutVec[i].n2.x);
                painter.drawText(minEnd_x-(inoutVec[i].name.size()-1)*9,inoutVec[i].n1.y*20-17,inoutVec[i].name.size()*9,17,Qt::AlignCenter,inoutVec[i].name,nullptr);
                qDebug()<<inoutVec[i].name<<endl;
            }
            if(inoutVec[i].in_out=="out"){
                int maxEnd_x=20 * (inoutVec[i].n1.x>inoutVec[i].n2.x ? inoutVec[i].n1.x : inoutVec[i].n2.x);
                painter.drawText(maxEnd_x-9,inoutVec[i].n1.y*20-17,inoutVec[i].name.size()*9,17,Qt::AlignCenter,inoutVec[i].name,nullptr);
            }
        }
        painter.drawLine(QPoint(inoutVec[i].n1.x*20,inoutVec[i].n1.y*20),QPoint(inoutVec[i].n2.x*20,inoutVec[i].n2.y*20));
    }

    QSize windowSize = size();
    qreal ratio1 = qreal(this->map_w)/windowSize.width();
    qreal ratio2 = qreal(this->map_h)/windowSize.height();
    qreal ratio = qMax(ratio1, ratio2);
    qreal scaleX = qreal(this->map_w)/ratio;
    qreal scaleY = qreal(this->map_h)/ratio;
    qDebug() << "map:    " << this->map_w << " " << this->map_h << endl;
    qDebug() << "windows:" << windowSize.width() << " " << windowSize.height() << endl;
    qDebug() << "scale:  " << scaleX << " " << scaleY << endl;
    painter.scale(scaleX, scaleY);

    QWidget::paintEvent(event);

    // 更新窗口
    update();
}

void Widget::wheelEvent(QWheelEvent *event)
{
    if(m_ctrlPressed){
        const QPoint delta = event->angleDelta();
        const int steps = delta.y() / 120;

        // 根据滚轮方向调整缩放因子
        constexpr qreal scaleFactorStep = 0.1;
        m_scaleFactor += steps * scaleFactorStep;

        // 限制缩放因子的范围
        constexpr qreal minScaleFactor = 0.01;
        constexpr qreal maxScaleFactor = 100.0;
        m_scaleFactor = qBound(minScaleFactor, m_scaleFactor, maxScaleFactor);

        // 刷新窗口
        update();
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        m_ctrlPressed = true;
    }

    QWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        m_ctrlPressed = false;
    }

    QWidget::keyReleaseEvent(event);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isDragging = true;
        m_dragStartPosition = event->pos();
    }

    QWidget::mousePressEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging)
    {
        QPoint currentPos = event->pos();
        // 计算鼠标的实际移动距离
        QPoint delta = currentPos - m_dragStartPosition;

        // 根据缩放因子来调整移动距离
        QPoint scaledDelta(delta.x() / m_scaleFactor, delta.y() / m_scaleFactor);

        // 应用拖动偏移量
        m_dragOffset += scaledDelta;

        // 更新起始位置
        m_dragStartPosition = currentPos;

        update();
    }

    QWidget::mouseMoveEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDragging)
    {
        m_isDragging = false;
        m_dragOffset += event->pos() - m_dragStartPosition;
        m_dragStartPosition = QPoint(); // 重置起始位置
    }

    QWidget::mouseReleaseEvent(event);
}

Widget::~Widget()
{
    delete ui;
}

