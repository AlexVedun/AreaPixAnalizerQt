#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fillblackwhitedialog.h"
#include "setscaledialog.h"
#include "setforcedialog.h"
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTimer>

static auto kReleaseApiUrl = "https://api.github.com/repos/AlexVedun/AreaPixAnalizerQt/releases/latest";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    FillMaskItem(nullptr),
    FillWhiteThreshold(255),
    FillBlackThreshold(0),
    networkManager(new QNetworkAccessManager(this)),
    updateCheckInProgress(false)
{
    ui->setupUi(this);
    
    translator = new QTranslator(this);
    translatorQt = new QTranslator(this);

    LanguageGroup = new QActionGroup(this);
    LanguageGroup->addAction(ui->EnglishLang_action);
    LanguageGroup->addAction(ui->RussianLang_action);
    LanguageGroup->addAction(ui->UkrainianLang_action);
    ui->EnglishLang_action->setChecked(true);
    // Layout and Toolbar fixes
    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 1);
    ui->splitter->setSizes(QList<int>({1000, 200})); 
    ui->mainToolBar->setIconSize(QSize(16, 16));
    ui->mainToolBar->setStyleSheet("QToolButton { min-width: 24px; min-height: 24px; max-width: 24px; max-height: 24px; }");

    Scale = 1;
    isSetScale = false;
    isSetScaleSegment = false;
    isDiag1 = true;
    Diag1 = 0;
    Diag2 = 0;
    isForceSet = false;

    Scene = new QMyGraphicsScene;
    ui->ImageView->setScene(Scene);
    MainChart = new QChart();
    MainChart->legend()->hide();
    MainChart->setMargins(QMargins(0, 0, 0, 0));
    MainChart->setBackgroundVisible(true);
    MainChart->setBackgroundBrush(QBrush(Qt::white));
    MainChart->setPlotAreaBackgroundVisible(true);
    MainChart->setPlotAreaBackgroundBrush(QBrush(Qt::white));
    MainChart->setBackgroundRoundness(0);
    MainChart->setDropShadowEnabled(false);
    MainChart->layout()->setContentsMargins(0, 0, 0, 0);

    PlotCurve = new QLineSeries();
    PlotCurve->setColor(Qt::red);
    PlotCurveCarbides = new QLineSeries();
    PlotCurveCarbides->setColor(Qt::darkBlue);

    MainChart->addSeries(PlotCurve);
    MainChart->addSeries(PlotCurveCarbides);

    AxisX = new QValueAxis();
    AxisX->setRange(0.0, 1.0);
    AxisX->setTitleText(tr("px"));
    AxisX->setLabelFormat("%.0f");
    AxisX->setTickType(QValueAxis::TicksDynamic);
    AxisX->setTickAnchor(0.0);
    AxisX->setTickInterval(100.0);
    AxisX->setMinorTickCount(1);
    AxisX->setGridLineVisible(true);
    AxisX->setMinorGridLineVisible(true);
    AxisX->setLabelsColor(Qt::black);
    AxisX->setTitleBrush(QBrush(Qt::black));
    AxisX->setLinePen(QPen(Qt::black));
    AxisX->setGridLinePen(QPen(QColor(190, 190, 190), 1, Qt::DotLine));
    AxisX->setMinorGridLinePen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));

    AxisY = new QValueAxis();
    AxisY->setRange(0.0, 260.0);
    AxisY->setLabelFormat("%.0f");
    AxisY->setTickType(QValueAxis::TicksFixed);
    AxisY->setTickCount(6);
    AxisY->setMinorTickCount(1);
    AxisY->setTruncateLabels(false);
    AxisY->setGridLineVisible(true);
    AxisY->setMinorGridLineVisible(true);
    AxisY->setLabelsColor(Qt::black);
    AxisY->setTitleBrush(QBrush(Qt::black));
    AxisY->setLinePen(QPen(Qt::black));
    AxisY->setGridLinePen(QPen(QColor(190, 190, 190), 1, Qt::DotLine));
    AxisY->setMinorGridLinePen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));

    MainChart->addAxis(AxisX, Qt::AlignBottom);
    MainChart->addAxis(AxisY, Qt::AlignLeft);
    PlotCurve->attachAxis(AxisX);
    PlotCurve->attachAxis(AxisY);
    PlotCurveCarbides->attachAxis(AxisX);
    PlotCurveCarbides->attachAxis(AxisY);

    MainChartView = new QChartView(MainChart, ui->MainPlot);
    MainChartView->setRenderHint(QPainter::Antialiasing);
    MainChartView->setFrameShape(QFrame::StyledPanel);
    MainChartView->setFrameShadow(QFrame::Sunken);
    MainChartView->setRubberBand(QChartView::NoRubberBand);
    MainChartView->setMouseTracking(true);
    MainChartView->viewport()->setCursor(Qt::CrossCursor);

    PlotCursorText = MainChart->scene()->addSimpleText(QString());
    PlotCursorText->setBrush(QBrush(Qt::black));
    PlotCursorText->setZValue(20.0);
    hidePlotCursorOverlay();

    auto *plotLayout = new QVBoxLayout(ui->MainPlot);
    plotLayout->setContentsMargins(0, 0, 0, 0);
    plotLayout->addWidget(MainChartView);
    MainChartView->viewport()->installEventFilter(this);
    ModeGroup = new QActionGroup(this);
    ModeGroup->addAction(ui->Line_action);
    ModeGroup->addAction(ui->Area_action);
    ModeGroup->addAction(ui->CalibrMode_action);
    ModeGroup->addAction(ui->Microhardness_action);
    ModeGroup->addAction(ui->BlackWhite_action);
    MeasureUnitsGroup = new QActionGroup (this);
    MeasureUnitsGroup->addAction(ui->SetPix_action);
    MeasureUnitsGroup->addAction(ui->SetMkm_action);
    MeasureUnitsGroup->addAction(ui->SetPercent_action);
    updateXAxisTitle();

    ui->mainToolBar->addAction(ui->OpenImage_action);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->Line_action);
    ui->mainToolBar->addAction(ui->Area_action);
    ui->mainToolBar->addAction(ui->CalibrMode_action);
    ui->mainToolBar->addAction(ui->Microhardness_action);
    ui->mainToolBar->addAction(ui->BlackWhite_action);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->Make_bigger_action);
    ui->mainToolBar->addAction(ui->Make_Smaller_action);
