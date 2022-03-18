#include "scorewidget.h"

#include <QApplication>
#include <QStyle>
#include <QVBoxLayout>

#include <QToolBar>
#include <QAction>
#include <QScrollArea>
#include <QLabel>
#include <QFrame>

#include <QDomElement>

#include <QDebug>

ScoreWidget::ScoreWidget(const QDomElement &elem, QWidget *parent)
    : QWidget(parent),
      mToolBar(new QToolBar), mActPrevWrong(new QAction), mActNextWrong(new QAction),
      mScrollArea(new QScrollArea), mScrollAreaWidget(new QWidget), mLayout(new QVBoxLayout)
{
    mActPrevWrong->setIcon(QIcon(":/icon/src/ArrowUp.png"));
    mActPrevWrong->setToolTip("前一个错题");
    mActNextWrong->setIcon(QIcon(":/icon/src/ArrowDown.png"));
    mActNextWrong->setToolTip("后一个错题");
    mToolBar->addAction(mActPrevWrong);
    mToolBar->addAction(mActNextWrong);
    mToolBar->setStyleSheet("QToolBar{background-color: rgb(24,204,192);}");
    mScrollArea->setObjectName("ScrArea");
    mScrollArea->setStyleSheet("QScrollArea#ScrArea{background-color: white;}"
                               "QLabel#LabelNum{"
                               "    border: 2px solid rgb(26, 222, 209);"
                               "    color: #FFFFFF;"
                               "    background-color: rgb(26, 222, 209);"
                               "}"
                               "QFrame#QuesBox{border: 1px solid #DDDDDD;}");
    mScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mScrollArea->setWidgetResizable(true);
    mScrollAreaWidget->setObjectName("ScrAreaWidget");
    mScrollAreaWidget->setStyleSheet("QWidget#ScrAreaWidget{background-color: white;}");

    int i = 1;
    QDomNode node = elem.firstChild();
    while(!node.isNull()) {
        QDomElement elem = node.toElement();
        if(!elem.isNull()) {
            mVecIsRight << elem.attribute("Right").toInt();

            QLabel *label = new QLabel(QString::number(i));
            label->setObjectName("LabelNum");
            QLabel *labelHtml = new QLabel(elem.text());
            labelHtml->setWordWrap(true);
            QFrame *frame = new QFrame;
            frame->setObjectName("QuesBox");

            QHBoxLayout *layoutFrame = new QHBoxLayout;
            layoutFrame->addWidget(labelHtml);
            frame->setLayout(layoutFrame);
            QHBoxLayout *layout = new QHBoxLayout;
            layout->addWidget(label, 0, Qt::AlignTop);
            layout->addWidget(frame, 1);

            mLayout->addLayout(layout);

            ++i;
        }
        node = node.nextSibling();
    }
    mLayout->addStretch();
    mScrollAreaWidget->setLayout(mLayout);
    mScrollArea->setWidget(mScrollAreaWidget);

    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->setContentsMargins(QMargins());
    layoutMain->setSpacing(0);
    layoutMain->addWidget(mToolBar);
    layoutMain->addWidget(mScrollArea);
    setLayout(layoutMain);
    resize(1050, 650);
}
