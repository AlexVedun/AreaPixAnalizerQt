#ifndef SETFORCEDIALOG_H
#define SETFORCEDIALOG_H

#include <QDialog>

namespace Ui {
class SetForceDialog;
}

class SetForceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetForceDialog(QWidget *parent = 0);
    ~SetForceDialog();
    double getForce ();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::SetForceDialog *ui;
    double Force;
};

#endif // SETFORCEDIALOG_H
