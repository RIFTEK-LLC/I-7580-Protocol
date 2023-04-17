#ifndef CAMSOWORKER_H
#define CAMSOWORKER_H

#include <QObject>

#include <CamsoChannel.h>

typedef struct __attribute__ ((packed))
{
    struct system_state
    {
        bool calibration_running;
        bool calibration_completed;
        bool calibration_error;

        bool zero_positioning_running;
        bool zero_positioning_completed;
        bool zero_positioning_error;

        bool measuring_running;
        bool measuring_error;
        bool measuring_completed;
        bool measuring_aborted;

        bool system_in_fault_condition;

        bool save_measurements_running;
        bool save_measurements_completed;
        bool save_measurements_error;

        bool data_transfer_read;
    }state;

    struct system_result
    {
        float diameter_min;
        float diameter_max;
        float diameter_avg;

        float chamfer_width_min;
        float chamfer_width_max;
        float chamfer_width_avg;

        float circularity_min;
        float circularity_max;
        float circularity_avg;

        float bevel_angle_min;
        float bevel_angle_max;
        float bevel_angle_avg;

        float band_height_min;
        float band_height_max;
        float band_height_avg;
    }result;

}system_packet;

typedef struct __attribute__ ((packed))
{
    bool calibration_start;
    bool zero_positioning_start;
    bool measuring_start;
    bool abort_measure;
    bool fault_reset;
    bool save_measurements;
    bool new_data_transfer;
    float scan_length;
    float calibration_diameter;
}plc_packet;

class CamsoWorker : public QObject
{
    Q_OBJECT
public:
    CamsoWorker(int32_t serial_port, uint32_t baudrate, QObject *parent = 0);
    ~CamsoWorker();

public slots:
    // Установка таймаута для портов примема и передачи.
    void SetPortTimeoutMs(uint32_t);
    // Отправить данные в PLC
    bool SendData(plc_packet packet);
    bool SendData(system_packet packet);

    // Запускает чтение команд от PLC
    void ReadThreadFuncPLC();
    // Запускает чтение команд от Системы
    void ReadThreadFuncSystem();

    // Останавливает чтение команд
    void Stop();
    // Проверка статуса работы воркера
    bool IsWork();


signals:
    // Cигнал о получении команды от PLC
    void CalibrationStart();
    void ZeroPositioningStart();
    void MeasuringStart();
    void AbortMeasure();
    void FaultReset();
    void SaveMeasurements();
    void NewDataTransfer(float scan_length, float calibration_diameter);
    void GetData(char*, unsigned int);

    // Вывод лога работы воркера
    void Print(QString s);

    // Сигнал завершения работы воркера
    void finished();

private:
    bool isInit = false;
    bool isRun = false;
    uint32_t m_Baudrate;
    uint8_t m_SerialPort;
    uint32_t m_TimeoutMs;
    Q_DISABLE_COPY(CamsoWorker)

    I7580::Channel* channel;

};

#endif // CAMSOWORKER_H
