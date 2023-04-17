#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "ledindicator.h"

#include "camsoworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Инициализация потока управления протоколом обмена
    void InitCamsoWorker();

public slots:
    // Обработка команды Calibration start
    void CalibrationStart();
    // Обработка команды Zero positioning start
    void ZeroPositioningStart();
    // Обработка команды Measuring start
    void MeasuringStart();
    // Обработка команды Abort measure
    void AbortMeasure();
    // Обработка команды Fault reset
    void FaultReset();
    // Обработка команды Save measurements
    void SaveMeasurements();
    // Обработка команды New data transfer
    void NewDataTransfer(float scan_length, float calibration_diameter);
    // Обработка пакета от PLC
    void GetDataFromPLC(char*, unsigned int);

    void Print(QString s);

private slots:
    void on_pushButton_clicked();



    void on_pushButton_sendPacket_clicked();

private:
    Ui::MainWindow *ui;

    LedIndicator* m_CalibrationStart;
    LedIndicator* m_ZeroPositioningStart;
    LedIndicator* m_MeasuringStart;
    LedIndicator* m_AbortMeasure;
    LedIndicator* m_FaultReset;
    LedIndicator* m_SaveMeasurements;
    LedIndicator* m_NewDataTransfer;

    // Воркер работы с мотором
    CamsoWorker* m_CamsoWorker = nullptr;
    QThread* m_CamsorThread = nullptr;

    bool m_IsWork;
};
#endif // MAINWINDOW_H
