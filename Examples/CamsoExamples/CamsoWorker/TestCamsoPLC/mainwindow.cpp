#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <endian_conv.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_IsWork(false)
{
    ui->setupUi(this);
    m_calibration_running = new LedIndicator();
    m_calibration_completed= new LedIndicator();
    m_calibration_error= new LedIndicator();
    m_zero_positioning_running= new LedIndicator();
    m_zero_positioning_completed= new LedIndicator();
    m_zero_positioning_error= new LedIndicator();
    m_measuring_running= new LedIndicator();
    m_measuring_error= new LedIndicator();
    m_measuring_completed= new LedIndicator();
    m_measuring_aborted= new LedIndicator();
    m_system_in_fault_condition= new LedIndicator();
    m_save_measurements_running= new LedIndicator();
    m_save_measurements_completed= new LedIndicator();
    m_save_measurements_error= new LedIndicator();
    m_data_transfer_read= new LedIndicator();

    ui->formLayout_5->setWidget(0,QFormLayout::FieldRole, m_calibration_running);
    ui->formLayout_5->setWidget(1,QFormLayout::FieldRole, m_calibration_completed);
    ui->formLayout_5->setWidget(2,QFormLayout::FieldRole, m_calibration_error);
    ui->formLayout_5->setWidget(3,QFormLayout::FieldRole, m_zero_positioning_running);
    ui->formLayout_5->setWidget(4,QFormLayout::FieldRole, m_zero_positioning_completed);
    ui->formLayout_5->setWidget(5,QFormLayout::FieldRole, m_zero_positioning_error);
    ui->formLayout_5->setWidget(6,QFormLayout::FieldRole, m_measuring_running);
    ui->formLayout_5->setWidget(7,QFormLayout::FieldRole, m_measuring_error);
    ui->formLayout_5->setWidget(8,QFormLayout::FieldRole, m_measuring_completed);
    ui->formLayout_5->setWidget(9,QFormLayout::FieldRole, m_measuring_aborted);
    ui->formLayout_5->setWidget(10,QFormLayout::FieldRole, m_system_in_fault_condition);
    ui->formLayout_5->setWidget(11,QFormLayout::FieldRole, m_save_measurements_running);
    ui->formLayout_5->setWidget(12,QFormLayout::FieldRole, m_save_measurements_completed);
    ui->formLayout_5->setWidget(13,QFormLayout::FieldRole, m_save_measurements_error);
    ui->formLayout_5->setWidget(14,QFormLayout::FieldRole, m_data_transfer_read);

    m_calibration_running->setState(false);
    m_calibration_completed->setState(false);
    m_calibration_error->setState(false);
    m_zero_positioning_running->setState(false);
    m_zero_positioning_completed->setState(false);
    m_zero_positioning_error->setState(false);
    m_measuring_running->setState(false);
    m_measuring_error->setState(false);
    m_measuring_completed->setState(false);
    m_measuring_aborted->setState(false);
    m_system_in_fault_condition->setState(false);
    m_save_measurements_running->setState(false);
    m_save_measurements_completed->setState(false);
    m_save_measurements_error->setState(false);
    m_data_transfer_read->setState(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Print(QString s)
{
    ui->textEdit->moveCursor (QTextCursor::End);
    ui->textEdit->insertPlainText (s);
    ui->textEdit->moveCursor (QTextCursor::End);
}

void MainWindow::InitCamsoWorker()
{
    m_CamsoWorker = new CamsoWorker(ui->lineEdit_Port->text().toInt(), ui->lineEdit_Baudrate->text().toInt());
    m_CamsorThread = new QThread();
    m_CamsoWorker->moveToThread(m_CamsorThread);

    connect(m_CamsoWorker, SIGNAL(GetData(char*, unsigned int)),
            this,          SLOT(GetDataFromSystem(char*, unsigned int)));

    connect(m_CamsorThread, SIGNAL(started()), m_CamsoWorker, SLOT(ReadThreadFuncSystem()));
    connect(m_CamsoWorker,  SIGNAL(finished()), m_CamsorThread, SLOT(quit()));

    connect(m_CamsoWorker, SIGNAL(Print(QString)),
            this,          SLOT(Print(QString)));

    m_CamsorThread->start();
    m_IsWork = true;
}

void MainWindow::GetDataFromSystem(char* packet, unsigned int size)
{
    if (size >= sizeof (system_packet))
    {
        system_packet* p = (system_packet*)packet;
        m_calibration_running->setState(p->state.calibration_running);
        m_calibration_completed->setState(p->state.calibration_completed);
        m_calibration_error->setState(p->state.calibration_error);
        m_zero_positioning_running->setState(p->state.zero_positioning_running);
        m_zero_positioning_completed->setState(p->state.zero_positioning_completed);
        m_zero_positioning_error->setState(p->state.zero_positioning_error);
        m_measuring_running->setState(p->state.measuring_running);
        m_measuring_error->setState(p->state.measuring_error);
        m_measuring_completed->setState(p->state.measuring_completed);
        m_measuring_aborted->setState(p->state.measuring_aborted);
        m_system_in_fault_condition->setState(p->state.system_in_fault_condition);
        m_save_measurements_running->setState(p->state.save_measurements_running);
        m_save_measurements_completed->setState(p->state.save_measurements_completed);
        m_save_measurements_error->setState(p->state.save_measurements_error);
        m_data_transfer_read->setState(p->state.data_transfer_read);

        uint8_t* pointer = (uint8_t*)&p->result.diameter_min;
        uint32_t value = get_rfUint32_from_packet(&pointer, kEndianessBig);
        float v = *(float*)(uint32_t*)&value;
        ui->lineEdit_diameter_min->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_diameter_max->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_diameter_avg->setText(QString("%1").arg(v, 3, 'f', 4));

        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_chamfer_width_min->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_chamfer_width_max->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_chamfer_width_avg->setText(QString("%1").arg(v, 3, 'f', 4));


        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_circularity_min->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_circularity_max->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_circularity_avg->setText(QString("%1").arg(v, 3, 'f', 4));

        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_bevel_angle_min->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_bevel_angle_max->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_bevel_angle_avg->setText(QString("%1").arg(p->result.bevel_angle_avg, 3, 'f', 4));

        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_band_height_min->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_band_height_max->setText(QString("%1").arg(v, 3, 'f', 4));
        value = get_rfUint32_from_packet(&pointer, kEndianessBig); v = *(float*)(uint32_t*)&value;
        ui->lineEdit_band_height_avg->setText(QString("%1").arg(v, 3, 'f', 4));

    }
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

void MainWindow::on_pushButton_sendPacket_clicked()
{
    plc_packet plc;
    plc.calibration_start = ui->checkBox_Calibration_start->checkState();
    plc.zero_positioning_start = ui->checkBox_Zero_positioning_start->checkState();
    plc.measuring_start = ui->checkBox_Measuring_start->checkState();
    plc.abort_measure = ui->checkBox_Abort_measure->checkState();
    plc.fault_reset = ui->checkBox_Fault_reset->checkState();
    plc.save_measurements = ui->checkBox_Save_Measurements->checkState();
    plc.new_data_transfer = ui->checkBox_NewData->checkState();

    plc.calibration_diameter = ui->lineEdit_Calibration_diameter->text().toFloat();
    plc.scan_length = ui->lineEdit_Scan_length->text().toFloat();

    m_CamsoWorker->SendData(plc);
}
