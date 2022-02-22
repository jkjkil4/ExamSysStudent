#include "quesinvalid.h"

#include <QLabel>
#include <QHBoxLayout>

#include <QXmlStreamWriter>

QuesInvalid::QuesInvalid(const QString &quesName, QWidget *parent)
    : Ques(parent), mQuesName(quesName),
      mLabelText(new QLabel("题目 \"" + quesName + "\" 无效\n请更新考生端版本")),
      mLayout(new QHBoxLayout)
{
    mLayout->addWidget(mLabelText);
    mFrame->setLayout(mLayout);
}

void QuesInvalid::writeXmlStuAns(QXmlStreamWriter &xml) const {
    xml.writeStartElement(mQuesName);
    xml.writeEndElement();
}
