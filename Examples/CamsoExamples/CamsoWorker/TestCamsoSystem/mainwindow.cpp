#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_IsWork(false)
{
    ui->setupUi(this);
    m_CalibrationStart = new LedIndicator();
    m_ZeroPositioningStart  = new LedIndicator();
    m_MeasuringStart  = new LedIndicator();
    m_AbortMeasure  = new LedIndicator();
    m_FaultReset  = new LedIndicator();
    m_SaveMeasurements  = new LedIndicator();
    m_NewDataTransfer = new LedIndicator();
    ui->formLayout_2->setWidget(0,QFormLayout::FieldRole, m_CalibrationStart);
    ui->formLayout_2->setWidget(1,QFormLayout::FieldRole, m_ZeroPositioningStart);
    ui->formLayout_2->setWidget(2,QFormLayout::FieldRole, m_MeasuringStart);
    ui->formLayout_2->setWidget(3,QFormLayout::FieldRole, m_AbortMeasure);
    ui->formLayout_2->setWidget(4,QFormLayout::FieldRole, m_FaultReset);
    ui->formLayout_2->setWidget(5,QFormLayout::FieldRole, m_SaveMeasurements);
    ui->formLayout_2->setWidget(6,QFormLayout::FieldRole, m_NewDataTransfer);

    m_CalibrationStart->setState(false);
    m_ZeroPositioningStart->setState(false);
    m_MeasuringStart->setState(false);
    m_AbortMeasure->setState(false);
    m_FaultReset->setState(false);
    m_SaveMeasurements->setState(false);
    m_NewDataTransfer->setState(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitCamsoWorker()
{
    m_CamsoWorker = new CamsoWorker(ui->lineEdit_Port->text().toInt(), ui->lineEdit_Baudrate->text().toInt());
    m_CamsorThread = new QThread();
    m_CamsoWorker->moveToThread(m_CamsorThread);

    connect(m_CamsoWorker, SIGNAL(GetData(char*, unsigned int)),
            this,          SLOT(GetDataFromPLC(char*, unsigned int)));

    connect(m_CamsoWorker, SIGNAL(CalibrationStart()),
            this,          SLOT(CalibrationStart()));

    connect(m_CamsoWorker, SIGNAL(ZeroPositioningStart()),
            this,          SLOT(ZeroPositioningStart()));

    connect(m_CamsoWorker, SIGNAL(MeasuringStart()),
            this,          SLOT(MeasuringStart()));

    connect(m_CamsoWorker, SIGNAL(AbortMeasure()),
            this,          SLOT(AbortMeasure()));

    connect(m_CamsoWorker, SIGNAL(FaultReset()),
            this,          SLOT(FaultReset()));

    connect(m_CamsoWorker, SIGNAL(SaveMeasurements()),
            this,          SLOT(SaveMeasurements()));

    connect(m_CamsoWorker, SIGNAL(NewDataTransfer(float, float)),
            this,          SLOT(NewDataTransfer(float, float)));

    connect(m_CamsoWorker, SIGNAL(Print(QString)),
            this,          SLOT(Print(QString)));

    connect(m_CamsorThread, SIGNAL(started()), m_CamsoWorker, SLOT(ReadThreadFuncPLC()));
    connect(m_CamsoWorker,  SIGNAL(finished()), m_CamsorThread, SLOT(quit()));

    m_CamsorThread->start();
    m_IsWork = true;
}

void MainWindow::CalibrationStart()
{
    m_CalibrationStart->setState(true);
}

void MainWindow::ZeroPositioningStart()
{
    m_ZeroPositioningStart->setState(true);
}

void MainWindow::MeasuringStart()
{
    m_MeasuringStart->setState(true);
}

void MainWindow::AbortMeasure()
{
    m_AbortMeasure->setState(true);
}

void MainWindow::FaultReset()
{
    m_FaultReset->setState(true);
}

void MainWindow::SaveMeasurements()
{
    m_SaveMeasurements->setState(true);
}

void MainWindow::NewDataTransfer(float scan_length, float calibration_diameter)
{
    m_NewDataTransfer->setState(true);
    ui->label_scanLength->setText("scan length: " + QString("%1").arg(scan_length, 3, 'f', 4));
    ui->label_calibDiam->setText("calid deameter: " + QString("%1").arg(calibration_diameter, 3, 'f', 4));
}

void MainWindow::GetDataFromPLC(char*, unsigned int)
{
    m_CalibrationStart->setState(false);
    m_ZeroPositioningStart->setState(false);
    m_MeasuringStart->setState(false);
    m_AbortMeasure->setState(false);
    m_FaultReset->setState(false);
    m_SaveMeasurements->setState(false);
    m_NewDataTransfer->setState(false);

    ui->label_scanLength->setText("scan length: 0");
    ui->label_calibDiam->setText("calid deameter: 0");
}


void MainWindow::on_pushButton_clicked()
{
    if (!m_IsWork)
    {
        InitCamsoWorker();
        ui->pushButton->setText("Stop");
    }
    else
    {
        if(m_CamsoWorker->IsWork())
            m_CamsoWorker->Stop();
        delete m_CamsoWorker;
        ui->pushButton->setText("Start");
        m_IsWork = false;
    }
}

void MainWindow::Print(QString s)
{
    ui->textEdit->moveCursor (QTextCursor::End);
    ui->textEdit->insertPlainText (s);
    ui->textEdit->moveCursor (QTextCursor::End);
}

void MainWindow::on_pushButton_sendPacket_clicked()
{
    system_packet pack;
    pack.state.calibration_running = ui->checkBox_calibration_running->checkState();
    pack.state.calibration_completed = ui->checkBox_calibration_completed->checkState();
    pack.state.calibration_error = ui->checkBox_calibration_error->checkState();

    pack.state.zero_positioning_running = ui->checkBox_zero_positioning_running->checkState();
    pack.state.zero_positioning_completed = ui->checkBox_zero_positioning_completed->checkState();
    pack.state.zero_positioning_error = ui->checkBox_zero_positioning_error->checkState();

    pack.state.measuring_running = ui->checkBox_measuring_running->checkState();
    pack.state.measuring_error = ui->checkBox_measuring_error->checkState();
    pack.state.measuring_completed = ui->checkBox_measuring_completed->checkState();
    pack.state.measuring_aborted = ui->checkBox_measuring_aborted->checkState();

    pack.state.system_in_fault_condition = ui->checkBox_system_in_fault_condition->checkState();

    pack.state.save_measurements_running = ui->checkBox_save_measurements_running->checkState();
    pack.state.save_measurements_completed = ui->checkBox_save_measurements_completed->checkState();
    pack.state.save_measurements_error = ui->checkBox_save_measurements_error->checkState();
    pack.state.data_transfer_read = ui->checkBox_data_transfer_read->checkState();

    pack.result.diameter_min = ui->lineEdit_diameter_min->text().toFloat();
    pack.result.diameter_max = ui->lineEdit_diameter_max->text().toFloat();
    pack.result.diameter_avg = ui->lineEdit_diameter_avg->text().toFloat();

    pack.result.chamfer_width_min = ui->lineEdit_chamfer_width_min->text().toFloat();
    pack.result.chamfer_width_max = ui->lineEdit_chamfer_width_max->text().toFloat();
    pack.result.chamfer_width_avg = ui->lineEdit_chamfer_width_avg->text().toFloat();

    pack.result.circularity_min = ui->lineEdit_circularity_min->text().toFloat();
    pack.result.circularity_max = ui->lineEdit_circularity_max->text().toFloat();
    pack.result.circularity_avg = ui->lineEdit_circularity_avg->text().toFloat();

    pack.result.bevel_angle_min = ui->lineEdit_bevel_angle_min->text().toFloat();
    pack.result.bevel_angle_max = ui->lineEdit_bevel_angle_max->text().toFloat();
    pack.result.bevel_angle_avg = ui->lineEdit_bevel_angle_avg->text().toFloat();

    pack.result.band_height_min = ui->lineEdit_band_height_min->text().toFloat();
    pack.result.band_height_max = ui->lineEdit_band_height_max->text().toFloat();
    pack.result.band_height_avg = ui->lineEdit_band_height_avg->text().toFloat();

    m_CamsoWorker->SendData(pack);
}
