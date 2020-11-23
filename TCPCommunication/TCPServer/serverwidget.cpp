#include "serverwidget.h"
#include "ui_serverwidget.h"

ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWidget)
{
    ui->setupUi(this);
    // 成员默认为空
    tcpServer = NULL;
    tcpSocket = NULL;
    // 监听套接字
    tcpServer = new QTcpServer(this);
    // 监听所有端口，默认端口8888
    tcpServer->listen(QHostAddress::Any, 8888);
    // 处理信号
    connect(tcpServer,&QTcpServer::newConnection,[=](){
        // 取出建立好连接的套接字并赋值给通信套接字
        tcpSocket = tcpServer->nextPendingConnection();
        // 获取对方IP和端口
        QString ip = tcpSocket->peerAddress().toString();
        qint16 port = tcpSocket->peerPort();
        QString temp = QString("[%1:%2]:成功连接!").arg(ip).arg(port);
        ui->textEditRead->setText(temp);

        // 处理信号（当有数据传入）
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            // 从套接字取出内容
            QByteArray array = tcpSocket->readAll();
            // 显示到屏幕
            ui->textEditRead->append(QString("Receive[%1:%2]:").arg(ip).arg(port)+array);
        });
    });
}

ServerWidget::~ServerWidget()
{
    delete ui;
}


/* 点击发送按钮之后的处理 */
void ServerWidget::on_pushButtonSend_clicked()
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
void ServerWidget::on_pushButtonClose_clicked()
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
