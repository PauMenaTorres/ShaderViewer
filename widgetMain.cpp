#include "widgetMain.h"
#include "ui_widgetMain.h"
#include "widgetSpheres.h"
#include "widgetModels.h"
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
    WidgetSpheres *ws = new WidgetSpheres();
    ws->setAttribute(Qt::WA_DeleteOnClose);
    ws->show();
}

void Widget::on_btnModels_clicked()
{
    WidgetModels *wm = new WidgetModels();
    wm->setAttribute(Qt::WA_DeleteOnClose);
    wm->show();
}


void Widget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
