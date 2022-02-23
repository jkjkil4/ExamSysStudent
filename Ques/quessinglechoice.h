#pragma once

#include "queschoice.h"

class QuesSingleChoice : public QuesChoice
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QuesSingleChoice(QWidget *parent = nullptr);

    bool isDone(QString *pStr = nullptr) const override;
    QAbstractButton* createBtn(const QString &str) override;
};
