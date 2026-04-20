#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget: public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_btnSpheres_clicked();
    void on_btnModels_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGETMAIN_H
