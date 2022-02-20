#include "examview.h"
#include "ui_examview.h"

#include <QTimer>

ExamView::ExamView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamView),
    mTimeTimer(new QTimer(this))
{
    ui->setupUi(this);

    ui->splitter->setSizes(QList<int>() << 10 << 600);

    connect(ui->btnStart, &QPushButton::clicked, this, &ExamView::onBtnStartClicked);
    connect(mTimeTimer, &QTimer::timeout, this, &ExamView::onTimeTimerTimeout);
}

ExamView::~ExamView()
{
    delete ui;
}

void ExamView::setExamName(const QString &examName) { ui->labelExamName->setText(examName); }
void ExamView::setStartDateTime(const QDateTime &dt) { setDateTime(mDateTimeStart, ui->labelStartTime, dt); }
void ExamView::setEndDateTime(const QDateTime &dt) { setDateTime(mDateTimeEnd, ui->labelEndTime, dt); }
void ExamView::setCurDateTime(const QDateTime &dt) { setDateTime(mDateTimeCur, ui->labelCurTime, dt); }

void ExamView::setStuName(const QString &stuName) { ui->labelStuName->setText(stuName); }

void ExamView::clear() {
    mDateTimeStart = mDateTimeEnd = mDateTimeCur = QDateTime();
    ui->labelExamName->clear();
    ui->labelQuesCnt->clear();
    ui->labelStartTime->clear();
    ui->labelEndTime->clear();
    ui->labelCurTime->clear();
    ui->labelStuName->clear();
}

void ExamView::setVisible(bool visible) {
    visible ? mTimeTimer->start(1000) : mTimeTimer->stop();
    QWidget::setVisible(visible);
}

void ExamView::onBtnStartClicked() {

}

void ExamView::onTimeTimerTimeout() {
    if(!mDateTimeCur.isValid())
        return;
    setDateTime(mDateTimeCur, ui->labelCurTime, mDateTimeCur.addSecs(1));
}

void ExamView::setDateTime(QDateTime &v, QLabel *label, const QDateTime &dt) {
    v = dt;
    label->setText(dt.toString("yyyy/M/d HH:mm:ss"));
}
