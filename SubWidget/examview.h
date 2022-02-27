#pragma once

#include <QWidget>

#include <QDateTime>
#include <QMap>

class QLabel;
class QVBoxLayout;
class QTimer;
class QDomElement;
class QXmlStreamWriter;

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
    QDateTime curDateTime() { return mDateTimeCur; }
    void setSrvState(bool isConnected);
    void setExamId(const QString &id) { mExamId = id; }
    QString examId() { return mExamId; }

    void setStuName(const QString &stuName);

    void setLastUploadDateTime(const QDateTime &dt);

    /**
     * @brief   读取题目
     * @param   elem    QDomElement对象，用于读取XML
     */
    void readXmlQues(const QDomElement &elem);

    /**
     * @brief   读取考生作答
     * @param   elem    QDomElement对象，用于读取XML
     */
    void readXmlStuAns(const QDomElement &elem);

    /**
     * @brief   写入考生答案
     * @param   xml     QXmlStreamWriter对象，用于写入XML
     * 未使用总Element对所有子节点进行包含，而是直接写入子节点
     */
    void writeXmlStuAns(QXmlStreamWriter &xml) const;

    void clearQues();
    void clear();

    void setExamVisible(bool visible);

    void setVisible(bool visible) override;

    bool checkIsDone();
    int proc();

signals:
    void sendStuProcRequested();
    void sendStuAnsRequested();
    void sendStuFinishRequested();
    void reconnectRequested();
    void logout();

public slots:
    void onBtnStartClicked();
    void onBtnCheckClicked();
    void onBtnExitClicked();

    void onTimeTimerTimeout();

private:
    Ui::ExamView *ui;

    QVBoxLayout *mLayoutExam, *mLayoutQues;
    QString mExamId;

    // 定时器，用于更新时间
    QTimer *mTimeTimer;
    int timeProcessCounter = 0;

    // 相关时间
    QDateTime mDateTimeStart;
    QDateTime mDateTimeEnd;
    QDateTime mDateTimeCur;
    void setDateTime(QDateTime &v, QLabel *label, const QDateTime &dt);

    // 题目相关
    QMap<QString, const QMetaObject *> availableQues;
};
