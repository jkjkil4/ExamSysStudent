#ifndef ENTERVIEW_H
#define ENTERVIEW_H

#include <QWidget>

namespace Ui {
class EnterView;
}

class EnterView : public QWidget
{
    Q_OBJECT

public:
    explicit EnterView(QWidget *parent = nullptr);
    ~EnterView();

private:
    Ui::EnterView *ui;
};

#endif // ENTERVIEW_H
