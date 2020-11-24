#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    isStart = true;

    connect(tcpSocket,&QTcpSocket::readyRead,[=](){
        QByteArray buf = tcpSocket->readAll();
        qDebug() << buf;
        if(isStart){
            isStart = false;
            fileName = QString(buf).section("##",0,0);
            fileSize = QString(buf).section("##",1,1).toInt();
            receivedSize = 0;
            file.setFileName(fileName);

            bool isOk = file.open(QIODevice::WriteOnly);
            qDebug() << fileName << fileSize << receivedSize << isOk;
            if(isOk == false){
                qDebug() << "File write failed!";
            }
        }else{
            qint64 len = file.write(buf);
            receivedSize += len;
            qDebug() << receivedSize;
            if(receivedSize == fileSize){
                file.close();
                QMessageBox::information(this,"Successful","File receive successful!");
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
            }
        }
    });



}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_pushButtonConnect_clicked()
{
    QString ip = ui->lineEditIP->text();
    quint16 port = ui->lineEditPort->text().toInt();
    tcpSocket->connectToHost(QHostAddress(ip),port);
}
