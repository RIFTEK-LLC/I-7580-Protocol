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
    // Обработка пакета от PLC
    void GetDataFromSystem(char*, unsigned int);

private slots:
    void on_pushButton_clicked();
    void Print(QString s);



    void on_pushButton_sendPacket_clicked();

private:
    Ui::MainWindow *ui;

    LedIndicator* m_calibration_running;
    LedIndicator* m_calibration_completed;
    LedIndicator* m_calibration_error;
    LedIndicator* m_zero_positioning_running;
    LedIndicator* m_zero_positioning_completed;
    LedIndicator* m_zero_positioning_error;
    LedIndicator* m_measuring_running;
    LedIndicator* m_measuring_error;
    LedIndicator* m_measuring_completed;
    LedIndicator* m_measuring_aborted;
    LedIndicator* m_system_in_fault_condition;
    LedIndicator* m_save_measurements_running;
    LedIndicator* m_save_measurements_completed;
    LedIndicator* m_save_measurements_error;
    LedIndicator* m_data_transfer_read;

    // Воркер работы с мотором
    CamsoWorker* m_CamsoWorker = nullptr;
    QThread* m_CamsorThread = nullptr;

    bool m_IsWork;
};
#endif // MAINWINDOW_H
