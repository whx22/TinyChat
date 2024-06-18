#include "widget.h"
#include "ui_widget.h"

#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>

Widget::Widget(QWidget *parent, QString name)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    _name = name;
    port = 9999;
    udp_socket = new QUdpSocket(this);
    udp_socket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    connect(udp_socket, &QUdpSocket::readyRead, this, &Widget::receive_message);
    connect(ui->button_send, &QPushButton::clicked, [=](){
        send_message(Message);
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *) {
    emit this->closeWidget();
}

QString Widget::get_name() {
    return this->_name;
}

QString Widget::get_message() {
    QString message = ui->textEdit->toHtml();
    ui->textEdit->clear();
    ui->textEdit->setFocus();
    return message;
}

void Widget::send_message(MessageType type) {
    QByteArray byte_array;
    QDataStream stream(&byte_array, QIODevice::WriteOnly);

    stream << type << this->get_name();
    switch (type) {
    case Message:
        if (ui->textEdit->toPlainText() == "") {
            QMessageBox::warning(this, "警告", "信息为空！");
            return;
        }
        stream << this->get_message();
        break;
    case UserEnter:
        break;
    case UserLeft:
        break;
    }
    udp_socket->writeDatagram(byte_array.data(), byte_array.size(), QHostAddress::Broadcast, this->port);
}

void Widget::receive_message() {
    qint64 size = udp_socket->pendingDatagramSize();
    int int_size = static_cast<int>(size);
    QByteArray *byte_array = new QByteArray(int_size, 0);
    udp_socket->readDatagram((*byte_array).data(), size);
    QDataStream stream(byte_array, QIODevice::ReadOnly);
    int message_type;
    QString name, message;
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    stream >> message_type;
    switch (message_type) {
    case Message:
        stream >> name >> message;
        ui->text_browser->setTextColor(QColor(Qt::blue));
        ui->text_browser->append("["+name+"]"+time);
        ui->text_browser->append(message);
        break;
    case UserEnter:
        break;
    case UserLeft:
        break;
    }
}
