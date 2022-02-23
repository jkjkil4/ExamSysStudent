#pragma once

#include <QWidget>

class QLabel;
class QFrame;
class QHBoxLayout;

class QXmlStreamWriter;
class QDomElement;

/**
 * @brief   题目基类
 */
class Ques : public QWidget
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit Ques(QWidget *parent = nullptr);

    /**
     * @brief   设置序号
     * @param   num     序号
     */
    void setNumber(int num);

    virtual bool isDone(QString *pStr = nullptr) const;

    /**
     * @brief   使用XML方式对题目内容进行读取
     * @param   elem    QDomElement对象，用于读取XML
     */
    virtual void readXml(const QDomElement &elem);

    /**
     * @brief   使用XML方式对考生作答进行保存
     * @param   xml     QXmlStreamWriter对象，用于写入XML
     */
    virtual void writeXmlStuAns(QXmlStreamWriter &xml) const;
    /**
     * @brief   使用XML方式对考生作答进行加载
     * @param   elem    QDomElement对象，用于读取XML
     */
    virtual void readXmlStuAns(const QDomElement &elem);

protected:
    QLabel *mLabelNum;
    QFrame *mFrame;
    QHBoxLayout *mLayoutMain;
};