//    ui->mainToolBar->addSeparator();
//    ui->mainToolBar->addAction(ui->ClearMarkers_action);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->Exit_action);

    ui->ClearMarkers_action->setVisible(false);

    cleanupOldExecutable();

    ReadyStatus = new QLabel;
    //ReadyStatus->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(ReadyStatus);
    //ScaleSegment = new QLabel (cScaleSegment);
    //statusBar()->addWidget(ScaleSegment);
    LineLength = new QLabel;
    statusBar()->addWidget(LineLength);
    NormalAreaPix = new QLabel;
    statusBar()->addWidget(NormalAreaPix);
    UpDownPix = new QLabel;
    statusBar()->addWidget(UpDownPix);
    Microhardness = new QLabel;
    statusBar()->addWidget(Microhardness);
    BlackWhiteResult = new QLabel;
    statusBar()->addWidget(BlackWhiteResult);
    UpdateStatus = new QLabel;
    currentUpdateMsgType = UpdateNone;
    UpdateStatus->hide();
    statusBar()->addPermanentWidget(UpdateStatus);

    Progress = new QProgressBar;
    Progress->setMaximumSize(100, statusBar()->height());
    Progress->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(Progress);
    
    updateStatusBarLabels();

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    setGeometry(screenWidth / 8, screenHeight / 8, (screenWidth * 3) / 4, (screenHeight * 3) / 4);
    setWindowTitle(QApplication::applicationName() + " " + QApplication::applicationVersion());

    QSettings Settings;
    WorkingDir = Settings.value("/Settings/WorkingDir", "").toString();
    QString lang = Settings.value("/Settings/Language", "en").toString();
    if (lang == "en") {
        ui->EnglishLang_action->setChecked(true);
    } else if (lang == "uk") {
        ui->UkrainianLang_action->setChecked(true);
    } else {
        ui->RussianLang_action->setChecked(true);
    }
    switchLanguage(lang);
    cleanupOldExecutable();
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        if (pendingUpdateExePath.isEmpty()) {
            return;
        }

        const QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        const QFileInfo appInfo(appPath);
        const QString oldExe = QDir::toNativeSeparators(
            appInfo.absolutePath() + "/" + appInfo.completeBaseName() + "_old." + appInfo.suffix());
        const QString newExe = QDir::toNativeSeparators(pendingUpdateExePath);
        const QString command = QString("timeout /t 1 /nobreak >nul && if exist \"%1\" del /f /q \"%2\" && move /y \"%1\" \"%2\" >nul && copy /y \"%3\" \"%1\" >nul && start \"\" \"%1\"")
                                    .arg(appPath, oldExe, newExe);
        QProcess::startDetached("cmd.exe", QStringList() << "/c" << command, appInfo.absolutePath());
    });
    QTimer::singleShot(0, this, [this]() { checkForUpdates(false); });

    connect (Scene, SIGNAL(MouseButRelease()), SLOT(MouseButtonRelease()));
    connect (Scene, SIGNAL(MouseButPress()), SLOT(MouseButtonPress()));
    connect (Scene, SIGNAL(MouseButMove()), SLOT(MouseButtonMove()));
    connect (ui->AboutQt_action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainWindow::~MainWindow()
{
    QSettings Settings;
    Settings.setValue("/Settings/WorkingDir", WorkingDir);
    
    QString lang = "ru";
    if (ui->EnglishLang_action->isChecked()) {
        lang = "en";
    } else if (ui->UkrainianLang_action->isChecked()) {
        lang = "uk";
    }
    Settings.setValue("/Settings/Language", lang);

    delete Scene;
    delete ui;
}

void MainWindow::MouseButtonPress()
{
    QPen TempPen;
    TempPen.setColor(Qt::red);
    TempPen.setWidth(2);

    if (!MainImage) {
        return;
    }

    if (ui->Line_action->isChecked()) {
        //
        LineItem = new QGraphicsLineItem ();
        Scene->addItem(LineItem);
        LineItem->setPen(TempPen);
    } else if (ui->Area_action->isChecked()) {
        //
        RectItem = new QGraphicsRectItem ();
        Scene->addItem(RectItem);
        RectItem->setPen(TempPen);
    } else if (ui->CalibrMode_action->isChecked()) {
        //
        LineItem = new QGraphicsLineItem ();
        Scene->addItem(LineItem);
        LineItem->setPen(TempPen);
    } else if (ui->Microhardness_action->isChecked()) {
        //
        if (isForceSet) {
            LineItem = new QGraphicsLineItem ();
            Scene->addItem(LineItem);
            LineItem->setPen(TempPen);
        } else QMessageBox::warning(ui->centralWidget, tr("Attention!"),
                                  tr("Force value not set. It can be set via 'Mode' menu."),
                                  QMessageBox::Ok);
    }
}

void MainWindow::MouseButtonRelease()
{
    if (!MainImage) {
        return;
    }

    if (ui->Line_action->isChecked()) {
        //
        Line (Scene->getTopLeft(), Scene->getBottomRight());
    } else if (ui->Area_action->isChecked()) {
        //
        Area (Scene->getTopLeft(), Scene->getBottomRight());
    } else if (ui->CalibrMode_action->isChecked()) {
        //
        ScaleInPixels = Scene->getBottomRight().x()-Scene->getTopLeft().x();
        //ScaleSegment->setText(cScaleSegment+QString::number(ScaleInPixels)+" ");
        isSetScaleSegment = true;
    } else if (ui->Microhardness_action->isChecked()) {
        //
        if (isForceSet) {
            int deltaX = abs(Scene->getBottomRight().x() - Scene->getTopLeft().x());
            int deltaY = abs(Scene->getBottomRight().y() - Scene->getTopLeft().y());
            int count = deltaY>deltaX ? deltaY : deltaX;
            if (isDiag1) {
                isDiag1 = false;
                Diag1 = count * Scale / 1000;
            } else {
                isDiag1 = true;
                Diag2 = count * Scale / 1000;
                double Diag = (Diag1 + Diag2)/2;
                double HV = 1.854 * Force / (Diag * Diag);
                Microhardness->setText(tr(" Microhardness: ") + QString::number(HV) + " " + tr("HV") + " ");
            }
        }
    } else if (ui->BlackWhite_action->isChecked()) {
        //
        BlackWhite(Scene->getTopLeft());
    }
}

void MainWindow::PlotSelectPoint(QPointF pos)
{
    const int CheckPoint = pos.y();
    int UpCount = 0;
    int DownCount = 0;
    QVector<QPointF> carbidePoints;
    carbidePoints.reserve(dataX.size());

    ReadyStatus->setText(tr(" Wait "));
    PlotCurveCarbides->setVisible(true);
    for (int i = 0; i < dataX.size(); i++)
    {
        if (dataY[i] >= CheckPoint)
        {
            carbidePoints.append(QPointF(dataX[i], CheckPoint));
            UpCount++;
        }
        else
        {
            carbidePoints.append(QPointF(dataX[i], 0));
            DownCount++;
        }
    }
    PlotCurveCarbides->replace(carbidePoints);
//    if (isSetScale) UpDownPix->setText(cUpDownPix+cMkm+" "+QString::number(UpCount * Scale)+" / "+
//                                                           QString::number(DownCount * Scale));
//    else UpDownPix->setText(cUpDownPix+cPix+" "+QString::number(UpCount)+" / "+QString::number(DownCount));
    if (ui->SetPix_action->isChecked()) {
        UpDownPix->setText(
            tr(" Matrix / Carbides: ") +
            tr("px") + " " +
            QString::number(UpCount) + " / " +
            QString::number(DownCount) + " "
        );
    } else if (ui->SetMkm_action->isChecked()) {
        UpDownPix->setText(
            tr(" Matrix / Carbides: ") +
            tr("um") + " " +
            QString::number(UpCount * Scale) + " / " +
            QString::number(DownCount * Scale) + " "
        );
    } else {
        UpDownPix->setText(
            tr(" Matrix / Carbides: ") +
            tr("%") + " " +
            QString::number((int)(UpCount * 100 / (UpCount + DownCount))) + " / " +
            QString::number((int)(DownCount * 100 / (UpCount + DownCount))) + " "
        );
    }
    ReadyStatus->setText(tr(" Ready "));
}

void MainWindow::Line(QPointF BeginPoint, QPointF EndPoint)
{
    BeginPoint = MainImage->mapFromScene(BeginPoint);
    EndPoint = MainImage->mapFromScene(EndPoint);
    int x1 = BeginPoint.x();
    int y1 = BeginPoint.y();
    int x2 = EndPoint.x();
    int y2 = EndPoint.y();
    int deltaX = abs(x2 - x1);
    int deltaY = abs(y2 - y1);
    int signX = x1 < x2 ? 1 : -1;
    int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
    int count = deltaY>deltaX ? deltaY : deltaX;
    AxisX->setRange(0.0, count * Scale);
    AxisX->setTickAnchor(0.0);
    if (ui->SetPix_action->isChecked() || ui->SetPercent_action->isChecked()) {
        AxisX->setTickInterval(100.0);
    } else {
        AxisX->setTickInterval(100.0 * Scale);
    }
//    if (isSetScale) AxisX->setTitleText(tr("um"));
    updateXAxisTitle();
    QImage TempImage = MainImage->pixmap().toImage();
    int i = 0;

    Progress->reset();
    Progress->setMinimum(0);
    Progress->setMaximum(count);
    dataX.clear();
    dataY.clear();
    QVector<QPointF> linePoints;
    linePoints.reserve(count + 1);
    PlotCurveCarbides->setVisible(false);
    ReadyStatus->setText(tr(" Wait "));
    for (;;)
    {
        dataX.append(i*Scale);
        dataY.append(255-qGray (TempImage.pixel(x1, y1)));
        linePoints.append(QPointF(dataX.back(), dataY.back()));
        QCoreApplication::processEvents();

        if(x1 == x2 && y1 == y2) break;

        int error2 = error * 2;

        if(error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }

        if(error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
        Progress->setValue(i);
        i++;
    }
    PlotCurve->replace(linePoints);
    Progress->setValue(count);
    if (ui->SetMkm_action->isChecked()) LineLength->setText(tr(" Line length: ") + " " + QString::number(count*Scale) + " " + tr("um"));
    else LineLength->setText(tr(" Line length: ") + " " + QString::number(count) + " " + tr("px"));
    ReadyStatus->setText(tr(" Ready "));
}

void MainWindow::Area(QPointF TopLeft, QPointF BottomRight)
{
    TopLeft = MainImage->mapFromScene(TopLeft);
    BottomRight = MainImage->mapFromScene(BottomRight);
    int x1 = TopLeft.x();
    int y1 = TopLeft.y();
    int x2 = BottomRight.x();
    int y2 = BottomRight.y();
    QImage TempImage = MainImage->pixmap().toImage();
    long pix = 0;
    long count = 0;

    Progress->reset();
    Progress->setMinimum(x1);
    Progress->setMaximum(x2);
    ReadyStatus->setText(tr(" Wait "));
    for (int i = x1; i<=x2; i++)
    {
        for (int j = y1; j<=y2; j++)
        {
            pix += (255-qGray (TempImage.pixel(i, j)));
            count++;
            QCoreApplication::processEvents();
        }
        Progress->setValue(i);
    }
    Progress->setValue(x2);
    NormalAreaPix->setText(tr(" Average intensity: ") + QString::number(pix/count) + " ");
    ReadyStatus->setText(tr(" Ready "));
}

void MainWindow::BlackWhite(QPointF point)
{
    point = MainImage->mapFromScene(point);
    QImage TempImage = MainImage->pixmap().toImage();
    int imageWidth = TempImage.width();
    int imageHeight = TempImage.height();
    int totalPixels = imageWidth * imageHeight;
    int pointColor = qGray(TempImage.pixel(point.x(), point.y()));
    int totalPointColorPixels = 0;

    Progress->reset();
    Progress->setMinimum(0);
    Progress->setMaximum(totalPixels);
    ReadyStatus->setText(tr(" Wait "));
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            if (qGray(TempImage.pixel(i, j)) == pointColor) {
                totalPointColorPixels++;
            }
            QCoreApplication::processEvents();
            Progress->setValue(i * imageWidth + j);
        }
    }
    Progress->setValue(totalPixels);
    double area = static_cast<double>(totalPointColorPixels) / totalPixels * 100;
    BlackWhiteResult->setText(tr(" Area: ") + QString::number(area) + " % ");
    ReadyStatus->setText(tr(" Ready "));
}

