#include "setelementdialog.h"
#include "ui_setelementdialog.h"

SetElementDialog::SetElementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetElementDialog)
{
    ui->setupUi(this);
}

double SetElementDialog::getMinElement()
{
    return MinElement;
}

double SetElementDialog::getMaxElement()
{
    return MaxElement;
}

SetElementDialog::~SetElementDialog()
{
    delete ui;
}

void SetElementDialog::on_pushButton_clicked()
{
    if (ui->MinEdit->text() != "") MinElement = ui->MinEdit->text().toDouble();
    else MinElement = 0;
    if (ui->MaxEdit->text() != "") MaxElement = ui->MaxEdit->text().toDouble();
    else MaxElement = 0;
}
