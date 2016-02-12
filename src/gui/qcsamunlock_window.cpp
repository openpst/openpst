/**
* LICENSE PLACEHOLDER
*
* @file qcsamunlock_window.cpp
* @class QcSamUnlockWindow
* @package OpenPST
* @brief QCSamUnlock GUI interface class
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/

#include "qcsamunlock_window.h"

using namespace std;
using namespace OpenPST;

QcSamUnlockWindow::QcSamUnlockWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::QcSamUnlockWindow),
	port("", 115200, serial::Timeout::simpleTimeout(500)),
	efsManager(port)
{
    ui->setupUi(this);

    updatePortList();

    QObject::connect(ui->updatePortListButton, SIGNAL(clicked()), this, SLOT(updatePortList()));
    QObject::connect(ui->unlockButton, SIGNAL(clicked()), this, SLOT(connectPort()));

    QObject::connect(ui->clearLogButton, SIGNAL(clicked()), this, SLOT(clearLog()));
    QObject::connect(ui->saveLogButton, SIGNAL(clicked()), this, SLOT(saveLog()));

	// Help Action Menu
	// QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));
}

/**
* @brief QcSamUnlockWindow::~QcSamUnlockWindow
*/
QcSamUnlockWindow::~QcSamUnlockWindow()
{
    delete ui;
}


/**
* @brief QcSamUnlockWindow::updatePortList
*/
void QcSamUnlockWindow::updatePortList()
{
    std::vector<serial::PortInfo> devices = serial::list_ports();
    std::vector<serial::PortInfo>::iterator iter = devices.begin();

    ui->portListComboBox->clear();
    ui->portListComboBox->addItem("- Select a Port -", 0);

	clearLog();

    while (iter != devices.end()) {
        serial::PortInfo device = *iter++;

        QString item = device.port.c_str();
        item.append(" - ").append(device.description.c_str());
		
		// Return only Samsung DIAG ports
		QString hwId = device.hardware_id.c_str();
		if (hwId.contains("VID_04E8", Qt::CaseInsensitive)) {
			ui->portListComboBox->addItem(item, device.port.c_str());

			QString logMsg = "[+] Found ";
			//logMsg.append(device.hardware_id.c_str()).append(" on ").append(device.port.c_str());

			if (device.description.length()) {
				//logMsg.append(" - ").append(device.description.c_str());
				logMsg.append(device.description.c_str());
			}

			log(kLogTypeDebug, logMsg);
		}

		// if (ui->portListComboBox->size == 1) {
		// 	log(kLogTypeDebug, "No Samsung devices found");
		// }
    }
}

/**
* @brief QcSamUnlockWindow::connectPort
*/
void QcSamUnlockWindow::connectPort()
{
	QString selected = ui->portListComboBox->currentData().toString();

    if (selected.compare("0") == 0) {
        log(kLogTypeWarning, "[-] Select a Port First");
        return;
    }

    std::vector<serial::PortInfo> devices = serial::list_ports();
    std::vector<serial::PortInfo>::iterator iter = devices.begin();

    while (iter != devices.end()) {
        serial::PortInfo device = *iter++;
        if (selected.compare(device.port.c_str(), Qt::CaseInsensitive) == 0) {
            currentPort = device;
            break;
        }
    }

    if (!currentPort.port.length()) {
        log(kLogTypeError, "[-] Invalid Port Type");
        return;
    }

    try {
        port.setPort(currentPort.port);

        port.open();

        ui->unlockButton->setEnabled(false);
        ui->updatePortListButton->setEnabled(false);
        ui->portListComboBox->setEnabled(false);

		clearLog();

        QString connectedText = "[+] Connected to ";
        connectedText.append(currentPort.port.c_str());
        log(kLogTypeInfo, connectedText);

		unlockSim();
    } catch (serial::IOException e) {
        log(kLogTypeError, "[-] Error Connecting To Serial Port");
        log(kLogTypeError, e.getErrorNumber() == 13 ? "[-] Permission Denied. Try Running With Elevated Privledges." : e.what());
		return;
	} catch(std::exception e) {
		log(kLogTypeError, e.what());
		return;
	}
}

/**
* @brief QcSamUnlockWindow::disconnectPort
*/
void QcSamUnlockWindow::disconnectPort()
{
    if (port.isOpen()) {
        QString closeText = "[+] Disconnected from ";
        closeText.append(currentPort.port.c_str());
        log(kLogTypeInfo, closeText);

        port.close();

		ui->unlockButton->setEnabled(true);
		ui->updatePortListButton->setEnabled(true);
		ui->portListComboBox->setEnabled(true);
    }
}

