#include "serverwidget.h"
#include "ui_serverwidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>

ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWidget)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer(this);
    // default can't click button until connect successful.
    ui->pushButtonSend->setEnabled(false);
    ui->pushButtonSelect->setEnabled(false);
    // listen
    tcpServer->listen(QHostAddress::Any, 8888);
    connect(tcpServer,&QTcpServer::newConnection,[=](){
       tcpSocket = tcpServer->nextPendingConnection();
       QString ip = tcpSocket->peerAddress().toString();
       quint16 port = tcpSocket->peerPort();
       QString str = QString("[%1:%2]connect successful!").arg(ip).arg(port);
       ui->textEditRead->setText(str);
       // set button enable click
       ui->pushButtonSelect->setEnabled(true);
    });

    connect(&timer,&QTimer::timeout,[=](){
        timer.stop();
        sendData();
    });
}

ServerWidget::~ServerWidget()
{
    delete ui;
}


void ServerWidget::on_pushButtonSelect_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"open","../");
    if(filePath.isEmpty() == false){
        fileName.clear();
        fileSize = 0;
        sendSize = 0;
        // get file info
        QFileInfo info(filePath);
        fileName = info.fileName();
        fileSize = info.size();
        // read file
        file.setFileName(filePath);
        bool isOk = file.open(QIODevice::ReadOnly);
        if(isOk == false){
            qDebug() << "File open failed!";
        }
        ui->textEditWrite->setText(filePath);
        ui->pushButtonSend->setEnabled(true);
        ui->pushButtonSelect->setEnabled(false);
    }else{
       qDebug() << "File path failed!";
    }
}

void ServerWidget::on_pushButtonSend_clicked()
{
    // send file head info
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    qint64 len = tcpSocket->write(head.toUtf8());
    if(len > 0){
        timer.start(2000);
    }else{
        qDebug() << "File head info send failed!";
        file.close();
        ui->pushButtonSelect->setEnabled(true);
        ui->pushButtonSend->setEnabled(false);
    }
}

void ServerWidget::sendData(){
    qint64 len = 0;
    do {
        char buf[4*1024] = {0};
        // len = 0;
        len = file.read(buf,sizeof(buf));
//        qDebug() << buf << sizeof(buf);
        tcpSocket->write(buf,len);
        sendSize += len;
        qDebug() << "senSize:" << sendSize << len;
    } while (len>0);
    if(fileSize==sendSize){
        ui->textEditRead->append(QString("File %1 send successful!").arg(fileName));
        file.close();
        tcpSocket->disconnectFromHost();
        tcpServer->close();
    }
}
