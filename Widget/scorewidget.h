#pragma once

#include <QWidget>

class QDomElement;

class QToolBar;
class QAction;
class QScrollArea;
class QVBoxLayout;

class ScoreWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScoreWidget(const QDomElement &elem, QWidget *parent = nullptr);

private:
    QToolBar *mToolBar;
    QAction *mActPrevWrong, *mActNextWrong;

    QScrollArea *mScrollArea;
    QWidget *mScrollAreaWidget;
    QVBoxLayout *mLayout;

    QVector<bool> mVecIsRight;
};
