#include "examview.h"
#include "ui_examview.h"

#include <QMessageBox>
#include <QScrollArea>

#include <QTimer>

#include <QDomElement>
#include <QXmlStreamWriter>

#include "Ques/quessinglechoice.h"
#include "Ques/quesmultichoice.h"
#include "Ques/queswhether.h"
#include "Ques/quesinvalid.h"

ExamView::ExamView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExamView),
    mLayoutExam(new QVBoxLayout), mLayoutQues(new QVBoxLayout),
    mTimeTimer(new QTimer(this)),
    availableQues({
                  {"QuesSingleChoice", &QuesSingleChoice::staticMetaObject},
                  {"QuesMultiChoice", &QuesMultiChoice::staticMetaObject},
                  {"QuesWhether", &QuesWhether::staticMetaObject}
                  })
{
    ui->setupUi(this);

    ui->splitter->setSizes(QList<int>() << 10 << 600);
    mLayoutExam->addLayout(mLayoutQues);
    mLayoutExam->addStretch();
    ui->widgetExam->widget()->setLayout(mLayoutExam);
    ui->widgetExam->widget()->setStyleSheet("QWidget#widgetExamContents{background-color: white;}");

    connect(ui->btnStart, &QPushButton::clicked, this, &ExamView::onBtnStartClicked);
    connect(ui->btnCheck, &QPushButton::clicked, this, &ExamView::onBtnCheckClicked);
    connect(ui->btnUpload, SIGNAL(clicked()), this, SIGNAL(sendStuAnsRequested()));
    connect(ui->btnFinish, SIGNAL(clicked()), this, SIGNAL(sendStuFinishRequested()));
    connect(ui->btnReconnect, SIGNAL(clicked()), this, SIGNAL(reconnectRequested()));
    connect(ui->btnExit, &QPushButton::clicked, this, &ExamView::onBtnExitClicked);
    connect(mTimeTimer, &QTimer::timeout, this, &ExamView::onTimeTimerTimeout);
}

ExamView::~ExamView()
{
    delete ui;
}

void ExamView::setExamName(const QString &examName) { ui->labelExamName->setText(examName); }
QString ExamView::examName() { return ui->labelExamName->text(); }
void ExamView::setStartDateTime(const QDateTime &dt) { setDateTime(mDateTimeStart, ui->labelStartTime, dt); }
void ExamView::setEndDateTime(const QDateTime &dt) { setDateTime(mDateTimeEnd, ui->labelEndTime, dt); }
void ExamView::setCurDateTime(const QDateTime &dt) { setDateTime(mDateTimeCur, ui->labelCurTime, dt); }
void ExamView::setSrvState(bool isConnected) {
    ui->labelSrvStat->setText(isConnected ? "已连接" : "未连接");
    ui->btnReconnect->setVisible(!isConnected);
}

void ExamView::setStuName(const QString &stuName) { ui->labelStuName->setText(stuName); }

void ExamView::setLastUploadDateTime(const QDateTime &dt) { ui->labelUploadTime->setText(dt.toString("HH:mm:ss")); }

