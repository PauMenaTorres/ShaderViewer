#include "WidgetScene.h"
#include "ui_WidgetScene.h"
#include <QFileDialog>

WidgetScene::WidgetScene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetScene)
{
    ui->setupUi(this);
}

WidgetScene::~WidgetScene()
{
    delete ui;
}


