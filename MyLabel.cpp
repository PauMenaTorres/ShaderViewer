#include "MyLabel.h"

MyLabel::MyLabel(QWidget* parent):QLabel(parent)
{

}

MyLabel::~MyLabel()
{

}
void MyLabel::toUpperCase(QString s)
{
    QString t = s.toUpper();
    setText("Text: " + t);
}
