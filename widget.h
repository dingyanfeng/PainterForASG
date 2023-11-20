#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QPushButton>
#include <QFile>
#include <QLineEdit>
#include <QDebug>
#include <QInputDialog>
#include <QLabel>
#include <QGridLayout>

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

struct Net{
public:
    QString name="";
    float min_mapx = 0;
    float min_mapy = 0;
    float map_w = 10;
    float map_h = 10;
    QVector<Device> deviceVec;//保存元件
    QVector<QVector<Segment>> streamVec;//保存线
    QVector<Segment> inoutVec;//输入输出线
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QVector<Net> netParam; // 保存所有网表
    int idx = -1;

private:
    Ui::Widget *ui;
    void inv(QPainter* painter,Device* device);
    void nand2(QPainter* painter,Device* device);
    void DFF(QPainter* painter,Device* device);
    void Latch(QPainter* painter,Device* device);
    void ck_gen(QPainter* painter,Device* device);
    void nand3(QPainter* painter,Device* device);
    void nor2(QPainter* painter,Device* device);
    void mos(QPainter* painter,Device* device);
public:
    float max_4(float a,float b,float c,float d);
    float min_4(float a,float b,float c,float d);
    bool Cross(Segment s1,Segment s2);
protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    qreal m_scaleFactor;  // 缩放因子
    bool m_ctrlPressed;   // Ctrl键状态
    bool m_isDragging;     // 拖动状态
    QPoint m_dragStartPosition;  // 拖动起始位置
    QPoint m_dragOffset;   // 拖动偏移量
private:
    QPushButton *uploadButton;

    QString resultFilePath;
public:
    bool have_new_file; // 重新绘图信号
public:
    void uploadFile();
private:
    QString filePath;  // 文件路径
    QFile *fileObj = nullptr;    // 文件对象
    int retryCount;    // 重传次数
    const int maxRetryCount = 6;  // 最大重传次数
private:
    QWidget* mainWindow;

public slots:
    void refreshPaintEvent(int index)
    {
        idx = index;
        qDebug()<<"Choose the "<<index<<"th netlist..."<<endl;
        // 重置缩放和偏移因子
        m_scaleFactor = 1.0;
        m_dragOffset = QPoint();
        update(); // 重新绘制widget
    }
// 用于服务器IP地址的获取
public:
    QString serverIP;
    QString serverPort;
public slots:
    void onUploadButtonClicked()
    {
        releaseKeyboard();
        QDialog dialog(this);
        dialog.setWindowTitle("Input");

        QLineEdit* ipPart1 = new QLineEdit(&dialog);
        QLineEdit* ipPart2 = new QLineEdit(&dialog);
        QLineEdit* ipPart3 = new QLineEdit(&dialog);
        QLineEdit* ipPart4 = new QLineEdit(&dialog);
        QLineEdit* portPart = new QLineEdit(&dialog);
        portPart->setPlaceholderText("port");

        // 设置文本框的最大长度为3
        ipPart1->setFixedWidth(50);
        ipPart2->setFixedWidth(50);
        ipPart3->setFixedWidth(50);
        ipPart4->setFixedWidth(50);
        portPart->setFixedWidth(50);

        QGridLayout* layout = new QGridLayout(&dialog);
        layout->addWidget(new QLabel("Server [IP:port]:", &dialog), 0, 0);
        layout->addWidget(ipPart1, 0, 1);
        layout->addWidget(new QLabel(".", &dialog), 0, 2);
        layout->addWidget(ipPart2, 0, 3);
        layout->addWidget(new QLabel(".", &dialog), 0, 4);
        layout->addWidget(ipPart3, 0, 5);
        layout->addWidget(new QLabel(".", &dialog), 0, 6);
        layout->addWidget(ipPart4, 0, 7);
        layout->addWidget(new QLabel(":", &dialog), 0, 8);
        layout->addWidget(portPart, 0, 9);

        QPushButton* okButton = new QPushButton("OK", &dialog);
        layout->addWidget(okButton, 1, 7);

        connect(okButton, &QPushButton::clicked, [&dialog, ipPart1, ipPart2, ipPart3, ipPart4, portPart, this]() {
            QString part1 = ipPart1->text();
            QString part2 = ipPart2->text();
            QString part3 = ipPart3->text();
            QString part4 = ipPart4->text();
            QString port_part = portPart->text();

            serverIP = part1 + "." + part2 + "." + part3 + "." + part4;
            qDebug() << "Server IP is: " << serverIP;
            serverPort = port_part;
            qDebug() << "Server Port is: "<<serverPort;

            dialog.accept();
        });

        // 设置焦点传递
        ipPart1->setFocusPolicy(Qt::StrongFocus);
        ipPart2->setFocusPolicy(Qt::StrongFocus);
        ipPart3->setFocusPolicy(Qt::StrongFocus);
        ipPart4->setFocusPolicy(Qt::StrongFocus);
        portPart->setFocusPolicy(Qt::StrongFocus);

        dialog.exec();
        grabKeyboard();
    }
};
#endif // WIDGET_H
