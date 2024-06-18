#include "widget.h"
#include "ui_widget.h"

#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>
#include <QFont>
#include <QColorDialog>
#include <QFileDialog>

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
    connect(ui->font_combo_box, &QFontComboBox::currentFontChanged, [=](const QFont &font){
        ui->textEdit->setFontFamily(font.toString());
        ui->textEdit->setFocus();
    });

    void (QComboBox:: * size_button)(const QString &text) = &QComboBox::currentIndexChanged;
    connect(ui->combo_box_font_size, size_button, [=](const QString &text){
        ui->textEdit->setFontPointSize(text.toDouble());
        ui->textEdit->setFocus();
    });

    connect(ui->button_bold, &QToolButton::clicked, [=](bool checked){
        if (checked) {
            ui->textEdit->setFontWeight(QFont::Bold);
        } else {
            ui->textEdit->setFontWeight(QFont::Normal);
        }
    });
    connect(ui->button_italic, &QToolButton::clicked, [=](bool checked){
        ui->textEdit->setFontItalic(checked);
        ui->textEdit->setFocus();
    });
    connect(ui->button_under_line, &QToolButton::clicked, [=](bool checked){
        ui->textEdit->setFontUnderline(checked);
        ui->textEdit->setFocus();
    });
    connect(ui->button_clear, &QToolButton::clicked, [=](){
        ui->text_browser->clear();
    });
    connect(ui->button_color, &QToolButton::clicked, [=](){
        QColor color = QColorDialog::getColor(color, this);
        ui->textEdit->setTextColor(color);
    });
    connect(ui->button_save, &QToolButton::clicked, [=](){
        if (ui->text_browser->toPlainText().isEmpty()) {
            QMessageBox::warning(this, "警告", "保存聊天记录为空！");
            return;
        }
        QString file_name = QFileDialog::getSaveFileName(this, "保存聊天记录", "聊天记录", "(*.txt)");
        if (!file_name.isEmpty()) {
            QFile file(file_name);
            file.open(QIODevice::WriteOnly | QFile::Text);
            QTextStream stream(&file);
            stream << ui->text_browser->toPlainText();
            file.close();
        }
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

void Widget::user_enter(QString user_name) {

}

void Widget::user_left(QString username, QString time) {

}
