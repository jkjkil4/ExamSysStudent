#include "quessinglechoice.h"

#include <QRadioButton>
#include <QVBoxLayout>

QuesSingleChoice::QuesSingleChoice(QWidget *parent)
    : QuesChoice("QuesSingleChoice", "（单选题）", parent) {}

bool QuesSingleChoice::isDone(QString *pStr) const {
    int count = mLayoutButtons->count();
    for(int i = 0; i < count; ++i) {
        QAbstractButton *btn = (QAbstractButton*)mLayoutButtons->itemAt(i)->widget();
        if(btn->isChecked())
            return true;
    }
    if(pStr)
        *pStr = "未选择选项";
    return false;
}

QAbstractButton* QuesSingleChoice::createBtn(const QString &str) {
    return new QRadioButton(str);
}
