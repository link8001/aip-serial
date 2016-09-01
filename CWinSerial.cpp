#include "CWinSerial.h"
#include "ui_CWinSerial.h"

CWinSerial::CWinSerial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWinSerial)
{
    ui->setupUi(this);
    WinInit();
    KeyInit();
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ComRead()));
    temp = 0;
}

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
    QProcess *pro = new QProcess(this);
    pro->start("dmesg");
    pro->waitForFinished(3000);
    QString t = pro->readAllStandardOutput();
    QStringList tt = t.split("\n");
    for (int i=0; i<tt.size(); i++) {
        QString ttt = tt.at(i);
        if (ttt.contains("ttyS")) {
            com << ttt.mid(ttt.indexOf("ttyS"),5);
        }
        if (ttt.contains("ttyUSB")) {
            com << ttt.mid(ttt.indexOf("ttyUSB"),7);
        }
    }

//    com <<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"<<"COM7"<<"COM8";
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
  * brief:      打开串口
******************************************************************************/
void CWinSerial::ComInit()
{
    QString dev = QString("/dev/%1").arg(ui->Box1->currentText());
    com = new Posix_QextSerialPort(dev,QextSerialBase::Polling);

    if (com->open(QIODevice::ReadWrite)) {
        ui->KeyOpen->setText("关闭串口");

        if (ui->Box2->currentText() == "2400")
            com->setBaudRate(BAUD2400);
        if (ui->Box2->currentText() == "4800")
            com->setBaudRate(BAUD4800);
        if (ui->Box2->currentText() == "9600")
            com->setBaudRate(BAUD9600);
        if (ui->Box2->currentText() == "19200")
            com->setBaudRate(BAUD19200);
        if (ui->Box2->currentText() == "38400")
            com->setBaudRate(BAUD38400);
        if (ui->Box2->currentText() == "57600")
            com->setBaudRate(BAUD57600);
        if (ui->Box2->currentText() == "115200")
            com->setBaudRate(BAUD115200);

        if (ui->Box3->currentText() == "None")
            com->setParity(PAR_NONE);
        if (ui->Box3->currentText() == "Odd")
            com->setParity(PAR_ODD);
        if (ui->Box3->currentText() == "Even")
            com->setParity(PAR_EVEN);

        if (ui->Box4->currentText() == "8")
            com->setDataBits(DATA_8);
        if (ui->Box4->currentText() == "7")
            com->setDataBits(DATA_7);
        if (ui->Box4->currentText() == "6")
            com->setDataBits(DATA_6);

        if (ui->Box5->currentText() == "1")
            com->setStopBits(STOP_1);
        if (ui->Box5->currentText() == "1.5")
            com->setStopBits(STOP_1_5);
        if (ui->Box5->currentText() == "2")
            com->setStopBits(STOP_2);

        com->setFlowControl(FLOW_OFF);
        com->setTimeout(10);
        timer->start(50);
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
    temp++;
    if (temp*50>= ui->EditPeriod->text().toInt() && ui->CheckAuto->isChecked()) {
        temp = 0;
        ComWrite();
    }
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
  * brief:      保存设置
******************************************************************************/
void CWinSerial::SettingSave()
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
    ui->textEdit->insertPlainText("\n");
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
    SettingSave();
    e->accept();
}
