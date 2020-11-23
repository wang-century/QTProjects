#include "clientwidget.h"
#include "ui_clientwidget.h"

#include <QHostAddress>

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    tcpSocket = NULL;
    ui->lineEditIP->setText("127.0.0.1");
    ui->lineEditPort->setText("8888");

    // 为套接字分配空间
    tcpSocket = new QTcpSocket(this);
    // 连接成功后打印提示文字
    connect(tcpSocket,&QTcpSocket::connected,[=](){
        ui->textEditRead->setText("与服务器连接成功");
    });
    // 处理信号（当有数据传入）
    connect(tcpSocket,&QTcpSocket::readyRead,[=](){
        // 获取对方IP和端口
        QHostAddress address = tcpSocket->peerAddress();
        QString ip = address.toString();
        qint16 port = tcpSocket->peerPort();
        QString temp = QString("[%1:%2]:").arg(ip).arg(port);
        // 从套接字取出内容
        QByteArray array = tcpSocket->readAll();
        // 显示到屏幕
        ui->textEditRead->append(temp+array);
    });
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

// 点击连接服务器按钮的处理
void ClientWidget::on_pushButtonConnect_clicked()
{
    // 获取服务器地址和端口
    QString ip = ui->lineEditIP->text();
    qint16 port = ui->lineEditPort->text().toInt();
    // 和服务器建立连接
    tcpSocket->connectToHost(ip,port);

}


/* 点击发送按钮之后的处理 */
void ClientWidget::on_pushButtonSend_clicked()
{
    // 判断是否连接
    if (tcpSocket == NULL){
        return;
    }
    // 获取编辑区内容
    QString str = ui->textEditWrite->toPlainText();
    // 发送编辑区数据,使用套接字
    tcpSocket->write(str.toUtf8().data());
}
/* 点击关闭按钮之后的处理 */
void ClientWidget::on_pushButtonClose_clicked()
{
    // 判断是否连接
    if (tcpSocket == NULL){
        return;
    }
    // 断开连接
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
    tcpSocket = NULL;
}

