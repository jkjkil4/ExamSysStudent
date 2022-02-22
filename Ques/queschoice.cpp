#include "queschoice.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QAbstractButton>

#include <QDomElement>
#include <QXmlStreamWriter>

QuesChoice::QuesChoice(const QString &quesName, const QString &head, QWidget *parent)
    : Ques(parent),
      mLabelQues(new QLabel(head)),
      mLayout(new QVBoxLayout), mLayoutButtons(new QVBoxLayout),
      mHead(head), mQuesName(quesName)
{
    mLabelQues->setWordWrap(true);

    mLayout->addWidget(mLabelQues);
    mLayout->addSpacing(16);
    mLayout->addLayout(mLayoutButtons);
    mFrame->setLayout(mLayout);
}

void QuesChoice::readXml(const QDomElement &elem) {
    // 清空原有内容
    int count = mLayoutButtons->count();
    for(int i = 0; i < count; ++i) {
        mLayoutButtons->itemAt(i)->widget()->deleteLater();
    }

    // 题目
    mLabelQues->setText(elem.attribute("Ques"));

    // 选项
    QDomNode node = elem.firstChild();
    int i = 0;
    while(!node.isNull()) {
        QDomElement elem = node.toElement();
        // 如果该节点名称为Ans，则读取文字并添加到选项中
        if(elem.tagName() == "Ans") {
            QAbstractButton *btn = createBtn(numToLetter(i) + ". " + elem.text());
            mLayoutButtons->addWidget(btn);
            i++;
        }
        node = node.nextSibling();
    }
}

void QuesChoice::writeXmlStuAns(QXmlStreamWriter &xml) const {
    QString ansStr;

    // 遍历 mLayoutButtons 将选中的索引写入到 ansStr 中
    int count = mLayoutButtons->count();
    for(int i = 0; i < count; ++i) {
        QAbstractButton *btn = (QAbstractButton*)mLayoutButtons->itemAt(i)->widget();
        if(btn->isChecked())
            ansStr += QString::number(i) + ';';
    }

    xml.writeTextElement(mQuesName, ansStr);
}

void QuesChoice::readXmlStuAns(const QDomElement &elem) {
    // 清空原有选中状态
    int count = mLayoutButtons->count();
    for(int i = 0; i < count; ++i) {
        QAbstractButton *btn = (QAbstractButton*)mLayoutButtons->itemAt(i)->widget();
        btn->setChecked(false);
    }

    // 读取选中状态
    QStringList ansList = elem.text().split(';', QString::KeepEmptyParts);
    for(const QString &ans : ansList) {
        int ind = ans.toInt();
        if(ind >= 0 && ind < count) {
            QAbstractButton *btn = (QAbstractButton*)mLayoutButtons->itemAt(ind)->widget();
            btn->setChecked(false);
        }
    }
}

QString QuesChoice::numToLetter(int num) {
    QString res;
    do {
        res += 'A' + num % 26;
        num /= 26;
    } while(num);
    return res;
}

QAbstractButton* QuesChoice::createBtn(const QString &) { return nullptr; }
