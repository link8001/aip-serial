#ifndef CWINSERIAL_H
#define CWINSERIAL_H

#include <QTimer>
#include <QDebug>
#include <QWidget>
#include <QCloseEvent>
#include <QProcess>
#include <QSettings>
#include <QStringList>
#include <QDataStream>
#include <QButtonGroup>

#ifndef WINDOWS
#include "posix_qextserialport.h"
#else
#include "win_qextserialport.h"
#endif

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
    void ComInit(void);
    void ComRead(void);
    void ComWrite(void);
    void KeyJudge(int id);
    void SettingSave(void);
    void SendMsg(QByteArray msg);
    void SendError(QByteArray msg);
    void closeEvent(QCloseEvent *e);

private:
#ifndef WINDOWS
    Posix_QextSerialPort *com;
#else
    Win_QextSerialPort *com;
#endif
    QButtonGroup *btnGroup;
    QSettings *setting;
    QThread *serial;
    QTimer *timer;
    quint8 temp;
};

#endif // CWINSERIAL_H
