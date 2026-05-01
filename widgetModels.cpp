#include "widgetModels.h"
#include "ui_widgetModels.h"
#include <QFileDialog>

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

void WidgetModels::on_btnLoadModel_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Model", "", "OBJ Files (*.obj)");
    if (!fileName.isEmpty())
    {
        ui->openGLWidget->loadModel(fileName);
    }
}
