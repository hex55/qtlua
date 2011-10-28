#include "serialmonitor.h"
#include "serialhelper.h"

SerialMonitor::SerialMonitor(const QString& portName, QWidget *parent) :
    QFrame(parent),
    mPortName(portName)
{
    setupUi(this);

    settingMenu = 0;

    serialPort = new QextSerialPort(portName, QextSerialPort::EventDriven);

    connect(btnConnect, SIGNAL(clicked()), this, SLOT(openDevice()));

    connect(serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    connect(btnSendData, SIGNAL(clicked()), this, SLOT(wirteData()));

    settingMapper = new QSignalMapper();

    connect(settingMapper, SIGNAL(mapped(QString)), this, SLOT(updateSetting(QString)));

    connect(btnSendFile, SIGNAL(clicked()),this, SLOT(sendFile()));

    connect(btnClearRecv, SIGNAL(clicked()), textRecv, SLOT(clear()));

    updateUI();
}

void SerialMonitor::addSettingMenuCallback(const QString& name, const QString& setting, bool isGroup, void* context)
{
    ((SerialMonitor*)context)->addSettingMenu(name,setting,isGroup);
}

void SerialMonitor::addSettingMenu(const QString& name, const QString& setting, bool isGroup)
{
    static QMenu* curMenu = 0;
    if(curMenu == 0){
        curMenu = settingMenu;
    }
    if(isGroup){
        curMenu = settingMenu->addMenu(name);
    }else{
        QAction* act = curMenu->addAction(name);
        act->setCheckable(true);
        if(isSameSetting(serialPort->portSetting(),setting)){
            act->setChecked(true);
        }
        connect(act, SIGNAL(triggered()), settingMapper, SLOT(map()));
        settingMapper->setMapping(act, setting);
    }
}

void SerialMonitor::updateUI()
{
    PortSettings setting = serialPort->portSetting();
    QString setStr = PortSettingString(setting);
    labelCfg->setText(setStr + " - flow:" + settingString(setting.FlowControl));
    QString openStr, connectBtnStr;
    if(serialPort->isOpen()){
        openStr = "Open";
        connectBtnStr = "Disconnect";
        btnSendData->setEnabled(true);
        btnSendFile->setEnabled(true);
    }else{
        openStr = "Closed";
        connectBtnStr = "Connect";
        btnSendData->setEnabled(false);
        btnSendFile->setEnabled(false);
    }
    setWindowTitle(mPortName + "(" + setStr + ")" + openStr);
    btnConnect->setText(connectBtnStr);

    settingMenu = new QMenu(this);

    tranverseSetting(addSettingMenuCallback,this);

    btnSetting->setMenu(settingMenu);
}

void SerialMonitor::updateSetting(QString setting)
{
    PortSettings s = serialPort->portSetting();
    UpdatePortSettingString(s, setting);
    serialPort->setPortSetting(s);
    updateUI();
}

void SerialMonitor::openDevice()
{
    if(serialPort->isOpen()){
        serialPort->close();
    }else{
        serialPort->open(QIODevice::ReadWrite);
    }
    updateUI();
}

void SerialMonitor::closeEvent(QCloseEvent *event)
{
    if (canbeClose()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool SerialMonitor::canbeClose()
{
    static bool closeAll = false;
    static bool canceled = false;
    if (serialPort->isOpen()) {
        if(closeAll){
            serialPort->close();
            return true;
        }
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Serial Monitor"),
                     tr("'%1' has already open.\n"
                        "Do you want to close it anyway?")
                     .arg(mPortName),
                     QMessageBox::Yes | QMessageBox::YesToAll
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes){
            serialPort->close();
        }
        else if (ret == QMessageBox::Cancel){
            canceled = true;
            return false;
        }
        else if (ret == QMessageBox::YesToAll){
            serialPort->close();
            closeAll = true;
        }
    }
    return true;
}

void SerialMonitor::onReadyRead()
{
    QByteArray bytes;
    int a = serialPort->bytesAvailable();
    bytes.resize(a);
    serialPort->read(bytes.data(), bytes.size());
    textRecv->setPlainText(textRecv->toPlainText()+QString(bytes));
}

void SerialMonitor::wirteData()
{
    if(serialPort->isOpen()){
        QString text = textSend->toPlainText();
        serialPort->write(text.toAscii());
    }
}

void SerialMonitor::sendFile()
{
    if(serialPort->isOpen()){
        QString name = QFileDialog::getOpenFileName(this);
        QFile file(name);
        file.open(QIODevice::ReadOnly);
        serialPort->write(file.readAll());
        file.close();
    }
}
