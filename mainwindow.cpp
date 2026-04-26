#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setscaledialog.h"
#include "setforcedialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
    AxisX->setTitleText(tr("пикс."));
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
    if (ui->Line_action->isChecked())
    {
        //
        LineItem = new QGraphicsLineItem ();
        Scene->addItem(LineItem);
        LineItem->setPen(TempPen);
    }
    else if (ui->Area_action->isChecked())
    {
        //
        RectItem = new QGraphicsRectItem ();
        Scene->addItem(RectItem);
        RectItem->setPen(TempPen);
    }
    else if (ui->CalibrMode_action->isChecked())
    {
        //
        LineItem = new QGraphicsLineItem ();
        Scene->addItem(LineItem);
        LineItem->setPen(TempPen);
    }
    else if (ui->Microhardness_action->isChecked())
    {
        //
        if (isForceSet)
        {
            LineItem = new QGraphicsLineItem ();
            Scene->addItem(LineItem);
            LineItem->setPen(TempPen);
        }
        else QMessageBox::warning(ui->centralWidget, tr("Внимание!"),
                                  tr("Не задано значение нагрузки. Его можно задать через меню 'Режим'."),
                                  QMessageBox::Ok);
    }
}

void MainWindow::MouseButtonRelease()
{
    if (ui->Line_action->isChecked())
    {
        //
        Line (Scene->getTopLeft(), Scene->getBottomRight());
    }
    else if (ui->Area_action->isChecked())
    {
        //
        Area (Scene->getTopLeft(), Scene->getBottomRight());
    }
    else if (ui->CalibrMode_action->isChecked())
    {
        //
        ScaleInPixels = Scene->getBottomRight().x()-Scene->getTopLeft().x();
        //ScaleSegment->setText(cScaleSegment+QString::number(ScaleInPixels)+" ");
        isSetScaleSegment = true;
    }
    else if (ui->Microhardness_action->isChecked())
    {
        //
        if (isForceSet)
        {
            int deltaX = abs(Scene->getBottomRight().x() - Scene->getTopLeft().x());
            int deltaY = abs(Scene->getBottomRight().y() - Scene->getTopLeft().y());
            int count = deltaY>deltaX ? deltaY : deltaX;
            if (isDiag1)
            {
                isDiag1 = false;
                Diag1 = count * Scale / 1000;
            }
            else
            {
                isDiag1 = true;
                Diag2 = count * Scale / 1000;
                double Diag = (Diag1 + Diag2)/2;
                double HV = 1.854 * Force / (Diag * Diag);
                Microhardness->setText(tr(" Микротвердость: ") + QString::number(HV) + " " + tr("HV") + " ");
            }
        }
    }
    else if (ui->BlackWhite_action->isChecked())
    {
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

    ReadyStatus->setText(tr(" Ждите "));
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
            tr(" Матрица / Карбиды: ") +
            tr("пикс.") + " " +
            QString::number(UpCount) + " / " +
            QString::number(DownCount) + " "
        );
    } else if (ui->SetMkm_action->isChecked()) {
        UpDownPix->setText(
            tr(" Матрица / Карбиды: ") +
            tr("мкм") + " " +
            QString::number(UpCount * Scale) + " / " +
            QString::number(DownCount * Scale) + " "
        );
    } else {
        UpDownPix->setText(
            tr(" Матрица / Карбиды: ") +
            tr("%") + " " +
            QString::number((int)(UpCount * 100 / (UpCount + DownCount))) + " / " +
            QString::number((int)(DownCount * 100 / (UpCount + DownCount))) + " "
        );
    }
    ReadyStatus->setText(tr(" Готово "));
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
//    if (isSetScale) AxisX->setTitleText(tr("мкм"));
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
    ReadyStatus->setText(tr(" Ждите "));
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
    if (ui->SetMkm_action->isChecked()) LineLength->setText(tr(" Длина отрезка: ") + " " + QString::number(count*Scale) + " " + tr("мкм"));
    else LineLength->setText(tr(" Длина отрезка: ") + " " + QString::number(count) + " " + tr("пикс."));
    ReadyStatus->setText(tr(" Готово "));
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
    ReadyStatus->setText(tr(" Ждите "));
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
    NormalAreaPix->setText(tr(" Среднее значение интенсивности: ") + QString::number(pix/count) + " ");
    ReadyStatus->setText(tr(" Готово "));
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
    ReadyStatus->setText(tr(" Ждите "));
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
    BlackWhiteResult->setText(tr(" Площадь: ") + QString::number(area) + " % ");
    ReadyStatus->setText(tr(" Готово "));
}

