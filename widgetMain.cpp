#include "widgetMain.h"
#include "ui_widgetMain.h"
#include "widgetSpheres.h"
#include "widgetModels.h"
#include "widgetScene.h"
#include <QMessageBox>
#include <QStyleOption>
#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnSpheres_clicked()
{
    WidgetSpheres *ws = new WidgetSpheres(this);
    ws->setWindowFlags(Qt::Window);
    ws->show();
}

void Widget::on_btnModels_clicked()
{
    WidgetModels *wm = new WidgetModels(this);
    wm->setWindowFlags(Qt::Window);
    wm->show();
}

void Widget::on_btnScene_clicked()
{
    WidgetScene *ws = new WidgetScene(this);
    ws->setWindowFlags(Qt::Window);
    ws->show();
}


void Widget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
