#include "widgetSpheres.h"
#include "ui_widgetSpheres.h"

WidgetSpheres::WidgetSpheres(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetSpheres)
{
    ui->setupUi(this);
}

WidgetSpheres::~WidgetSpheres()
{
    delete ui;
}



