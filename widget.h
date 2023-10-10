#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QPushButton>
#include <QFile>
#include <QLineEdit>
#include <QDebug>

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
};
#endif // WIDGET_H
