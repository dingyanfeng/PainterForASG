#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QtSvg/QSvgGenerator>
#include <QtSvg/QSvgRenderer>
#include <QKeyEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QVariant>
#include <QMessageBox>
#include <QComboBox>

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
    , have_new_file(false)
    , retryCount(0)
{
    setFocusPolicy(Qt::StrongFocus); // 设置焦点策略
    grabKeyboard(); // 捕获键盘事件
    mainWindow = parent->parentWidget();
    ui->setupUi(this);
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

void Widget::paintEvent(QPaintEvent * event)
{
    if(have_new_file){
        idx = netParam.size()-1;

        // 初始化数据结构
        QFile file(resultFilePath);//与文件建立联系
        if(!file.exists())//判断是否建立成功
        {
             qDebug()<<"file not exist!"<<endl;
        }
        else
        {
            this->setStyleSheet("background-color:rgb(255,255,255)");
            this->showMaximized();//成功则窗口会最大化，这只是我用检测的方法
        }
        QTextStream in(&file);
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))//打开文件，以只读的方式打开文本文件
        {
            while(true){
                Net cur_net;
                // 重置缩放和偏移因子
                m_scaleFactor = 1.0;
                m_dragOffset = QPoint();
                have_new_file=false;

                bool readDevice=false,readSegment=false,readInOut=false,aNewList=false;
                bool readNetName=false;
                while (!in.atEnd()) {
                    QString line=in.readLine();
                    if(line=="device:"){
                        readDevice=true;
                        readSegment=false;
                        readInOut=false;
                        readNetName=false;
                        continue;
                    }
                    if(line=="segments:"){
                        QVector<Segment> temp;
                        cur_net.streamVec.push_back(temp);
                        readSegment=true;
                        readDevice=false;
                        readInOut=false;
                        readNetName=false;
                        continue;
                    }
                    if(line=="inout:"){
                        readInOut=true;
                        readDevice=false;
                        readSegment=false;
                        readNetName=false;
                        continue;
                    }
                    if(line=="NAME:"){
                        readInOut=false;
                        readDevice=false;
                        readSegment=false;
                        readNetName=true;
                        continue;
                    }
                    if(line=="--NETLIST---"){
                        qDebug()<<cur_net.name<<endl;
                        idx=netParam.size();
                        // 判断当前网表是否已经在netParam中
                        bool already_in = false;
                        for (int i=0; i<netParam.size(); i++) {
                            if(netParam[i].name == cur_net.name) {
                                idx=i;
                                already_in = true;
                                break;
                            }
                        }
                        if(!already_in){
                            netParam.push_back(cur_net);
                            QComboBox* comboBox = mainWindow->findChild<QComboBox*>();
                            if(comboBox)
                            {
                                comboBox->addItem(netParam[idx].name);
                            }
                        }
                        aNewList=true;
                        break;
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
                            cur_net.deviceVec.push_back(temp);
                            readDevice=false;
                            //判断是否更改截图大小和坐标
                            if((lh.x + temp.width)*20 > cur_net.map_w){
                                cur_net.map_w = (lh.x + temp.width + 1)*20;
                            }
                            if((lh.y + temp.height)*20 > cur_net.map_h){
                                cur_net.map_h = (lh.y + temp.height + 1)*20;
                            }
                            if(lh.x*20 <= cur_net.min_mapx){
                                cur_net.min_mapx = (lh.x-1)*20;
                            }
                            if(lh.y*20 <= cur_net.min_mapy){
                                cur_net.min_mapy = (lh.y-1)*20;
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
                            cur_net.deviceVec.push_back(temp);
                            readDevice=false;
                            //判断是否更改截图大小
                            if((lh.x + temp.width)*20 > cur_net.map_w){
                                cur_net.map_w = (lh.x + temp.width + 1)*20;
                            }
                            if((lh.y + temp.height)*20 > cur_net.map_h){
                                cur_net.map_h = (lh.y + temp.height + 1)*20;
                            }
                            if(lh.x*20 <= cur_net.min_mapx){
                                cur_net.min_mapx = (lh.x-1)*20;
                            }
                            if(lh.y*20 <= cur_net.min_mapy){
                                cur_net.min_mapy = (lh.y-1)*20;
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
                        cur_net.streamVec[cur_net.streamVec.size()-1].push_back(tempSegment);
                        //判断是否更改截图大小
                        if(start.x*20 >= cur_net.map_w){
                            cur_net.map_w = (start.x + 1)*20;
                        }
                        if(end.x*20 >= cur_net.map_w){
                           cur_net. map_w = (end.x + 1)*20;
                        }
                        if(start.y*20 >= cur_net.map_h){
                            cur_net.map_h = (start.y + 1)*20;
                        }
                        if(end.y*20 >= cur_net.map_h){
                            cur_net.map_h = (end.y + 1)*20;
                        }
                        if(start.x*20 <= cur_net.min_mapx){
                            cur_net.min_mapx = (start.x - 1)*20;
                        }
                        if(end.x*20 <= cur_net.min_mapx){
                            cur_net.min_mapx = (end.x - 1)*20;
                        }
                        if(start.y*20 <= cur_net.min_mapy){
                            cur_net.min_mapy = (start.y - 1)*20;
                        }
                        if(end.y*20 <= cur_net.min_mapy){
                            cur_net.min_mapy = (end.y - 1)*20;
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
                            cur_net.inoutVec.push_back(tempSInOut);
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
                            cur_net.inoutVec.push_back(tempSInOut);
                        }

                        //判断是否更改截图大小
                        if(StInOut.x*20+(tempSInOut.name.size()-1)*9 >= cur_net.map_w){
                            cur_net.map_w = (StInOut.x + 1)*20+(tempSInOut.name.size()-1)*9;
                        }
                        if(EnInOut.x*20+(tempSInOut.name.size()-1)*9 >= cur_net.map_w){
                            cur_net.map_w = (EnInOut.x + 1)*20+(tempSInOut.name.size()-1)*9;
                        }
                        if(StInOut.y*20 >= cur_net.map_h){
                            cur_net.map_h = (StInOut.y + 1)*20;
                        }
                        if(EnInOut.y*20 >= cur_net.map_h){
                            cur_net.map_h = (EnInOut.y + 1)*20;
                        }
                        if(StInOut.x*20-(tempSInOut.name.size()-1)*9 <= cur_net.min_mapx){
                            cur_net.min_mapx = (StInOut.x - 1)*20-(tempSInOut.name.size()-1)*9;
                        }
                        if(EnInOut.x*20-(tempSInOut.name.size()-1)*9 <= cur_net.min_mapx){
                            cur_net.min_mapx = (EnInOut.x - 1)*20-(tempSInOut.name.size()-1)*9;
                        }
                        if(StInOut.y*20 <= cur_net.min_mapy){
                            cur_net.min_mapy = (StInOut.y - 1)*20;
                        }
                        if(EnInOut.y*20 <= cur_net.min_mapy){
                            cur_net.min_mapy = (EnInOut.y - 1)*20;
                        }
                    }
                    if(readNetName){
                        cur_net.name=line;
                    }
                }
                if(aNewList) continue;
                file.close();

                filePath = "";
                retryCount = 0;
                QComboBox* comboBox = mainWindow->findChild<QComboBox*>();
                if(comboBox)
                {
                    comboBox->setCurrentIndex(idx);
                }
                break;
            }
        }
        else{
             qDebug()<<file.errorString();
        }
    }

    if(idx >= 0){
        QPainter painter(this);
        painter.scale(m_scaleFactor, m_scaleFactor);
        painter.translate(m_dragOffset);
        //绘制元件
        painter.setBrush(Qt::gray);
        for(int i=0;i<netParam[idx].deviceVec.size();i++){
            QPen pen(QColor(138,76,76));//13,238,255
            pen.setWidth(2);
            painter.setPen(pen);
            if(netParam[idx].deviceVec[i].type=="inv"){
                this->inv(&painter,&netParam[idx].deviceVec[i]);
            }
            else if (netParam[idx].deviceVec[i].type=="nand2") {
                this->nand2(&painter,&netParam[idx].deviceVec[i]);
            }
            else if (netParam[idx].deviceVec[i].type=="DFF") {
                this->DFF(&painter,&netParam[idx].deviceVec[i]);
            }
            else if (netParam[idx].deviceVec[i].type=="Latch"){
                this->Latch(&painter,&netParam[idx].deviceVec[i]);
            }
            else if (netParam[idx].deviceVec[i].type=="ck_gen") {
                this->ck_gen(&painter,&netParam[idx].deviceVec[i]);
            }
            else {
                painter.drawRect(netParam[idx].deviceVec[i].lh.x*20,
                                 netParam[idx].deviceVec[i].lh.y*20,
                                 netParam[idx].deviceVec[i].width*20,
                                 netParam[idx].deviceVec[i].height*20);
            }
            QFont font("宋体",10);
            //设置字母大小写
            //font.setCapitalization(QFont::SmallCaps);
            painter.setFont(font);
            painter.drawText((netParam[idx].deviceVec[i].lh.x-1)*20,
                             netParam[idx].deviceVec[i].lh.y*20+netParam[idx].deviceVec[i].height*20,
                             (netParam[idx].deviceVec[i].width+2)*20,17,Qt::AlignCenter,
                             netParam[idx].deviceVec[i].type,nullptr);
            painter.drawText((netParam[idx].deviceVec[i].lh.x-1)*20,
                             netParam[idx].deviceVec[i].lh.y*20-17,
                             (netParam[idx].deviceVec[i].width+2)*20,17,Qt::AlignCenter,
                             netParam[idx].deviceVec[i].name,nullptr);
            //painter.drawPixmap(deviceVec[i].lh.x*20,deviceVec[i].lh.y*20,deviceVec[i].width*20,deviceVec[i].height*20,QPixmap(":/new/prefix1/NAVD.png"));
        }
        //绘制Segment
        for(int i=0;i<netParam[idx].streamVec.size();i++){
            QVector<Segment> portraitStream;//纵向Segment
            QVector<Segment> temp_portraitStream;//纵向未连接Segment
            QVector<QPair<int,int>> pSp;//纵向线端点连线数--决定是否需要绘制point
            QVector<Segment> transverseStream;//横向Segment
            for(int j=0;j<netParam[idx].streamVec[i].size();j++){
                QPen pen(QColor(31,0,214));//255,255,13
                pen.setWidth(2);
                painter.setPen(pen);
                painter.drawLine(QPoint(netParam[idx].streamVec[i][j].n1.x*20,
                                        netParam[idx].streamVec[i][j].n1.y*20),
                                 QPoint(netParam[idx].streamVec[i][j].n2.x*20,
                                        netParam[idx].streamVec[i][j].n2.y*20));
                if(netParam[idx].streamVec[i][j].n1.x==netParam[idx].streamVec[i][j].n2.x){
                    temp_portraitStream.push_back(netParam[idx].streamVec[i][j]);
                    //QPair<int,int> temp(1,1);
                    //pSp.push_back(temp);
                }else {
                    transverseStream.push_back(netParam[idx].streamVec[i][j]);
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
        for(int i=0;i<netParam[idx].inoutVec.size();i++){
            QPen pen(QColor(138,76,76));//255,255,13
            pen.setWidth(2);
            painter.setPen(pen);
            //painter.drawRect(inoutVec[i].n1.x*20,inoutVec[i].n1.y*20,20*20,20*20);
            if(netParam[idx].inoutVec[i].name!="" && netParam[idx].inoutVec[i].in_out!=""){
                if(netParam[idx].inoutVec[i].in_out=="in"){
                    int minEnd_x=20 * (netParam[idx].inoutVec[i].n1.x<netParam[idx].inoutVec[i].n2.x
                                       ? netParam[idx].inoutVec[i].n1.x
                                       : netParam[idx].inoutVec[i].n2.x);
                    painter.drawText(minEnd_x-(netParam[idx].inoutVec[i].name.size()-1)*9,
                                     netParam[idx].inoutVec[i].n1.y*20-17,
                                     netParam[idx].inoutVec[i].name.size()*9,17,Qt::AlignCenter,
                                     netParam[idx].inoutVec[i].name,nullptr);
                    qDebug()<<netParam[idx].inoutVec[i].name<<endl;
                }
                if(netParam[idx].inoutVec[i].in_out=="out"){
                    int maxEnd_x=20 * (netParam[idx].inoutVec[i].n1.x>netParam[idx].inoutVec[i].n2.x
                                       ? netParam[idx].inoutVec[i].n1.x
                                       : netParam[idx].inoutVec[i].n2.x);
                    painter.drawText(maxEnd_x-9,netParam[idx].inoutVec[i].n1.y*20-17,
                                     netParam[idx].inoutVec[i].name.size()*9,17,Qt::AlignCenter,
                                     netParam[idx].inoutVec[i].name,nullptr);
                }
            }
            painter.drawLine(QPoint(netParam[idx].inoutVec[i].n1.x*20,netParam[idx].inoutVec[i].n1.y*20),
                             QPoint(netParam[idx].inoutVec[i].n2.x*20,netParam[idx].inoutVec[i].n2.y*20));
        }

        QSize windowSize = size();
        qreal ratio1 = qreal(this->netParam[idx].map_w)/windowSize.width();
        qreal ratio2 = qreal(this->netParam[idx].map_h)/windowSize.height();
        qreal ratio = qMax(ratio1, ratio2);
        qreal scaleX = qreal(this->netParam[idx].map_w)/ratio;
        qreal scaleY = qreal(this->netParam[idx].map_h)/ratio;
        painter.scale(scaleX, scaleY);
    }


    QWidget::paintEvent(event);

}

void Widget::wheelEvent(QWheelEvent *event)
{
    if(m_ctrlPressed){
        const QPoint delta = event->angleDelta();
        const int steps = delta.y() / 120;

        // 根据滚轮方向调整缩放因子
        constexpr qreal scaleFactorStep = 0.01;
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
    if (event->modifiers() == Qt::ControlModifier)
    {
        m_ctrlPressed = true;
    }

    QWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->modifiers() != Qt::ControlModifier)
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
    else if(event->button() == Qt::RightButton)
    {
        QPoint transformedPos = event->pos();
        transformedPos /= m_scaleFactor;
        transformedPos -= m_dragOffset;
        for(int i=0; i<netParam[idx].deviceVec.size(); i++){
            if(netParam[idx].deviceVec[i].lh.x*20 < transformedPos.x() &&
               netParam[idx].deviceVec[i].lh.y*20 < transformedPos.y() &&
               netParam[idx].deviceVec[i].lh.x*20+netParam[idx].deviceVec[i].width*20 >transformedPos.x() &&
               netParam[idx].deviceVec[i].lh.y*20+netParam[idx].deviceVec[i].height*20 >transformedPos.y())
            {
                int temp = idx;
                // 遍历所有网表找是否有当前元件对应的
                for(int j=0; j<netParam.size(); j++){
                    if(netParam[j].name == netParam[idx].deviceVec[i].type){
                        idx = j;
                        break;
                    }
                }
                // 若没找到对应元件的网表，则不刷新
                if(temp==idx){
                    QMessageBox::critical(this, tr("ERROR"), tr("未找到对应网表！"));
                    break;
                }
                else{
                    qDebug()<<"Right-click device:"<<netParam[temp].deviceVec[i].name<<", turn to net"<<netParam[idx].name<<"(idx ="<<idx<<")"<<endl;
                    // 重置缩放和偏移因子
                    m_scaleFactor = 1.0;
                    m_dragOffset = QPoint();
                    // 重置网表名称
                    QComboBox* comboBox = mainWindow->findChild<QComboBox*>();
                    if(comboBox)
                    {
                        comboBox->setCurrentIndex(idx);
                    }
                    update();
                    break;
                }
            }
        }
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

void Widget::uploadFile()
{
    if (filePath.isEmpty()) {
        // 打开文件对话框，选择要上传的文件
        filePath = QFileDialog::getOpenFileName(this, "Select spi!");
    }

    // 检查文件路径是否有效
    if (!filePath.isEmpty())
    {
        qDebug() << "Select the file: " << filePath <<endl;
        QNetworkRequest request(QUrl("http://localhost:5000/upload"));

        // 打开文件
        if (fileObj == nullptr) {
            fileObj = new QFile(filePath);
        } else {
            // 关闭现有的文件对象
            fileObj->close();
            fileObj = new QFile(filePath);
            qDebug() << "Reassign the file-object with the new  file-path." << endl;
        }
        if (fileObj->open(QIODevice::ReadOnly))
        {
            // 创建网络访问管理器
            QNetworkAccessManager* manager = new QNetworkAccessManager(this);
            // 构建多部分请求
            QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
            // 创建文件字段
            QHttpPart filePart;
            QString fileName = QFileInfo(fileObj->fileName()).fileName();
            QString headerValue = "form-data; name=\"file\"; filename=\"%1\"";
            headerValue = headerValue.arg(fileName);
            filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                               QVariant(headerValue)); // 指定字段名称和文件名
            filePart.setBodyDevice(fileObj);
            multiPart->append(filePart);

            // 发送POST请求上传文件
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              QVariant("multipart/form-data; boundary=" + multiPart->boundary()));

            QNetworkReply* reply = manager->post(request, multiPart);


            // 连接请求完成信号到槽函数
            connect(reply, &QNetworkReply::finished, this, [=]() {
                // 检查请求是否成功
                if (reply->error() == QNetworkReply::NoError)
                {
                    // 生成保存文件的路径
                    QString saveFilePath = QCoreApplication::applicationDirPath() + "/result.txt";
                    // 保存返回的结果到本地文件
                    QFile file(saveFilePath);
                    if (file.open(QIODevice::WriteOnly))
                    {
                        file.write(reply->readAll());
                        file.close();

                        // 保存文件成功后，更新成员变量和重绘
                        resultFilePath = saveFilePath;
                        qDebug() << "Saving the result in :" << resultFilePath << endl;
                        have_new_file = true;
                        update();
                    }
                    else
                    {
                        qDebug() << "Fail to save the result!";
                    }
                }
                else
                {
                    // 请求发生错误，处理错误信息
                    qDebug() << "Fail to send the request Or Reply message error!" << reply->errorString();
                    QByteArray errorData = reply->readAll();
                    QString errorMessage(errorData);
                    qDebug() << "Error Message:" << errorMessage;
                    // 判断是否达到最大重传次数
                    if (retryCount < maxRetryCount)
                    {
                        // 重传文件
                        qDebug() << "Retrying upload...";
                        reply->deleteLater();
                        multiPart->deleteLater();
                        retryCount++;
                        qDebug() << retryCount << endl;
                        uploadFile();
                        return;
                    }
                    else
                    {
                        qDebug() << "Maximum retry count reached. Upload failed.";
                    }
                }

                // 释放reply对象
                reply->deleteLater();
                multiPart->deleteLater();
            });
        }
        else
        {
            // 文件打开失败，处理错误信息
            qDebug() << "Fail to open the file!";
        }
    }
}

Widget::~Widget()
{
    delete ui;
}

