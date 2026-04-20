#include "widgetSpheres.h"
#include "ui_widgetSpheres.h"

WidgetSpheres::WidgetSpheres(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetSpheres)
{
    ui->setupUi(this);

    connect(ui->sliderCenterX, &QSlider::valueChanged, [this](int val){
        ui->spinCenterX->setValue(val / 10.0);
    });
    connect(ui->spinCenterX, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val){
        ui->sliderCenterX->setValue(static_cast<int>(val * 10.0));
        ui->openGLWidget->setCurrentCenterX(val);
    });

    connect(ui->sliderCenterY, &QSlider::valueChanged, [this](int val){
        ui->spinCenterY->setValue(val / 10.0);
    });
    connect(ui->spinCenterY, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val){
        ui->sliderCenterY->setValue(static_cast<int>(val * 10.0));
        ui->openGLWidget->setCurrentCenterY(val);
    });

    connect(ui->sliderCenterZ, &QSlider::valueChanged, [this](int val){
        ui->spinCenterZ->setValue(val / 10.0);
    });
    connect(ui->spinCenterZ, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val){
        ui->sliderCenterZ->setValue(static_cast<int>(val * 10.0));
        ui->openGLWidget->setCurrentCenterZ(val);
    });
}

WidgetSpheres::~WidgetSpheres()
{
    delete ui;
}



