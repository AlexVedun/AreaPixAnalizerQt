#include "fillblackwhitedialog.h"
#include "ui_fillblackwhitedialog.h"

#include <QCloseEvent>
#include <QIntValidator>
#include <QSignalBlocker>
#include <QTimer>

FillBlackWhiteDialog::FillBlackWhiteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FillBlackWhiteDialog)
{
    ui->setupUi(this);
    setWindowModality(Qt::NonModal);

    auto *validator = new QIntValidator(0, 255, this);
    ui->whiteValueEdit->setValidator(validator);
    ui->blackValueEdit->setValidator(validator);

    ui->applyButton->setDefault(false);
    ui->applyButton->setAutoDefault(false);
    ui->cancelButton->setDefault(false);
    ui->cancelButton->setAutoDefault(false);

    ui->whiteSlider->installEventFilter(this);
    ui->blackSlider->installEventFilter(this);
    connect(ui->whiteSlider, &QSlider::valueChanged,
            this, &FillBlackWhiteDialog::onWhiteSliderValueChanged);
    connect(ui->blackSlider, &QSlider::valueChanged,
            this, &FillBlackWhiteDialog::onBlackSliderValueChanged);
    connect(ui->whiteSlider, &QSlider::sliderReleased,
            this, &FillBlackWhiteDialog::requestThresholdsChanged);
    connect(ui->blackSlider, &QSlider::sliderReleased,
            this, &FillBlackWhiteDialog::requestThresholdsChanged);
    connect(ui->whiteValueEdit, &QLineEdit::editingFinished,
            this, &FillBlackWhiteDialog::onWhiteValueEditFinished);
    connect(ui->blackValueEdit, &QLineEdit::editingFinished,
            this, &FillBlackWhiteDialog::onBlackValueEditFinished);
    connect(ui->applyButton, &QPushButton::clicked,
            this, &FillBlackWhiteDialog::onApplyButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &FillBlackWhiteDialog::onCancelButtonClicked);
}

FillBlackWhiteDialog::~FillBlackWhiteDialog()
{
    delete ui;
}

int FillBlackWhiteDialog::whiteThreshold() const
{
    return ui->whiteSlider->value();
}

int FillBlackWhiteDialog::blackThreshold() const
{
    return ui->blackSlider->value();
}

void FillBlackWhiteDialog::setThresholds(int white, int black)
{
    const int whiteValue = qBound(0, white, 255);
    const int blackValue = qBound(0, black, 255);
    syncWhiteControls(whiteValue);
    syncBlackControls(blackValue);
    ensureThresholdOrder(true);
}

void FillBlackWhiteDialog::resetThresholdsToDefault()
{
    QSignalBlocker whiteSliderBlocker(ui->whiteSlider);
    QSignalBlocker blackSliderBlocker(ui->blackSlider);
    QSignalBlocker whiteEditBlocker(ui->whiteValueEdit);
    QSignalBlocker blackEditBlocker(ui->blackValueEdit);

    ui->whiteSlider->setValue(255);
    ui->blackSlider->setValue(0);
    ui->whiteValueEdit->setText(QStringLiteral("255"));
    ui->blackValueEdit->setText(QStringLiteral("0"));
}

void FillBlackWhiteDialog::syncWhiteControls(int value)
{
    const int clamped = qBound(0, value, 255);
    QSignalBlocker sliderBlocker(ui->whiteSlider);
    QSignalBlocker editBlocker(ui->whiteValueEdit);
    ui->whiteSlider->setValue(clamped);
    ui->whiteValueEdit->setText(QString::number(clamped));
}

void FillBlackWhiteDialog::syncBlackControls(int value)
{
    const int clamped = qBound(0, value, 255);
    QSignalBlocker sliderBlocker(ui->blackSlider);
    QSignalBlocker editBlocker(ui->blackValueEdit);
    ui->blackSlider->setValue(clamped);
    ui->blackValueEdit->setText(QString::number(clamped));
}

void FillBlackWhiteDialog::ensureThresholdOrder(bool whiteChanged)
{
    const int white = whiteThreshold();
    const int black = blackThreshold();

    if (white < black) {
        if (whiteChanged) {
            syncBlackControls(white);
        } else {
            syncWhiteControls(black);
        }
    }
}

void FillBlackWhiteDialog::onWhiteSliderValueChanged(int value)
{
    syncWhiteControls(value);
    ensureThresholdOrder(true);
}

void FillBlackWhiteDialog::onBlackSliderValueChanged(int value)
{
    syncBlackControls(value);
    ensureThresholdOrder(false);
}

void FillBlackWhiteDialog::onWhiteValueEditFinished()
{
    bool ok = false;
    const int value = ui->whiteValueEdit->text().toInt(&ok);
    if (!ok) {
        syncWhiteControls(whiteThreshold());
        return;
    }

    syncWhiteControls(value);
    ensureThresholdOrder(true);
    requestThresholdsChanged();
}

void FillBlackWhiteDialog::onBlackValueEditFinished()
{
    bool ok = false;
    const int value = ui->blackValueEdit->text().toInt(&ok);
    if (!ok) {
        syncBlackControls(blackThreshold());
        return;
    }

    syncBlackControls(value);
    ensureThresholdOrder(false);
    requestThresholdsChanged();
}

void FillBlackWhiteDialog::onApplyButtonClicked()
{
    emit applyClicked();
}

void FillBlackWhiteDialog::onCancelButtonClicked()
{
    emit cancelClicked();
}

void FillBlackWhiteDialog::requestThresholdsChanged()
{
    emit thresholdsChanged();
}

bool FillBlackWhiteDialog::eventFilter(QObject *watched, QEvent *event)
{
    if ((watched == ui->whiteSlider || watched == ui->blackSlider) && event->type() == QEvent::Wheel) {
        QTimer::singleShot(0, this, [this]() {
            if (isVisible()) {
                emit thresholdsChanged();
            }
        });
    }

    return QDialog::eventFilter(watched, event);
}

void FillBlackWhiteDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QDialog::changeEvent(event);
}

void FillBlackWhiteDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->ignore();
        return;
    }
    QDialog::keyPressEvent(event);
}

void FillBlackWhiteDialog::closeEvent(QCloseEvent *event)
{
    emit cancelClicked();
    event->accept();
}
