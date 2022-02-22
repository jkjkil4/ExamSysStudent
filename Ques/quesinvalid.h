#pragma once

#include "ques.h"

class QLabel;

class QuesInvalid : public Ques
{
    Q_OBJECT
public:
    explicit QuesInvalid(const QString &quesName, QWidget *parent = nullptr);

    void writeXmlStuAns(QXmlStreamWriter &xml) const override;

private:
    QString mQuesName;
    QLabel *mLabelText;
    QHBoxLayout *mLayout;
};
