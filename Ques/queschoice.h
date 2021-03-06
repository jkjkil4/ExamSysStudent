#pragma once

#include "ques.h"

class QVBoxLayout;
class QAbstractButton;

class QuesChoice : public Ques
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QuesChoice(const QString &quesName, const QString &head, QWidget *parent = nullptr);

    void readXml(const QDomElement &elem) override;
    void writeXmlStuAns(QXmlStreamWriter &xml) const override;
    void readXmlStuAns(const QDomElement &elem) override;

    /**
     * @brief   通过数字序号得到对应的字母序号
     * @param   数字序号
     * @return  字母序号
     */
    QString numToLetter(int num);

    /**
     * @brief   用于让子类自定Btn类
     * @param   Btn文字
     * @return  自定的Btn类的对象
     */
    virtual QAbstractButton* createBtn(const QString &str);

protected:
    QLabel *mLabelQues;
    QVBoxLayout *mLayout, *mLayoutButtons;

    QString mHead, mQuesName;
};
