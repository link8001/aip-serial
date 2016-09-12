/*******************************************************************************
 * Copyright (c) 2016,青岛艾普智能仪器有限公司
 * All rights reserved.
 *
 * version:     1.0
 * author:      link
 * date:        2016.08.18
 * brief:       手主界面
*******************************************************************************/
#include "CWinSerial.h"
#include "ui_CWinSerial.h"
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      构造函数
******************************************************************************/
CWinSerial::CWinSerial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWinSerial)
{
    ui->setupUi(this);
    WinInit();
    KeyInit();
    DatInit();
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(ComRead()));
    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(ComWrite()));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      析构函数
******************************************************************************/
CWinSerial::~CWinSerial()
{
    delete ui;
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      界面初始化
******************************************************************************/
void CWinSerial::WinInit()
{
    QStringList com;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        com << info.portName();
    }
    ui->Box1->addItems(com);
    QStringList bit;
    bit <<"2400"<<"4800"<<"9600"<<"19200"<<"38400"<<"57600"<<"115200";
    ui->Box2->addItems(bit);
    QStringList par;
    par <<"None"<<"Odd"<<"Even";
    ui->Box3->addItems(par);
    QStringList dat;
    dat <<"8"<<"7"<<"6";
    ui->Box4->addItems(dat);
    QStringList stop;
    stop<<"1"<<"1.5"<<"2";
    ui->Box5->addItems(stop);

    QFile file;
    QString qss;
    file.setFileName(":/css/aip001.css");
    file.open(QFile::ReadOnly);
    qss = QLatin1String(file.readAll());
    this->setStyleSheet(qss);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      按键初始化
******************************************************************************/
void CWinSerial::KeyInit()
{
    btnGroup = new QButtonGroup;
    btnGroup->addButton(ui->KeyOpen,Qt::Key_A);
    btnGroup->addButton(ui->KeyClear,Qt::Key_B);
    btnGroup->addButton(ui->KeyExit,Qt::Key_C);
    btnGroup->addButton(ui->KeySend,Qt::Key_E);
    connect(btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(KeyJudge(int)));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      按键功能
******************************************************************************/
void CWinSerial::KeyJudge(int id)
{
    switch (id) {
    case Qt::Key_A:
        if (ui->KeyOpen->text().contains("打开串口")) {
            ComInit();
        } else {
            com->close();
            ui->KeyOpen->setText("打开串口");
        }
        break;
    case Qt::Key_B:
        ui->textEdit->clear();
        break;
    case Qt::Key_C:
        this->close();
        break;
    case Qt::Key_E:
        ComWrite();
        break;
    default:
        break;
    }
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      读取设置
******************************************************************************/
void CWinSerial::DatInit()
{
    setting = new QSettings("default",QSettings::IniFormat);
    ui->Box1->setCurrentText(setting->value("/Default/COM").toString());
    ui->Box2->setCurrentIndex(setting->value("/Default/BIT").toInt());
    ui->Box3->setCurrentIndex(setting->value("/Default/PAR").toInt());
    ui->Box4->setCurrentIndex(setting->value("/Default/DAT").toInt());
    ui->Box5->setCurrentIndex(setting->value("/Default/STP").toInt());
    ui->CheckHex->setChecked(setting->value("/Default/HEX").toBool());
    ui->CheckAuto->setChecked(setting->value("/Default/AUTO").toBool());
    ui->CheckHexSend->setChecked(setting->value("/Default/SEND").toBool());
    ui->EditPeriod->setText(setting->value("/Default/PERIOD").toString());
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      保存设置
******************************************************************************/
void CWinSerial::DatSave()
{
    setting->setValue("/Default/COM",ui->Box1->currentText());
    setting->setValue("/Default/BIT",ui->Box2->currentIndex());
    setting->setValue("/Default/PAR",ui->Box3->currentIndex());
    setting->setValue("/Default/DAT",ui->Box4->currentIndex());
    setting->setValue("/Default/STP",ui->Box5->currentIndex());
    setting->setValue("/Default/HEX",ui->CheckHex->isChecked());
    setting->setValue("/Default/AUTO",ui->CheckAuto->isChecked());
    setting->setValue("/Default/SEND",ui->CheckHexSend->isChecked());
    setting->setValue("/Default/PERIOD",ui->EditPeriod->text());
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      打开串口
******************************************************************************/
void CWinSerial::ComInit()
{ 
    com = new QSerialPort(ui->Box1->currentText(),this);
    if (com->open(QIODevice::ReadWrite)) {
        com->setBaudRate(ui->Box2->currentText().toInt());    //波特率
        if (ui->Box3->currentText() == "None")
            com->setParity(QSerialPort::NoParity);
        if (ui->Box3->currentText() == "Odd")
            com->setParity(QSerialPort::OddParity);
        if (ui->Box3->currentText() == "Even")
            com->setParity(QSerialPort::EvenParity);

        if (ui->Box4->currentText() == "8")
            com->setDataBits(QSerialPort::Data8);
        if (ui->Box4->currentText() == "7")
            com->setDataBits(QSerialPort::Data7);
        if (ui->Box4->currentText() == "6")
            com->setDataBits(QSerialPort::Data6);

        if (ui->Box5->currentText() == "1")
            com->setStopBits(QSerialPort::OneStop);
        if (ui->Box5->currentText() == "1.5")
            com->setStopBits(QSerialPort::OneAndHalfStop);
        if (ui->Box5->currentText() == "2")
            com->setStopBits(QSerialPort::TwoStop);

        com->setFlowControl(QSerialPort::NoFlowControl);
        com->setDataTerminalReady(true);
        com->setRequestToSend(false);
        ui->KeyOpen->setText("关闭串口");

        timer1->start(50);
        return;
    }
    SendError("串口打开失败\n");
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      读取串口数据
******************************************************************************/
void CWinSerial::ComRead()
{
    if (com->bytesAvailable()>0) {
        emit SendMsg(com->readAll());
    }
    if (ui->CheckAuto->isChecked() && !timer2->isActive())
        timer2->start(ui->EditPeriod->text().toInt());
    if (!ui->CheckAuto->isChecked())
        timer2->stop();
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      发送数据
******************************************************************************/
void CWinSerial::ComWrite()
{
    if (ui->CheckHexSend->isChecked())
        com->write(QByteArray::fromHex(ui->EditSend->toPlainText().toUtf8()));
    else
        com->write(ui->EditSend->toPlainText().toUtf8());
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      显示数据
******************************************************************************/
void CWinSerial::SendMsg(QByteArray msg)
{
    if (ui->CheckHex->isChecked())
        ui->textEdit->insertPlainText(msg.toHex());
    else
        ui->textEdit->insertPlainText(msg);
    ui->textEdit->moveCursor(QTextCursor::End);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      显示错误
******************************************************************************/
void CWinSerial::SendError(QByteArray msg)
{
    ui->textEdit->insertPlainText(msg);
    ui->textEdit->insertPlainText("\n");
    ui->textEdit->moveCursor(QTextCursor::End);
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2016.08.18
  * brief:      退出保存
******************************************************************************/
void CWinSerial::closeEvent(QCloseEvent *e)
{
    DatSave();
    e->accept();
}
/*******************************************************************************
 *                                  END
*******************************************************************************/
