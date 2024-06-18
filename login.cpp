#include "login.h"
#include "ui_login.h"
#include "widget.h"

#include <QIcon>
#include <QToolButton>
#include <QMessageBox>

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/send.png"));
    this->setWindowTitle("Tiny Chat");
    QList<QString> name_list;
    name_list << "毛泽东" << "刘少奇" << "李先念" << "杨尚昆" << "邓小平" << "江泽民" \
              << "胡锦涛" << "习近平" << "周恩来" << "李强" << "王胡鑫";
    QStringList icon_name_list;
    icon_name_list << "car" << "computer" << "dog" << "duck" << "gray_cat" << "rabbit" \
                   << "spider" << "sun" << "water" << "whale" << "white_cat";
    QVector<QToolButton *> vector;
    for (int i = 0; i < 11; ++i) {
        QToolButton *button = new QToolButton(this);
        QPixmap originalPixmap(QString(":/images/%1.png").arg(icon_name_list[i]));
        QPixmap scaledPixmap = originalPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        button->setIcon(scaledPixmap);
        button->setIconSize(scaledPixmap.size());
        button->setText(QString("%1").arg(name_list[i]));
        button->setAutoRaise(true);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->user_layout->addWidget(button);
        vector.push_back(button);
        is_show.push_back(false);
    }
    for (int i = 0; i < 11; ++i) {
        connect(vector[i], &QToolButton::clicked, [=](){
            if (is_show[i]) {
                QMessageBox::warning(this, "警告", "用户已经登陆！");
                return;
            }
            is_show[i] = true;
            Widget *widget = new Widget(nullptr, vector[i]->text());
            widget->setWindowIcon(vector[i]->icon());
            widget->setWindowTitle(vector[i]->text());
            widget->show();

            connect(widget, &Widget::closeWidget, this, [=](){
                is_show[i] = false;
            });
        });
    }
}

login::~login()
{
    delete ui;
}
