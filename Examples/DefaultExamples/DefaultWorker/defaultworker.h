#ifndef PROFINETWORKER_H
#define PROFINETWORKER_H

#include <QObject>

class ProfinetWorker : public QObject
{
    Q_OBJECT
public:
    ProfinetWorker(uint32_t bandwidth, int32_t serial_port, QObject *parent = 0);
    ~ProfinetWorker();

public slots:
    // Запускает чтение команд от PLC
    void ReadThreadFunc();
    // Останавливает чтение команд
    void stop();

signals:
    // Cигнал о получении команды Calibration start
    void CalibrationStart();
    // Cигнал о получении команды Zero positioning start
    void ZeroPositioningStart();
    // Cигнал о получении команды Measuring start
    void MeasuringStart();
    // Cигнал о получении команды Abort measure
    void AbortMeasure();
    // Cигнал о получении команды Fault reset
    void FaultReset();
    // Cигнал о получении команды New data transfer
    void NewDataTransfer(int measuring_start_position, int calibration_diameter);

private:
    bool isRun = false;
    uint32_t m_Bandwidth;
    uint8_t m_SerialPort;
    Q_DISABLE_COPY(ProfinetWorker)

};

#endif // PROFINETWORKER_H
