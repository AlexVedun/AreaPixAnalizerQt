#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtGui>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "qmygraphicsscene.h"

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
    ~MainWindow() override;
    bool eventFilter(QObject *watched, QEvent *event) override;

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
    void on_EnglishLang_action_triggered();
    void on_RussianLang_action_triggered();
    void on_UkrainianLang_action_triggered();

protected:
    void changeEvent(QEvent *event) override;

private:
    void switchLanguage(const QString &lang);
    void updateStatusBarLabels();
    void updateXAxisTitle();
    void updatePlotCursorOverlay(const QPointF &viewPos);
    void hidePlotCursorOverlay();

    Ui::MainWindow *ui;

    QActionGroup *ModeGroup;
    QActionGroup *MeasureUnitsGroup;
    QChartView *MainChartView;
    QChart *MainChart;
    QLineSeries *PlotCurve;
    QLineSeries *PlotCurveCarbides;
    QValueAxis *AxisX;
    QValueAxis *AxisY;
    QGraphicsSimpleTextItem *PlotCursorText;
    QGraphicsLineItem *LineItem;
    QGraphicsRectItem *RectItem;
    QLabel *ReadyStatus;
    //QLabel *ScaleSegment;
    QLabel *LineLength;
    QLabel *NormalAreaPix;
    QLabel *UpDownPix;
    QLabel *Microhardness;
    QLabel *BlackWhiteResult;
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
    QTranslator *translator;
    QTranslator *translatorQt;
    QActionGroup *LanguageGroup;
    void Line (QPointF BeginPoint, QPointF EndPoint);
    void Area (QPointF TopLeft, QPointF BottomRight);
    void BlackWhite (QPointF point);
};

#endif // MAINWINDOW_H
