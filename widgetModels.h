#ifndef WIDGETMODELS_H
#define WIDGETMODELS_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetModels; }
QT_END_NAMESPACE

class WidgetModels: public QWidget
{
    Q_OBJECT

public:
    WidgetModels(QWidget *parent = nullptr);
    ~WidgetModels();

private slots:
    void on_btnLoadModel_clicked();
private:
    Ui::WidgetModels *ui;
};
#endif // WIDGETMODELS_H
