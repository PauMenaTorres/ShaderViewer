#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    MyLabel(QWidget* parent);
    ~MyLabel();

public slots:
    void toUpperCase(QString s);

};

#endif // MYLABEL_H
