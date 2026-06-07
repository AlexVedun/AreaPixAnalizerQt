#ifndef FILLBLACKWHITEDIALOG_H
#define FILLBLACKWHITEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QEvent>
#include <QKeyEvent>

namespace Ui {
class FillBlackWhiteDialog;
}

class FillBlackWhiteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FillBlackWhiteDialog(QWidget *parent = nullptr);
    ~FillBlackWhiteDialog() override;

    int whiteThreshold() const;
    int blackThreshold() const;
    void setThresholds(int white, int black);
    void resetThresholdsToDefault();

signals:
    void thresholdsChanged();
    void applyClicked();
    void cancelClicked();

private slots:
    void onWhiteSliderValueChanged(int value);
    void onBlackSliderValueChanged(int value);
    void onWhiteValueEditFinished();
    void onBlackValueEditFinished();
    void onApplyButtonClicked();
    void onCancelButtonClicked();

protected:
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void syncWhiteControls(int value);
    void syncBlackControls(int value);
    void ensureThresholdOrder(bool whiteChanged);
    void requestThresholdsChanged();

    Ui::FillBlackWhiteDialog *ui;
};

#endif // FILLBLACKWHITEDIALOG_H