/**
* @brief QcSamUnlockWindow::processItem
* Delete an NV item by deltree traversal exploit
* @param int item - The NV item number to delete
* @return bool
*/
bool QcSamUnlockWindow::processItem(int item, int sequence)
{
	ostringstream path;
	path << "/public/../nvm/num/" << item; // deltree only works in /public

	if (efsManager.mkdir("public", 0x01FF) == efsManager.kDmEfsSuccess) {
		if (efsManager.deltree(path.str(), sequence) == efsManager.kDmEfsSuccess) {
			return true;
		} else {
			return false;
		}
	}

	return false;
}

/**
* @brief QcSamUnlockWindow::unlockSim
*/
void QcSamUnlockWindow::unlockSim()
{
	if (efsManager.statfs("/", statResponse) == efsManager.kDmEfsSuccess) {

		if (!processItem(10080, ++sequence)) {
			log(kLogTypeWarning, "[-] Error removing NV item 10080");
		}

		if (!processItem(10074, ++sequence)) {
			log(kLogTypeWarning, "[-] Error removing NV item 10074");
		}

		if (!processItem(10073, ++sequence)) {
			log(kLogTypeWarning, "[-] Error removing NV item 10073");
		}
	} else {
		log(kLogTypeError, "[-] Error checking for EFS access");
		goto finish;
	}

	try {
		syncResponse = efsManager.syncNoWait("/");

		QString msg = "[+] EFS sync initiated. Received token ";
		log(kLogTypeInfo, msg.append(syncResponse.token));
	} catch (std::exception e) {
		QString msg = "[-] Error encountered initiating EFS sync: Error: ";
		log(kLogTypeError, msg.append(e.what()));
		goto finish;
	}

	try {
		sequence = 0;

		while (syncMaxRetries > syncRetries) {
			syncStatusResponse = efsManager.getSyncStatus("/", syncResponse.token, ++sequence);
			if (syncStatusResponse.status) {
				log(kLogTypeInfo, "[+] EFS Sync Complete");
				log(kLogTypeInfo, "[+] SUCCESS. Reboot device and insert a different carriers SIM.");
				goto finish;
			} else {
				sleep(1000); // wait and check again
				syncRetries++;
			}
		}

		log(kLogTypeInfo, "[-] Sync Check Error. Device may still have been unlocked. Reboot and insert a different carriers SIM.");
	} catch (std::exception e) {
		QString msg = "[-] Error encountered during sync check: ";
		log(kLogTypeError, msg.append(e.what()));
		goto finish;
	}

	finish:
		disconnectPort();
		return;
}

/**
* @brief QcSamUnlockWindow::About
* Not working with Release -- mraabhimself
*/
void QcSamUnlockWindow::About()
{
	aboutDialog = new AboutDialog(this);
	
	aboutDialog->show();
	aboutDialog->raise();
	aboutDialog->activateWindow();

	//AboutDialog dialog(this);
	//dialog.exec();
}

/**
* @brief QcSamUnlockWindow::clearLog
*/
void QcSamUnlockWindow::clearLog() 
{
    ui->log->clear();
}

/**
* @brief QcSamUnlockWindow::saveLog
*/
void QcSamUnlockWindow::saveLog() 
{
    log(kLogTypeWarning, "[-] Not Implemented Yet");
}

/**
* @brief QcSamUnlockWindow::log
* @param message
*/
void QcSamUnlockWindow::log(int type, const char* message) 
{
	QString tmp = "gray";

	switch (type) {
	case kLogTypeDebug:		tmp = "gray";	break;
	case kLogTypeError:		tmp = "red";	break;
	case kLogTypeWarning:	tmp = "orange";	break;
	case kLogTypeInfo:		tmp = "green";	break;
	}

	ui->log->appendHtml(tmp.sprintf("<font color=%s><pre>%s</pre></font>", tmp.toStdString().c_str(), message));
}

/**
* @brief QcSamUnlockWindow::log
* @param message
*/
void QcSamUnlockWindow::log(int type, std::string message)
{
	QString tmp = "gray";

	switch (type) {
	case kLogTypeDebug:		tmp = "gray";	break;
	case kLogTypeError:		tmp = "red";	break;
	case kLogTypeWarning:	tmp = "orange";	break;
	case kLogTypeInfo:		tmp = "green";	break;
	}

	ui->log->appendHtml(tmp.sprintf("<font color=%s><pre>%s</pre></font>", tmp.toStdString().c_str(), message.c_str())); 
}

/**
* @brief QcSamUnlockWindow::log
* @param type
* @param message
*/
void QcSamUnlockWindow::log(int type, QString message)
{
	QString tmp = "gray";

    switch (type) {
		case kLogTypeDebug:		tmp = "gray";	break;
		case kLogTypeError:		tmp = "red";	break;		
		case kLogTypeWarning:	tmp = "orange";	break;
		case kLogTypeInfo:		tmp = "green";	break;
    }

	ui->log->appendHtml(tmp.sprintf("<font color=%s><pre>%s</pre></font>", tmp.toStdString().c_str(), message.toStdString().c_str()));
}
