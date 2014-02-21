#include "setscaledialog.h"
#include "ui_setscaledialog.h"
#include <QMessageBox>

SetScaleDialog::SetScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetScaleDialog)
{
    ui->setupUi(this);
}

SetScaleDialog::~SetScaleDialog()
{
    delete ui;
}

double SetScaleDialog::getScale()
{
    return Scale;
}

void SetScaleDialog::setPixels(double Pixels)
{
    pixels = Pixels;
}

void SetScaleDialog::on_pushButton_clicked()
{
    double mkm;
    //if ((ui->lineEdit->text()!="") && (ui->lineEdit->text()!="0"))
    //{
    if (ui->radioButton_1->isChecked()) mkm = 1000;
    else if (ui->radioButton_2->isChecked()) mkm = 100;
    else if (ui->radioButton_3->isChecked()) mkm = 50;
    else if (ui->radioButton_4->isChecked()) mkm = 10;
    else if (ui->radioButton_5->isChecked()) mkm = 1;
    else mkm = ui->lineEdit->text().toDouble();
    Scale = mkm / pixels;
    //}
}
