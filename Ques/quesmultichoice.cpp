#include "quesmultichoice.h"

#include <QCheckBox>

QuesMultiChoice::QuesMultiChoice(QWidget *parent)
    : QuesChoice("QuesMultiChoice", "（多选题）", parent) {}

QAbstractButton* QuesMultiChoice::createBtn(const QString &str) {
    return new QCheckBox(str);
}
