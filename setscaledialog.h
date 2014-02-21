#ifndef SETSCALEDIALOG_H
#define SETSCALEDIALOG_H

#include <QDialog>

namespace Ui {
class SetScaleDialog;
}

class SetScaleDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetScaleDialog(QWidget *parent = 0);
    ~SetScaleDialog();
    double getScale();
    void setPixels (double Pixels);
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::SetScaleDialog *ui;
    double Scale;
    double pixels;

};

#endif // SETSCALEDIALOG_H
