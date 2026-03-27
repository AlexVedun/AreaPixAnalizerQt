#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setscaledialog.h"
#include "setforcedialog.h"

const QString cReady = " Готово ";
const QString cWait = " Ждите ";
const QString cPix = "пикс.";
const QString cMkm = "мкм";
const QString cPercent = "%";
const QString cHV = "HV";
const QString cScaleSegment = " Длина калибровочного отрезка: ";
const QString cLineLength = " Длина отрезка: ";
const QString cScaleIsSet = " Масштаб установлен ";
const QString cNormalAreaPix = " Среднее значение интенсивности: ";
const QString cUpDownPix = " Матрица / Карбиды: ";
const QString cMicrohardness = " Микротвердость: ";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    ui->MainPlot->setAxisScale(QwtPlot::yLeft, 0, 255);
    ui->MainPlot->setAxisScale(QwtPlot::xBottom, 0, 1);
    ui->MainPlot->setAxisTitle(QwtPlot::xBottom, cPix);
    grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->setMajorPen(QPen(Qt::black,0,Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray,0,Qt::DotLine));
    grid->attach(ui->MainPlot);
    PlotCurve = new QwtPlotCurve();
    PlotCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    PlotCurve->setPen(QPen(Qt::red));
    PlotCurve->attach(ui->MainPlot);
    PlotCurveCarbides = new QwtPlotCurve();
    PlotCurveCarbides->setRenderHint(QwtPlotItem::RenderAntialiased);
    PlotCurveCarbides->setPen(QPen(Qt::darkBlue));
    PlotCurveCarbides->attach(ui->MainPlot);
    PlotPicker = new QwtPlotPicker (QwtPlot::xBottom, QwtPlot::yLeft,
                                    QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                    ui->MainPlot->canvas());
    PlotPicker->setStateMachine(new QwtPickerClickPointMachine);

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

    ui->BlackWhite_action->setVisible(false);
    ui->ClearMarkers_action->setVisible(false);

    ReadyStatus = new QLabel (cReady);
    //ReadyStatus->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(ReadyStatus);
    //ScaleSegment = new QLabel (cScaleSegment);
    //statusBar()->addWidget(ScaleSegment);
    LineLength = new QLabel (cLineLength);
    statusBar()->addWidget(LineLength);
    NormalAreaPix = new QLabel (cNormalAreaPix);
    statusBar()->addWidget(NormalAreaPix);
    UpDownPix = new QLabel (cUpDownPix);
    statusBar()->addWidget(UpDownPix);
    Microhardness = new QLabel (cMicrohardness);
    statusBar()->addWidget(Microhardness);
    Progress = new QProgressBar;
    Progress->setMaximumSize(150, statusBar()->height());
    Progress->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(Progress);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    setGeometry(screenWidth / 8, screenHeight / 8, (screenWidth * 3) / 4, (screenHeight * 3) / 4);

    QSettings Settings;
    WorkingDir = Settings.value("/Settings/WorkingDir", "").toString();

    connect (Scene, SIGNAL(MouseButRelease()), SLOT(MouseButtonRelease()));
    connect (Scene, SIGNAL(MouseButPress()), SLOT(MouseButtonPress()));
    connect (Scene, SIGNAL(MouseButMove()), SLOT(MouseButtonMove()));
    connect (PlotPicker, SIGNAL(selected(QPointF)), SLOT(PlotSelectPoint(QPointF)));
    connect (ui->AboutQt_action, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainWindow::~MainWindow()
{
    QSettings Settings;
    Settings.setValue("/Settings/WorkingDir", WorkingDir);

    delete grid;
    delete PlotCurve;
    delete PlotCurveCarbides;
    delete PlotPicker;
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
                Microhardness->setText(cMicrohardness+QString::number(HV)+" "+cHV+" ");
            }
        }
    }
    else if (ui->BlackWhite_action->isChecked())
    {
        //
//        ScaleSegment->setText(cScaleSegment+QString::number(Scene->getBottomRight().x()-Scene->getTopLeft().x())+" ");
    }
}

void MainWindow::PlotSelectPoint(QPointF pos)
{
    int CheckPoint = pos.y();
    int UpCount = 0;
    int DownCount = 0;
    QVector<double> dataCarbideX, dataCarbideY;
    ReadyStatus->setText(cWait);
    PlotCurveCarbides->show();
    for (int i = 0; i<dataX.size(); i++)
    {
        dataCarbideX.append(dataX[i]);
        if (dataY[i]>=CheckPoint)
        {
            dataCarbideY.append(CheckPoint);
            UpCount++;
        }
        else
        {
            dataCarbideY.append(0);
            DownCount++;
        }
        PlotCurveCarbides->setSamples(dataCarbideX, dataCarbideY);
        ui->MainPlot->replot();
    }
//    if (isSetScale) UpDownPix->setText(cUpDownPix+cMkm+" "+QString::number(UpCount * Scale)+" / "+
//                                                           QString::number(DownCount * Scale));
//    else UpDownPix->setText(cUpDownPix+cPix+" "+QString::number(UpCount)+" / "+QString::number(DownCount));
    if (ui->SetPix_action->isChecked()) UpDownPix->setText(cUpDownPix+cPix+" "+QString::number(UpCount)+
                                                           " / "+QString::number(DownCount)+" ");
    else if (ui->SetMkm_action->isChecked()) UpDownPix->setText(cUpDownPix+cMkm+" "+QString::number(UpCount * Scale)+
                                                                " / "+QString::number(DownCount * Scale)+" ");
    else UpDownPix->setText(cUpDownPix+cPercent+" "+QString::number((int)(UpCount*100/(UpCount+DownCount)))+
                            " / "+QString::number((int)(DownCount*100/(UpCount+DownCount)))+" ");
    ReadyStatus->setText(cReady);
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
    ui->MainPlot->setAxisScale(QwtPlot::xBottom, 0, count*Scale);
//    if (isSetScale) ui->MainPlot->setAxisTitle(QwtPlot::xBottom, cMkm);
    if (ui->SetPix_action->isChecked() || ui->SetPercent_action->isChecked())
        ui->MainPlot->setAxisTitle(QwtPlot::xBottom, cPix);
    else ui->MainPlot->setAxisTitle(QwtPlot::xBottom, cMkm);
    QImage TempImage = MainImage->pixmap().toImage();
    int i = 0;

    Progress->reset();
    Progress->setMinimum(0);
    Progress->setMaximum(count);
    dataX.clear();
    dataY.clear();
    PlotCurveCarbides->hide();
    ReadyStatus->setText(cWait);
    for (;;)
    {
        dataX.append(i*Scale);
        dataY.append(255-qGray (TempImage.pixel(x1, y1)));
        PlotCurve->setSamples(dataX, dataY);
        ui->MainPlot->replot();
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
    Progress->setValue(count);
    if (ui->SetMkm_action->isChecked()) LineLength->setText(cLineLength+" "+QString::number(count*Scale)+" "+cMkm);
    else LineLength->setText(cLineLength+" "+QString::number(count)+" "+cPix);
    ReadyStatus->setText(cReady);
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
    ReadyStatus->setText(cWait);
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
    NormalAreaPix->setText(cNormalAreaPix+QString::number(pix/count)+" ");
    ReadyStatus->setText(cReady);
}

void MainWindow::BlackWhite(QPointF point)
{
    //
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
            statusBar()->showMessage(cScaleIsSet, 2000);
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
                   "количественного анализа фотографий микроструктуры. "
                   "Программа написана с использованием библиотек Qt и "
                   "Qwt (qwt.sourceforge.net)"));
}
