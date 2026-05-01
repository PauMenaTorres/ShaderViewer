#ifndef WidgetScene_H
#define WidgetScene_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetScene; }
QT_END_NAMESPACE

class WidgetScene: public QWidget
{
    Q_OBJECT

public:
    WidgetScene(QWidget *parent = nullptr);
    ~WidgetScene();

private slots:
private:
    Ui::WidgetScene *ui;
};
#endif // WidgetScene_H
