#include "examview.h"
#include "ui_examview.h"

#include <QMessageBox>

#include <QTimer>

#include <QDomElement>
#include <QXmlStreamWriter>

#include "Ques/quessinglechoice.h"
#include "Ques/quesmultichoice.h"
#include "Ques/queswhether.h"
#include "Ques/quesinvalid.h"

ExamView::ExamView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamView), mLayoutQues(new QVBoxLayout),
    mTimeTimer(new QTimer(this)),
    availableQues({
                  {"QuesSingleChoice", &QuesSingleChoice::staticMetaObject},
                  {"QuesMultiChoice", &QuesMultiChoice::staticMetaObject},
                  {"QuesWhether", &QuesWhether::staticMetaObject}
                  })
{
    ui->setupUi(this);

    ui->splitter->setSizes(QList<int>() << 10 << 600);
    ui->widgetExam->setLayout(mLayoutQues);

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

void ExamView::readQues(const QDomElement &elem) {
    clearQues();
    QDomNode node = elem.firstChild();
    int i = 1;
    while(!node.isNull()) {
        QDomElement elem = node.toElement();
        do {
            if(elem.isNull())
                break;
            const QMetaObject *metaObj = availableQues.value(elem.tagName(), nullptr);
            Ques *ques = metaObj ? (Ques*)metaObj->newInstance() : new QuesInvalid(elem.tagName());
            if(!ques)
                break;
            ques->readXml(elem);
            ques->setNumber(i);
            mLayoutQues->addWidget(ques);
        } while(false);
        i++;
        node = node.nextSibling();
    }
    ui->labelQuesCnt->setText(QString::number(mLayoutQues->count()));
}

void ExamView::clearQues() {
    int count = mLayoutQues->count();
    for(int i = 0; i < count; ++i) {
        QLayoutItem *item = mLayoutQues->takeAt(0);
        item->widget()->deleteLater();
        delete item;
    }
}
void ExamView::clear() {
    mDateTimeStart = mDateTimeEnd = mDateTimeCur = QDateTime();
    ui->labelExamName->clear();
    ui->labelQuesCnt->clear();
    ui->labelStartTime->clear();
    ui->labelEndTime->clear();
    ui->labelCurTime->clear();
    ui->labelStuName->clear();
    clearQues();
    ui->widgetExam->setVisible(false);
    ui->btnStart->setEnabled(true);
}

void ExamView::setVisible(bool visible) {
    visible ? mTimeTimer->start(1000) : mTimeTimer->stop();
    QWidget::setVisible(visible);
}

void ExamView::onBtnStartClicked() {
    if(!mDateTimeStart.isValid() || !mDateTimeEnd.isValid() || !mDateTimeCur.isValid())
        return;
    if(mDateTimeCur < mDateTimeStart) {
        QMessageBox::information(this, "提示", "考试未开始");
        return;
    }
    if(mDateTimeEnd <= mDateTimeCur) {
        QMessageBox::information(this, "提示", "考试已结束");
        return;
    }
    ui->widgetExam->setVisible(true);
    ui->btnStart->setEnabled(false);
}

void ExamView::onTimeTimerTimeout() {
    if(!mDateTimeCur.isValid())
        return;
    setDateTime(mDateTimeCur, ui->labelCurTime, mDateTimeCur.addSecs(1));
    if(ui->widgetExam->isVisible()) {
        ++timeProcessCounter;
        if(timeProcessCounter >= 10) {
            timeProcessCounter = 0;
            // 得到当前进度
            int doneCnt = 0;
            int count = mLayoutQues->count();
            for(int i = 0; i < count; ++i) {
                Ques *ques = (Ques*)mLayoutQues->itemAt(i)->widget();
                if(ques->isDone())
                    ++doneCnt;
            }

            emit sendStuProcRequested(doneCnt * 100 / count);
        }
    }
}

void ExamView::setDateTime(QDateTime &v, QLabel *label, const QDateTime &dt) {
    v = dt;
    label->setText(dt.toString("yyyy/M/d HH:mm:ss"));
}
