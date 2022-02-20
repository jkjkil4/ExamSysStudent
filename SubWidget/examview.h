#pragma once

#include <QWidget>

#include <QDateTime>

class QLabel;
class QTimer;

namespace Ui {
class ExamView;
}

class ExamView : public QWidget
{
    Q_OBJECT
public:
    explicit ExamView(QWidget *parent = nullptr);
    ~ExamView() override;

    void setExamName(const QString &examName);
    void setStartDateTime(const QDateTime &dt);
    void setEndDateTime(const QDateTime &dt);
    void setCurDateTime(const QDateTime &dt);

    void setStuName(const QString &stuName);

    void clear();

    void setVisible(bool visible) override;

public slots:
    void onBtnStartClicked();
    void onTimeTimerTimeout();

private:
    Ui::ExamView *ui;

    // 定时器，用于更新时间
    QTimer *mTimeTimer;

    // 相关时间
    QDateTime mDateTimeStart;
    QDateTime mDateTimeEnd;
    QDateTime mDateTimeCur;
    void setDateTime(QDateTime &v, QLabel *label, const QDateTime &dt);
};