void ExamView::readXmlQues(const QDomElement &elem) {
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

void ExamView::readXmlStuAns(const QDomElement &elem) {
    setLastUploadDateTime(QDateTime::fromString(elem.attribute("Time"), "yyyy/M/d H:m:s"));
    QDomNode node = elem.firstChild();
    int i = 0;
    while(!node.isNull()) {
        QDomElement elem = node.toElement();
        if(!elem.isNull()) {
            Ques *ques = (Ques*)mLayoutQues->itemAt(i)->widget();
            ques->readXmlStuAns(elem);
            ++i;
        }
        node = node.nextSibling();
    }
}

void ExamView::writeXmlStuAns(QXmlStreamWriter &xml) const {
    int count = mLayoutQues->count();
    for(int i = 0; i < count; ++i) {
        Ques *ques = (Ques*)mLayoutQues->itemAt(i)->widget();
        ques->writeXmlStuAns(xml);
    }
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
    ui->labelSrvStat->clear();
    ui->labelStuName->clear();
    clearQues();
    setExamVisible(false);
    ui->btnStart->setEnabled(true);
    ui->labelUploadTime->setText("暂无");
}

void ExamView::setExamVisible(bool visible) {
    ui->widgetExam->setVisible(visible);
    ui->btnStart->setVisible(!visible);
    ui->widgetButtons->setVisible(visible);
}

void ExamView::setVisible(bool visible) {
    visible ? mTimeTimer->start(1000) : mTimeTimer->stop();
    QWidget::setVisible(visible);
}

QScrollArea* ExamView::newCheckWidget() {
    struct Unfinished { int ind; QString what; };

    // 遍历题目检查是否有未完成的题目
    QList<Unfinished> listUnfinished;
    int count = mLayoutQues->count();
    for(int i = 0; i < count; ++i) {
        Ques *ques = (Ques*)mLayoutQues->itemAt(i)->widget();
        QString what;
        if(!ques->isDone(&what))
            listUnfinished << Unfinished{ i, what };
    }

    // 如果没有未完成的题目，则退出函数
    if(listUnfinished.isEmpty())
        return nullptr;

    // 创建控件并显示
    QVBoxLayout *layoutArea = new QVBoxLayout;
    for(const Unfinished &unf : listUnfinished) {
        QLabel *label = new QLabel(QString::number(unf.ind + 1) + '.');
        label->setObjectName("left");
        QLabel *labelWhat = new QLabel(unf.what);
        labelWhat->adjustSize();
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(label);
        layout->addWidget(labelWhat, 1);
        layoutArea->addLayout(layout);
    }
    layoutArea->addStretch();

    QScrollArea *area = new QScrollArea;
    area->setStyleSheet("QScrollArea{"
                        "    background-color: white;"
                        "}"
                        "QLabel#left{"
                        "    border: 2px solid rgb(26, 222, 209);"
                        "    color: #FFFFFF;"
                        "    background-color: rgb(26, 222, 209);"
                        "}");
    QWidget *areaWidget = new QWidget;
    areaWidget->setObjectName("areaWidget");
    areaWidget->setStyleSheet("QWidget#areaWidget{"
                              "    background-color: white;"
                              "}");
    areaWidget->setLayout(layoutArea);
    area->setWidget(areaWidget);

    return area;
}

int ExamView::proc() {
    int doneCnt = 0;
    int count = mLayoutQues->count();
    for(int i = 0; i < count; ++i) {
        Ques *ques = (Ques*)mLayoutQues->itemAt(i)->widget();
        if(ques->isDone())
            ++doneCnt;
    }
    return doneCnt * 100 / count;
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
    setExamVisible(true);
    ui->btnStart->setEnabled(false);
}
void ExamView::onBtnCheckClicked() {
    QScrollArea *widget = newCheckWidget();
    if(!widget) {
        QMessageBox::information(this, "提示", "所有题目已完成");
        return;
    }

    QHBoxLayout *layoutDialog = new QHBoxLayout;
    layoutDialog->setContentsMargins(QMargins());
    layoutDialog->addWidget(widget);

    QDialog dialog(this);
    dialog.setWindowTitle("以下题目未完成");
    dialog.setLayout(layoutDialog);
    dialog.resize(300, 400);
    dialog.exec();
}
void ExamView::onBtnExitClicked() {
    int ret = QMessageBox::information(this, "提示", "确认登出吗?", "确定", "取消");
    if(ret != 0)
        return;
    emit logout();
}

void ExamView::onTimeTimerTimeout() {
    if(!mDateTimeCur.isValid())
        return;
    setDateTime(mDateTimeCur, ui->labelCurTime, mDateTimeCur.addSecs(1));
    if(ui->widgetExam->isVisible()) {
        ++timeProcessCounter;
        if(timeProcessCounter >= 10) {
            timeProcessCounter = 0;
            emit sendStuProcRequested();
        }
    }
}

void ExamView::setDateTime(QDateTime &v, QLabel *label, const QDateTime &dt) {
    v = dt;
    label->setText(dt.toString("yyyy/M/d HH:mm:ss"));
}
