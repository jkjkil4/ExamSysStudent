#include "enterview.h"
#include "ui_enterview.h"

EnterView::EnterView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnterView)
{
    ui->setupUi(this);
}

EnterView::~EnterView()
{
    delete ui;
}
