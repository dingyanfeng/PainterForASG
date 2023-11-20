#include "widget.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVariant>

// 自定义消息处理
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    // 根据消息类型选择输出样式
    QString message;
    switch (type) {
    case QtDebugMsg:
        message = QString("[Debug] %1").arg(msg);
        break;
    case QtWarningMsg:
        message = QString("[Warning] %1").arg(msg);
        break;
    case QtCriticalMsg:
        message = QString("[Critical] %1").arg(msg);
        break;
    case QtFatalMsg:
        message = QString("[Fatal] %1").arg(msg);
        break;
    default:
        break;
    }

    // 在控制台输出框中显示消息
    QPlainTextEdit *outputWidget = qobject_cast<QPlainTextEdit*>(qApp->property("outputWidget").value<QObject*>());
    if (outputWidget) {
        outputWidget->appendPlainText(message);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget window;
    window.setWindowTitle("Schematic Generation");
    window.showMaximized();
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

    QPushButton* uploadButton = new QPushButton("UPLOAD FILE");
    QPushButton* IPButton = new QPushButton("SET SERVER IP AND PORT FIRST!");
    IPButton->setStyleSheet("QPushButton { color: red; font-weight: bold; }");
    QComboBox* comboBox = new QComboBox();
    comboBox->setFixedWidth(400);
    QPlainTextEdit* consoleOutput = new QPlainTextEdit();
    consoleOutput->setReadOnly(true);



    // 创建标签并设置文本
    QLabel* label = new QLabel("NetList Name: ");
    label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    QFont font = label->font();
    font.setPointSize(10);
    label->setFont(font);

    // 创建水平布局，将标签和文本框放入其中
    QHBoxLayout* textBoxLayout = new QHBoxLayout();
    textBoxLayout->addWidget(label);
    textBoxLayout->addWidget(comboBox);

    rightLayout->addWidget(uploadButton);
    rightLayout->addWidget(IPButton);
    rightLayout->addLayout(textBoxLayout);
    rightLayout->addWidget(consoleOutput);

    centralLayout->addWidget(leftFrame);
    centralLayout->addLayout(rightLayout);

    layout->addLayout(centralLayout);
    QObject::connect(uploadButton, &QPushButton::clicked, w, &Widget::uploadFile);
    QObject::connect(IPButton, &QPushButton::clicked, w, &Widget::onUploadButtonClicked);
    QObject::connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), w, &Widget::refreshPaintEvent);

    // 设置右侧布局的伸缩因子
    centralLayout->setStretchFactor(leftFrame, 1);
    centralLayout->setStretchFactor(rightLayout, 0);
    // 将控制台输出框设为全局属性
    a.setProperty("outputWidget", QVariant::fromValue<QObject*>(consoleOutput));

    // 将默认消息处理器替换为自定义的消息处理器
    qInstallMessageHandler(customMessageHandler);

    window.show();

//    QPixmap pm=w.grab(QRect(QPoint(w.min_mapx,w.min_mapy),QSize(qAbs(w.min_mapx)+w.map_w,qAbs(w.min_mapy)+w.map_h)));
//    pm.save("C:\\Users\\dingyanfeng\\Desktop\\pic.png");
    return a.exec();
}
