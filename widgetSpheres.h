#ifndef WIDGETSPHERES_H
#define WIDGETSPHERES_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetSpheres; }
QT_END_NAMESPACE

class WidgetSpheres : public QWidget
{
    Q_OBJECT

public:
    WidgetSpheres(QWidget *parent = nullptr);
    ~WidgetSpheres();

private slots:
private:
    Ui::WidgetSpheres *ui;
};
#endif // WIDGETSPHERES_H
