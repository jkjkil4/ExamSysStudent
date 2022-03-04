#include "queswhether.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDomElement>
#include <QXmlStreamWriter>

QuesWhether::QuesWhether(QWidget *parent)
    : Ques(parent),
      mLabelQues(new QLabel("（判断题）")),
      mLabelBtnLeft(new QLabel), mLabelBtnRight(new QLabel), mButton(new DoubleSlideButton),
      mLayoutButton(new QHBoxLayout), mLayout(new QVBoxLayout)
{
    mLayoutButton->addWidget(mLabelBtnLeft);
    mLayoutButton->addWidget(mButton);
    mLayoutButton->addWidget(mLabelBtnRight);
    mLayoutButton->addStretch();

    mLayout->addWidget(mLabelQues);
    mLayout->addSpacing(16);
    mLayout->addLayout(mLayoutButton);
    mFrame->setLayout(mLayout);
}

bool QuesWhether::isDone(QString *pStr) const {
    if(mButton->state() != DoubleSlideButton::Mid)
        return true;
    if(pStr)
        *pStr = "未选择选项";
    return false;
}

void QuesWhether::readXml(const QDomElement &elem) {
    mLabelBtnLeft->setText(elem.attribute("LTxt"));
    mLabelBtnRight->setText(elem.attribute("RTxt"));
    mLabelQues->setText(elem.text());
}
void QuesWhether::writeXmlStuAns(QXmlStreamWriter &xml) const {
    xml.writeTextElement("v", QString::number(mButton->state()));
}
void QuesWhether::readXmlStuAns(const QDomElement &elem) {
    mButton->setState((DoubleSlideButton::State)elem.text().toInt());
}