void MainWindow::on_OpenImage_action_triggered()
{
    QString ImageName = QFileDialog::getOpenFileName(0, tr("Открыть файл"), WorkingDir,
                                                     tr("Все графические форматы (*.bmp *.png *.jpg);;BMP (*.bmp);;PNG (*.png);;JPEG (*.jpg)"));
    if (ImageName != "")
    {
        LineItem = NULL;
        RectItem = NULL;
        MainImage = NULL;
        Scene->clear();
        MainImage = Scene->addPixmap(QPixmap(ImageName));
        Scene->setSceneRect(MainImage->pixmap().rect());
        WorkingDir = QDir (ImageName).absolutePath();
    }
}

void MainWindow::MouseButtonMove()
{
    if (ui->Line_action->isChecked())
    {
        //
        LineItem->setVisible(false);
        LineItem->setLine(Scene->getTopLeft().x(), Scene->getTopLeft().y(), Scene->getBottomRight().x(), Scene->getBottomRight().y());
        LineItem->setVisible(true);
    }
    else if (ui->Area_action->isChecked())
    {
        //
        RectItem->setVisible(false);
        RectItem->setRect(QRectF(Scene->getTopLeft(), Scene->getBottomRight()));
        RectItem->setVisible(true);
    }
    else if (ui->CalibrMode_action->isChecked())
    {
        //
        LineItem->setVisible(false);
        LineItem->setLine(Scene->getTopLeft().x(), Scene->getTopLeft().y(), Scene->getBottomRight().x(), Scene->getTopLeft().y());
        LineItem->setVisible(true);
    }
    else if (ui->Microhardness_action->isChecked())
    {
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
            statusBar()->showMessage(tr(" Масштаб установлен "), 2000);
        }
        delete SetScaleWindow;
        isSetScaleSegment = false;
    }
    else QMessageBox::warning(ui->centralWidget, tr("Внимание!"),
                                      tr("Не задан калибровочный отрезок. Его необходимо задать прежде, чем устанавливать масштаб."),
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
    QMessageBox::about(this, tr("О программе"),
                tr("<h2>AreaPixAnalizer v. 1.2.0</h2>"
                   "<p>Автор: Ефременко А.В."
                   "<p>2012-2026"
                   "<p>Программа AreaPixAnalizer предназначена для "
                   "количественного анализа фотографий микроструктуры."));
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

void MainWindow::switchLanguage(const QString &lang)
{
    qApp->removeTranslator(translator);
    qApp->removeTranslator(translatorQt);

    if (lang == "en") {
        if (translator->load(":/lang/en.qm")) qApp->installTranslator(translator);
    } else if (lang == "uk") {
        if (translator->load(":/lang/uk.qm")) qApp->installTranslator(translator);
    }
    // For Russian, we just use the default strings in the code
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
        AxisX->setTitleText(tr("пикс."));
    } else {
        AxisX->setTitleText(tr("мкм"));
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
    ReadyStatus->setText(tr(" Готово "));
    LineLength->setText(tr(" Длина отрезка: "));
    NormalAreaPix->setText(tr(" Среднее значение интенсивности: "));
    UpDownPix->setText(tr(" Матрица / Карбиды: "));
    Microhardness->setText(tr(" Микротвердость: "));
    BlackWhiteResult->setText(tr(" Площадь: "));
}
