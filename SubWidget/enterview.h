#pragma once

#include <QWidget>

#include <QDateTime>

class QTimer;
class QLabel;

namespace Ui {
class EnterView;
}

/**
 * @brief   准备界面
 */
class EnterView : public QWidget
{
    Q_OBJECT
public:
    explicit EnterView(QWidget *parent = nullptr);
    ~EnterView() override;

    void setStartDateTime(const QDateTime &dt);
    void setEndDateTime(const QDateTime &dt);
    void setCurDateTime(const QDateTime &dt);

    void clear();

    void setVisible(bool visible) override;

public slots:
    void onTimeTimerTimeout();

private:
    Ui::EnterView *ui;

    // 定时器，用于更新时间
    QTimer *mTimeTimer;

    // 相关时间
    QDateTime mDateTimeStart;
    QDateTime mDateTimeEnd;
    QDateTime mDateTimeCur;
    void setDateTime(QDateTime &v, QLabel *label, const QDateTime &dt);
};
