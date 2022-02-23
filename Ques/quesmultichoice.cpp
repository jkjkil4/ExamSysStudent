#include "quesmultichoice.h"

#include <QCheckBox>
#include <QVBoxLayout>

QuesMultiChoice::QuesMultiChoice(QWidget *parent)
    : QuesChoice("QuesMultiChoice", "（多选题）", parent) {}

bool QuesMultiChoice::isDone(QString *pStr) const {
    int checkedCnt = 0;
    int count = mLayoutButtons->count();
    for(int i = 0; i < count; ++i) {
        QAbstractButton *btn = (QAbstractButton*)mLayoutButtons->itemAt(i)->widget();
        if(btn->isChecked())
            ++checkedCnt;
    }
    if(checkedCnt == 0) {
        if(pStr)
            *pStr = "未选择选项";
        return false;
    }
    if(checkedCnt == 1) {
        if(pStr)
            *pStr = "选项应不少于2个";
        return false;
    }
    return true;
}

QAbstractButton* QuesMultiChoice::createBtn(const QString &str) {
    return new QCheckBox(str);
}
