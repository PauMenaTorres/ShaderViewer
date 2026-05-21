#include "WidgetScene.h"
#include "ui_WidgetScene.h"
#include <QFileDialog>

WidgetScene::WidgetScene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetScene)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(2); // Set default active tab to Water Props (index 2)
}

WidgetScene::~WidgetScene()
{
    delete ui;
}


