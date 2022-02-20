#include "enterview.h"
#include "ui_enterview.h"

#include <QTimer>

EnterView::EnterView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnterView),
    mTimeTimer(new QTimer(this))
{
    ui->setupUi(this);
}

EnterView::~EnterView()
{
    delete ui;
}

void EnterView::setVisible(bool visible) {
    visible ? mTimeTimer->start(1000) : mTimeTimer->stop();
    QWidget::setVisible(visible);
}

void EnterView::onTimeTimerTimeout() {
    mDateTimeCur = mDateTimeCur.addSecs(1);
    // TODO: 设置控件文字
}