void MainWindow::on_OpenImage_action_triggered()
{
    QString ImageName = QFileDialog::getOpenFileName(nullptr, tr("Open file"), WorkingDir,
                                                     tr("All graphic formats (*.bmp *.png *.jpg);;BMP (*.bmp);;PNG (*.png);;JPEG (*.jpg)"));
    if (ImageName != "")
    {
        LineItem = nullptr;
        RectItem = nullptr;
        MainImage = nullptr;
        clearFillBlackWhitePreview();
        Scene->clear();
        MainImage = Scene->addPixmap(QPixmap(ImageName));
        MainImage->setZValue(0.0);
        Scene->setSceneRect(MainImage->pixmap().rect());
        WorkingDir = QDir (ImageName).absolutePath();
        if (FillBlackWhiteWindow && FillBlackWhiteWindow->isVisible()) {
            refreshFillBlackWhitePreview();
        } else {
            clearFillBlackWhitePreview();
        }
    }
}

void MainWindow::MouseButtonMove()
{
    if (!MainImage) {
        return;
    }

    if (ui->Line_action->isChecked()) {
        //
        LineItem->setVisible(false);
        LineItem->setLine(Scene->getTopLeft().x(), Scene->getTopLeft().y(), Scene->getBottomRight().x(), Scene->getBottomRight().y());
        LineItem->setVisible(true);
    } else if (ui->Area_action->isChecked()) {
        //
        RectItem->setVisible(false);
        RectItem->setRect(QRectF(Scene->getTopLeft(), Scene->getBottomRight()));
        RectItem->setVisible(true);
    } else if (ui->CalibrMode_action->isChecked()) {
        //
        LineItem->setVisible(false);
        LineItem->setLine(Scene->getTopLeft().x(), Scene->getTopLeft().y(), Scene->getBottomRight().x(), Scene->getTopLeft().y());
        LineItem->setVisible(true);
    } else if (ui->Microhardness_action->isChecked()) {
        //
        LineItem->setVisible(false);
        LineItem->setLine(Scene->getTopLeft().x(), Scene->getTopLeft().y(), Scene->getBottomRight().x(), Scene->getBottomRight().y());
        LineItem->setVisible(true);
    }
}

