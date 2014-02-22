#ifndef SETELEMENTDIALOG_H
#define SETELEMENTDIALOG_H

#include <QDialog>

namespace Ui {
class SetElementDialog;
}

class SetElementDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetElementDialog(QWidget *parent = 0);
    double getMinElement ();
    double getMaxElement ();
    ~SetElementDialog();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::SetElementDialog *ui;
    double MinElement;
    double MaxElement;
};

#endif // SETELEMENTDIALOG_H
