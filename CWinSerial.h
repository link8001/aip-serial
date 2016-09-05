#ifndef CWINSERIAL_H
#define CWINSERIAL_H

#include <QTimer>
#include <QDebug>
#include <QWidget>
#include <QSettings>
#include <QStringList>
#include <QCloseEvent>
#include <QButtonGroup>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class CWinSerial;
}

class CWinSerial : public QWidget
{
    Q_OBJECT

public:
    explicit CWinSerial(QWidget *parent = 0);
    ~CWinSerial();

private:
    Ui::CWinSerial *ui;

private slots:
    void WinInit(void);
    void KeyInit(void);
    void KeyJudge(int id);
    void DatInit(void);
    void DatSave(void);
    void ComInit(void);
    void ComRead(void);
    void ComWrite(void);
    void SendMsg(QByteArray msg);
    void SendError(QByteArray msg);
    void closeEvent(QCloseEvent *e);
private:
    QButtonGroup *btnGroup;
    QSettings *setting;
    QSerialPort *com;
    QTimer *timer1;
    QTimer *timer2;
};

#endif // CWINSERIAL_H