void MainWindow::on_SetScale_action_triggered()
{
    if (isSetScaleSegment)
    {
        SetScaleDialog *SetScaleWindow = new SetScaleDialog ();
        SetScaleWindow->setPixels(ScaleInPixels);
        if (SetScaleWindow->exec()==QDialog::Accepted)
        {
            isSetScale = true;
            ui->SetMkm_action->setEnabled(true);
            Scale = SetScaleWindow->getScale();
            statusBar()->showMessage(tr(" Scale has been set "), 2000);
        }
        delete SetScaleWindow;
        isSetScaleSegment = false;
    }
    else QMessageBox::warning(ui->centralWidget, tr("Attention!"),
                                      tr("Calibration segment is not set. It must be set before setting scale."),
                                      QMessageBox::Ok);
}

void MainWindow::on_Make_bigger_action_triggered()
{
    ui->ImageView->scale(1.1, 1.1);
}

void MainWindow::on_Make_Smaller_action_triggered()
{
    ui->ImageView->scale(1/1.1, 1/1.1);
}

void MainWindow::on_ClearMarkers_action_triggered()
{
//    Scene->items().clear();
//    ui->ImageView->
}

void MainWindow::on_FillBlackWhite_action_triggered()
{
    if (!MainImage) {
        QMessageBox::warning(ui->centralWidget, tr("Attention!"),
                             tr("Image is not loaded."),
                             QMessageBox::Ok);
        return;
    }

    if (FillBlackWhiteWindow && FillBlackWhiteWindow->isVisible()) {
        FillBlackWhiteWindow->raise();
        FillBlackWhiteWindow->activateWindow();
        refreshFillBlackWhitePreview();
        return;
    }

    if (!FillBlackWhiteWindow) {
        FillBlackWhiteWindow = new FillBlackWhiteDialog(this);
        FillBlackWhiteWindow->setThresholds(FillWhiteThreshold, FillBlackThreshold);

        connect(FillBlackWhiteWindow, &FillBlackWhiteDialog::thresholdsChanged, this, [this]() {
            if (!FillBlackWhiteWindow || !FillBlackWhiteWindow->isVisible()) {
                return;
            }
            FillWhiteThreshold = FillBlackWhiteWindow->whiteThreshold();
            FillBlackThreshold = FillBlackWhiteWindow->blackThreshold();
            refreshFillBlackWhitePreview();
        });
        connect(FillBlackWhiteWindow, &FillBlackWhiteDialog::applyClicked, this, [this]() {
            commitFillBlackWhiteChanges();
            if (FillBlackWhiteWindow) {
                FillBlackWhiteWindow->resetThresholdsToDefault();
                FillWhiteThreshold = 255;
                FillBlackThreshold = 0;
                FillBlackWhiteWindow->hide();
            }
        });
        connect(FillBlackWhiteWindow, &FillBlackWhiteDialog::cancelClicked, this, [this]() {
            clearFillBlackWhitePreview();
            if (FillBlackWhiteWindow) {
                FillBlackWhiteWindow->resetThresholdsToDefault();
                FillWhiteThreshold = 255;
                FillBlackThreshold = 0;
                FillBlackWhiteWindow->hide();
            }
        });
        connect(FillBlackWhiteWindow, &QObject::destroyed, this, [this]() {
            FillBlackWhiteWindow = nullptr;
        });

        if (!FillBlackWhiteWindow->property("initialPositionSet").toBool()) {
            FillBlackWhiteWindow->adjustSize();
            const QRect mainFrame = frameGeometry();
            const QSize dialogSize = FillBlackWhiteWindow->size();
            const int marginX = 16;
            const int marginY = 16;
            const QPoint targetPos(
                mainFrame.right() - dialogSize.width() - marginX + 1,
                mainFrame.top() + marginY
            );
            FillBlackWhiteWindow->move(targetPos);
            FillBlackWhiteWindow->setProperty("initialPositionSet", true);
        }
    }

    FillBlackWhiteWindow->setThresholds(FillWhiteThreshold, FillBlackThreshold);
    FillBlackWhiteWindow->show();
    FillBlackWhiteWindow->raise();
    FillBlackWhiteWindow->activateWindow();
    refreshFillBlackWhitePreview();
}

