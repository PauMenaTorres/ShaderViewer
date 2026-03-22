#include "widget.h"
#include "ui_widget.h"

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


void Widget::on_pushButton_2_clicked()
{
    float px = ui->spinX->value();
    float py = ui->spinY->value();
    float pz = ui->spinZ->value();

    float radio = ui->spinRadius->value();

    float r = ui->sliderR->value() / 100.0f;
    float g = ui->sliderG->value() / 100.0f;
    float b = ui->sliderB->value() / 100.0f;

    ui->openGLWidget->addSphere(glm::vec3(px, py, pz), radio, glm::vec4(r, g, b, 1.0f));

}

