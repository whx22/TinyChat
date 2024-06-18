#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    enum MessageType{Message, UserEnter, UserLeft};
    Widget(QWidget *parent, QString name);
    ~Widget();
    void closeEvent(QCloseEvent *);
    void send_message(MessageType type);
    QString get_name();
    QString get_message();
    void user_enter(QString user_name);
    void user_left(QString username, QString time);
    void receive_message();
signals:
    void closeWidget();

private:
    Ui::Widget *ui;
    QString _name;
    quint16 port;
    QUdpSocket *udp_socket;
};
#endif // WIDGET_H
