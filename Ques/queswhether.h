#pragma once

#include "ques.h"
#include "Widget/doubleslidebutton.h"

class QVBoxLayout;

class QuesWhether : public Ques
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QuesWhether(QWidget *parent = nullptr);

    void readXml(const QDomElement &elem) override;
    void writeXmlStuAns(QXmlStreamWriter &xml) const override;
    void readXmlStuAns(const QDomElement &elem) override;

private:
    QLabel *mLabelQues;
    QLabel *mLabelBtnLeft, *mLabelBtnRight;
    DoubleSlideButton *mButton;
    QHBoxLayout *mLayoutButton;
    QVBoxLayout *mLayout;
};
