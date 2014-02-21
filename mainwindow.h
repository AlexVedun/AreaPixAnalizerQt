#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "qmygraphicsscene.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QMyGraphicsScene *Scene;
    QGraphicsPixmapItem *MainImage;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void MouseButtonPress();
    void MouseButtonMove();
    void MouseButtonRelease();
    void PlotSelectPoint (QPointF pos);

    void on_OpenImage_action_triggered();

    void on_SetScale_action_triggered();

    void on_Make_bigger_action_triggered();

    void on_Make_Smaller_action_triggered();

    void on_ClearMarkers_action_triggered();

    void on_Set_Force_action_triggered();

    void on_About_action_triggered();

private:
    Ui::MainWindow *ui;

    QActionGroup *ModeGroup;
    QActionGroup *MeasureUnitsGroup;
    QwtPlotCurve *PlotCurve;
    QwtPlotCurve *PlotCurveCarbides;
    QwtPlotGrid *grid;
    QwtPlotPicker *PlotPicker;
    QGraphicsLineItem *LineItem;
    QGraphicsRectItem *RectItem;
    QLabel *ReadyStatus;
    //QLabel *ScaleSegment;
    QLabel *LineLength;
    QLabel *NormalAreaPix;
    QLabel *UpDownPix;
    QLabel *Microhardness;
    QProgressBar *Progress;
    double Scale;
    double Force;
    bool isForceSet;
    double Diag1;
    double Diag2;
    bool isDiag1;
    double ScaleInPixels;
    bool isSetScale;
    bool isSetScaleSegment;
    QVector<double> dataX, dataY;
    QString WorkingDir;
    void Line (QPointF BeginPoint, QPointF EndPoint);
    void Area (QPointF TopLeft, QPointF BottomRight);
    void BlackWhite (QPointF point);
};

#endif // MAINWINDOW_H
