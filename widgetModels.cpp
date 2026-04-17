#include "widgetModels.h"
#include "ui_widgetModels.h"

WidgetModels::WidgetModels(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetModels)
{
    ui->setupUi(this);
}

WidgetModels::~WidgetModels()
{
    delete ui;
}
