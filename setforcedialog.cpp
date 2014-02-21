#include "setforcedialog.h"
#include "ui_setforcedialog.h"

SetForceDialog::SetForceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetForceDialog)
{
    ui->setupUi(this);
}

SetForceDialog::~SetForceDialog()
{
    delete ui;
}

double SetForceDialog::getForce()
{
    return Force;
}

void SetForceDialog::on_pushButton_clicked()
{
    if (ui->lineEdit->text() != "") Force = ui->lineEdit->text().toDouble();
}