void MainWindow::on_Set_Force_action_triggered()
{
    SetForceDialog *SetForceWindow = new SetForceDialog ();
    if (SetForceWindow->exec()==QDialog::Accepted)
    {
        Force = SetForceWindow->getForce();
        isForceSet = true;
        //statusBar()->showMessage(cScaleIsSet, 2000);
    }
    delete SetForceWindow;
}

void MainWindow::on_About_action_triggered()
{
    QMessageBox::about(this, tr("About"),
                tr("<h2>%1</h2>"
                    "<p>v. %2"
                    "<p>Author: Efremenko A.V."
                    "<p>2012-2026"
                    "<p>AreaPixAnalizer program is designed for quantitative analysis of microstructure photos.")
                    .arg(QApplication::applicationName(), QApplication::applicationVersion()));
}

void MainWindow::on_EnglishLang_action_triggered()
{
    switchLanguage("en");
}

void MainWindow::on_RussianLang_action_triggered()
{
    switchLanguage("ru");
}

void MainWindow::on_UkrainianLang_action_triggered()
{
    switchLanguage("uk");
}

void MainWindow::on_Update_action_triggered()
{
    checkForUpdates(true);
}

void MainWindow::switchLanguage(const QString &lang)
{
    qApp->removeTranslator(translator);
    qApp->removeTranslator(translatorQt);

    if (lang == "en") {
        if (translator->load(":/lang/en.qm")) qApp->installTranslator(translator);
    } else if (lang == "ru") {
        if (translator->load(":/lang/ru.qm")) qApp->installTranslator(translator);
    } else if (lang == "uk") {
        if (translator->load(":/lang/uk.qm")) qApp->installTranslator(translator);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == MainChartView->viewport()) {
        if (event->type() == QEvent::MouseMove) {
            auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
            updatePlotCursorOverlay(mouseEvent->position());
            return false;
        }
        if (event->type() == QEvent::Leave) {
            hidePlotCursorOverlay();
            return false;
        }
        if (event->type() == QEvent::MouseButtonPress) {
            auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
            const QPointF valuePos = MainChart->mapToValue(mouseEvent->position(), PlotCurve);
            PlotSelectPoint(valuePos);
            return false;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::updateXAxisTitle()
{
    if (ui->SetPix_action->isChecked() || ui->SetPercent_action->isChecked()) {
        AxisX->setTitleText(tr("px"));
    } else {
        AxisX->setTitleText(tr("um"));
    }
}
void MainWindow::updatePlotCursorOverlay(const QPointF &viewPos)
{
    const QRectF plotArea = MainChart->plotArea();
    const QPointF scenePos = MainChartView->mapToScene(viewPos.toPoint());

    if (!plotArea.contains(scenePos)) {
        hidePlotCursorOverlay();
        return;
    }

    const QPointF valuePos = MainChart->mapToValue(scenePos, PlotCurve);
    const qreal xValue = qMax(AxisX->min(), qMin(AxisX->max(), valuePos.x()));
    const qreal yValue = qMax(AxisY->min(), qMin(AxisY->max(), valuePos.y()));
    PlotCursorText->setText(QString::number(xValue, 'f', 4) + ", " + QString::number(yValue, 'f', 4));

    QPointF textPos = scenePos + QPointF(10.0, -10.0);
    const QRectF textRect = PlotCursorText->boundingRect();
    if (textPos.x() + textRect.width() > plotArea.right()) {
        textPos.setX(scenePos.x() - textRect.width() - 10.0);
    }
    if (textPos.y() < plotArea.top()) {
        textPos.setY(scenePos.y() + 10.0);
    }
    PlotCursorText->setPos(textPos);
    PlotCursorText->setVisible(true);
}

void MainWindow::hidePlotCursorOverlay()
{
    PlotCursorText->setVisible(false);
}

void MainWindow::clearFillBlackWhitePreview()
{
    if (FillMaskItem) {
        Scene->removeItem(FillMaskItem);
        delete FillMaskItem;
        FillMaskItem = nullptr;
    }
}

void MainWindow::refreshFillBlackWhitePreview()
{
    if (!MainImage) {
        return;
    }

    const QImage sourceImage = MainImage->pixmap().toImage();
    const int imageWidth = sourceImage.width();
    const int imageHeight = sourceImage.height();
    QImage maskImage(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
    maskImage.fill(Qt::transparent);

    Progress->reset();
    Progress->setMinimum(0);
    Progress->setMaximum(imageHeight);
    ReadyStatus->setText(tr(" Wait "));

    for (int y = 0; y < imageHeight; ++y) {
        QRgb *maskLine = reinterpret_cast<QRgb *>(maskImage.scanLine(y));
        for (int x = 0; x < imageWidth; ++x) {
            const int gray = qGray(sourceImage.pixel(x, y));
            if (gray >= FillWhiteThreshold) {
                maskLine[x] = QColor(255, 0, 0, 255).rgba();
            } else if (gray <= FillBlackThreshold) {
                maskLine[x] = QColor(0, 255, 0, 255).rgba();
            }
        }

        Progress->setValue(y + 1);
        QCoreApplication::processEvents();
    }

    if (!FillMaskItem) {
        FillMaskItem = Scene->addPixmap(QPixmap::fromImage(maskImage));
        FillMaskItem->setZValue(1000.0);
        FillMaskItem->setAcceptedMouseButtons(Qt::NoButton);
    } else {
        FillMaskItem->setPixmap(QPixmap::fromImage(maskImage));
    }

    ReadyStatus->setText(tr(" Ready "));
}

void MainWindow::commitFillBlackWhiteChanges()
{
    if (!MainImage) {
        return;
    }

    const QImage sourceImage = MainImage->pixmap().toImage();
    const int imageWidth = sourceImage.width();
    const int imageHeight = sourceImage.height();
    QImage updatedImage = sourceImage;

    Progress->reset();
    Progress->setMinimum(0);
    Progress->setMaximum(imageHeight);
    ReadyStatus->setText(tr(" Wait "));

    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            const int gray = qGray(sourceImage.pixel(x, y));
            if (gray >= FillWhiteThreshold) {
                updatedImage.setPixelColor(x, y, QColor(255, 255, 255));
            } else if (gray <= FillBlackThreshold) {
                updatedImage.setPixelColor(x, y, QColor(0, 0, 0));
            }
        }

        Progress->setValue(y + 1);
        QCoreApplication::processEvents();
    }

    MainImage->setPixmap(QPixmap::fromImage(updatedImage));
    clearFillBlackWhitePreview();
    Progress->setValue(imageHeight);
    ReadyStatus->setText(tr(" Ready "));
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateStatusBarLabels();
        updateXAxisTitle();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::updateStatusBarLabels()
{
    ReadyStatus->setText(tr(" Ready "));
    LineLength->setText(tr(" Line length: "));
    NormalAreaPix->setText(tr(" Average intensity: "));
    UpDownPix->setText(tr(" Matrix / Carbides: "));
    Microhardness->setText(tr(" Microhardness: "));
    BlackWhiteResult->setText(tr(" Area: "));

    switch (currentUpdateMsgType) {
        case UpdateNone:
            UpdateStatus->setText("");
            break;
        case UpdateChecking:
            UpdateStatus->setText(tr(" Checking updates... "));
            break;
        case UpdateAvailable:
            UpdateStatus->setText(tr(" Program update available "));
            break;
        case UpdateFailed:
            UpdateStatus->setText(tr(" Update check failed "));
            break;
        case UpdateUpToDate:
            UpdateStatus->setText(tr(" No updates "));
            break;
    }
}

void MainWindow::checkForUpdates(bool userInitiated)
{
    if (updateCheckInProgress) {
        if (userInitiated) {
            QMessageBox::information(this, tr("Update"), tr("Update check is already in progress."));
        }
        return;
    }
    updateCheckInProgress = true;
    if (userInitiated) {
        currentUpdateMsgType = UpdateChecking;
        UpdateStatus->setText(tr(" Checking updates... "));
        UpdateStatus->show();
    }

    QNetworkRequest request{QUrl(QString::fromLatin1(kReleaseApiUrl))};
    request.setRawHeader("User-Agent", "AreaPixAnalizerQt");
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, userInitiated]() {
        updateCheckInProgress = false;
        const QByteArray body = reply->readAll();
        const bool hasError = reply->error() != QNetworkReply::NoError;
        reply->deleteLater();

        if (hasError) {
            currentUpdateMsgType = UpdateFailed;
            UpdateStatus->setText(tr(" Update check failed "));
            UpdateStatus->show();
            if (userInitiated) {
                QMessageBox::warning(this, tr("Update"), tr("Failed to check updates."));
            }
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(body);
        if (!doc.isObject()) {
            currentUpdateMsgType = UpdateFailed;
            UpdateStatus->hide();
            if (userInitiated) {
                QMessageBox::warning(this, tr("Update"), tr("Invalid response from GitHub."));
            }
            return;
        }

        const QJsonObject root = doc.object();
        const QString remoteVersion = normalizedVersionString(root.value("tag_name").toString());
        const QJsonArray assets = root.value("assets").toArray();
        QString downloadUrl;
        for (const QJsonValue &value : assets) {
            const QJsonObject asset = value.toObject();
            const QString name = asset.value("name").toString();
            if (name.endsWith(".zip", Qt::CaseInsensitive)) {
                downloadUrl = asset.value("browser_download_url").toString();
                break;
            }
        }

        latestVersion = remoteVersion;
        latestDownloadUrl = downloadUrl;

        if (!downloadUrl.isEmpty() && isRemoteVersionNewer(remoteVersion)) {
            currentUpdateMsgType = UpdateAvailable;
            UpdateStatus->setText(tr(" Program update available "));
            UpdateStatus->show();
            if (userInitiated) {
                QMessageBox msg(this);
                msg.setWindowTitle(tr("Update"));
                msg.setIcon(QMessageBox::Information);
                msg.setText(tr("Version %1 is available").arg(remoteVersion));
                QPushButton *updateButton = msg.addButton(tr("Update"), QMessageBox::AcceptRole);
                msg.addButton(tr("Cancel"), QMessageBox::RejectRole);
                msg.exec();

                if (msg.clickedButton() == updateButton && prepareAndScheduleUpdate(downloadUrl)) {
                    const auto answer = QMessageBox::question(this, tr("Update"),
                                                               tr("Update is ready. Restart now?"),
                                                               QMessageBox::Ok | QMessageBox::Cancel,
                                                               QMessageBox::Ok);
                    if (answer == QMessageBox::Ok) {
                        const QString appPath = QCoreApplication::applicationFilePath();
                        const QFileInfo appInfo(appPath);
                        const QString oldExe = appInfo.absolutePath() + "/" + appInfo.completeBaseName() + "_old." + appInfo.suffix();

                        if (QFile::exists(oldExe)) {
                            QFile::remove(oldExe);
                        }

                        if (QFile::rename(appPath, oldExe)) {
                            if (QFile::copy(pendingUpdateExePath, appPath)) {
                                QFile::remove(pendingUpdateExePath);
                                QProcess::startDetached(appPath);
                                qApp->quit();
                            } else {
                                // Rollback rename
                                QFile::rename(oldExe, appPath);
                                QMessageBox::critical(this, tr("Update"), tr("Failed to apply update."));
                            }
                        } else {
                            QMessageBox::critical(this, tr("Update"), tr("Failed to rename current executable. Close the program and try again."));
                        }
                    }
                }
            }
        } else {
            currentUpdateMsgType = UpdateUpToDate;
            UpdateStatus->hide();
            if (userInitiated) {
                QMessageBox::information(this, tr("Update"), tr("The current version is up to date."));
            }
        }
    });
}

void MainWindow::cleanupOldExecutable()
{
    const QFileInfo appInfo(QCoreApplication::applicationFilePath());
    const QString oldExe = appInfo.absolutePath() + "/" + appInfo.completeBaseName() + "_old." + appInfo.suffix();
    if (QFile::exists(oldExe)) {
        QFile::remove(oldExe);
    }
}

bool MainWindow::isRemoteVersionNewer(const QString &remoteVersion) const
{
    const QVersionNumber remote = QVersionNumber::fromString(normalizedVersionString(remoteVersion));
    const QVersionNumber current = QVersionNumber::fromString(QApplication::applicationVersion());
    return QVersionNumber::compare(remote.normalized(), current.normalized()) > 0;
}

QString MainWindow::normalizedVersionString(const QString &version) const
{
    QString v = version.trimmed();
    if (v.startsWith('v', Qt::CaseInsensitive)) {
        v.remove(0, 1);
    }
    return v;
}

bool MainWindow::prepareAndScheduleUpdate(const QString &downloadUrl)
{
    const QString appPath = QCoreApplication::applicationFilePath();
    const QFileInfo appInfo(appPath);
    const QString appDir = appInfo.absolutePath();
    const QString oldExe = appDir + "/" + appInfo.completeBaseName() + "_old." + appInfo.suffix();

    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        return false;
    }

    QNetworkRequest request{QUrl(downloadUrl)};
    request.setRawHeader("User-Agent", "AreaPixAnalizerQt");
    QNetworkReply *reply = networkManager->get(request);
    QEventLoop waitLoop;
    connect(reply, &QNetworkReply::finished, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        QMessageBox::warning(this, tr("Update"), tr("Failed to download update archive."));
        return false;
    }

    const QString zipPath = tempDir.path() + "/update.zip";
    QFile zipFile(zipPath);
    if (!zipFile.open(QIODevice::WriteOnly)) {
        reply->deleteLater();
        return false;
    }
    zipFile.write(reply->readAll());
    zipFile.close();
    reply->deleteLater();

    const QString extractDir = tempDir.path() + "/extract";
    QDir().mkpath(extractDir);
    const int unpackResult = QProcess::execute("powershell", QStringList()
                                               << "-NoProfile"
                                               << "-Command"
                                               << QString("Expand-Archive -Path '%1' -DestinationPath '%2' -Force")
                                                     .arg(QDir::toNativeSeparators(zipPath),
                                                          QDir::toNativeSeparators(extractDir)));
    if (unpackResult != 0) {
        QMessageBox::warning(this, tr("Update"), tr("Failed to unpack update archive."));
        return false;
    }

    const QString newExePath = findExtractedExePath(extractDir, appInfo.fileName());
    if (newExePath.isEmpty()) {
        QMessageBox::warning(this, tr("Update"), tr("Application exe was not found in archive."));
        return false;
    }

    QTemporaryFile stagedExe(appDir + "/AreaPixAnalizer_update_XXXXXX.exe");
    stagedExe.setAutoRemove(false);
    if (!stagedExe.open()) {
        QMessageBox::warning(this, tr("Update"), tr("Failed to prepare temporary exe file."));
        return false;
    }

    QFile newExeFile(newExePath);
    if (!newExeFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Update"), tr("Failed to read new exe file."));
        return false;
    }

    const QByteArray newExeData = newExeFile.readAll();
    if (stagedExe.write(newExeData) != newExeData.size()) {
        QMessageBox::warning(this, tr("Update"), tr("Failed to save new exe file."));
        return false;
    }
    stagedExe.close();

    pendingUpdateExePath = stagedExe.fileName();
    return true;
}

QString MainWindow::findExtractedExePath(const QString &rootDir, const QString &targetFileName) const
{
    QDirIterator exactIt(rootDir, QStringList() << "*.exe", QDir::Files, QDirIterator::Subdirectories);
    while (exactIt.hasNext()) {
        const QString file = exactIt.next();
        if (QFileInfo(file).fileName().compare(targetFileName, Qt::CaseInsensitive) == 0) {
            return file;
        }
    }

    QDirIterator anyIt(rootDir, QStringList() << "*.exe", QDir::Files, QDirIterator::Subdirectories);
    if (anyIt.hasNext()) {
        return anyIt.next();
    }
    return {};
}
