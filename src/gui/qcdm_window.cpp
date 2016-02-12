/**
* LICENSE PLACEHOLDER
*
* @file qcdm_window.cpp
* @class QcdmWindow
* @package OpenPST
* @brief QCDM GUI interface class
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/

#include "qcdm_window.h"
#include "util/endian.h"


using namespace OpenPST::GUI;
using OpenPST::AboutDialog;
using OpenPST::QC::DmEfsManager;
using OpenPST::QC::DmEfsNode;


QcdmWindow::QcdmWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QcdmWindow),
    port("", 115200),
	efsManager(port),
	nvItemReadWorker(nullptr)
{
    ui->setupUi(this);

    disableUI();
    updatePortList();

    ui->decSpcValue->setInputMask("999999");
    ui->hexMeidValue->setInputMask("HHHHHHHHHHHHHH");
    ui->imeiValue->setInputMask("999999999999999");
    ui->mdnValue->setInputMask("9999999999");
    ui->minValue->setInputMask("9999999999");

	ui->subscriptionValue->addItem("", -1);
	ui->subscriptionValue->addItem("RUIM Only", kQcdmRuimConfigTypeRuim);
	ui->subscriptionValue->addItem("NV Only", kQcdmRuimConfigTypeNv);
	ui->subscriptionValue->addItem("RUIM Preferred", kQcdmRuimConfigTypePref);
	ui->subscriptionValue->addItem("GSM 1x",		 kQcdmRuimConfigTypeGsm1x);


    QObject::connect(ui->portListRefreshButton, SIGNAL(clicked()), this, SLOT(updatePortList()));
    QObject::connect(ui->portDisconnectButton, SIGNAL(clicked()), this, SLOT(disconnectPort()));
    QObject::connect(ui->portConnectButton, SIGNAL(clicked()), this, SLOT(connectPort()));
    QObject::connect(ui->sendSpcButton, SIGNAL(clicked()), this, SLOT(sendSpc()));
    QObject::connect(ui->sendPasswordButton, SIGNAL(clicked()), this, SLOT(sendPassword()));
    QObject::connect(ui->readMeidButton, SIGNAL(clicked()), this, SLOT(readMeid()));
    QObject::connect(ui->writeMeidButton, SIGNAL(clicked()), this, SLOT(writeMeid()));
    QObject::connect(ui->readImeiButton, SIGNAL(clicked()), this, SLOT(readImei()));
    QObject::connect(ui->readNamButton, SIGNAL(clicked()), this, SLOT(readNam()));
    QObject::connect(ui->writeNamButton, SIGNAL(clicked()), this, SLOT(writeNam()));
    QObject::connect(ui->readSpcButton, SIGNAL(clicked()), this, SLOT(readSpc()));
    QObject::connect(ui->writeSpcButton, SIGNAL(clicked()), this, SLOT(writeSpc()));
    QObject::connect(ui->readSubscriptionButton, SIGNAL(clicked()), this, SLOT(readSubscription()));
    QObject::connect(ui->writeSubscriptionButton, SIGNAL(clicked()), this, SLOT(writeSubscription()));

    QObject::connect(ui->clearLogButton, SIGNAL(clicked()), this, SLOT(clearLog()));
    QObject::connect(ui->saveLogButton, SIGNAL(clicked()), this, SLOT(saveLog()));

	QObject::connect(ui->sendPhoneModeButton, SIGNAL(clicked()), this, SLOT(setPhoneMode()));
	QObject::connect(ui->switchDloadModeButton, SIGNAL(clicked()), this, SLOT(switchToDload()));
	
	QObject::connect(ui->decSpcValue, SIGNAL(textChanged(QString)), this, SLOT(onSpcTextChanged(QString)));
	
	QObject::connect(ui->probeCommandsButton, SIGNAL(clicked()), this, SLOT(probeCommands()));
	
	// NV Tab Buttons
	QObject::connect(ui->nvReadSelectionToLogButton, SIGNAL(clicked()), this, SLOT(nvReadSelectionToLog()));
	QObject::connect(ui->nvReadSelectionToTextButton, SIGNAL(clicked()), this, SLOT(nvReadSelectionToText()));
	QObject::connect(ui->nvReadSelectionToBinaryButton, SIGNAL(clicked()), this, SLOT(nvReadSelectionToBinary()));
	QObject::connect(ui->nvReadRangeToLogButton, SIGNAL(clicked()), this, SLOT(nvReadRangeToLog()));
	QObject::connect(ui->nvReadRangeToTextButton, SIGNAL(clicked()), this, SLOT(nvReadRangeToText()));
	QObject::connect(ui->nvReadRangeToBinaryButton, SIGNAL(clicked()), this, SLOT(nvReadRangeToBinary()));
	QObject::connect(ui->readNvSingleButton, SIGNAL(clicked()), this, SLOT(nvReadSingleToLog()));
	QObject::connect(ui->nvReadSelectionSelectAllButton, SIGNAL(clicked()), this, SLOT(nvReadSelectionSelectAll()));
	QObject::connect(ui->nvReadSelectionClearSelectionButton, SIGNAL(clicked()), this, SLOT(nvReadSelectionClearSelection()));
	QObject::connect(ui->nvWriteSingleButton, SIGNAL(clicked()), this, SLOT(nvItemWriteSingle()));
	
	// EFS Browse Sub Tab
	QObject::connect(ui->efsListDirectoriesButton, SIGNAL(clicked()), this, SLOT(efsListDirectories()));
	QObject::connect(ui->efsReadAllButton, SIGNAL(clicked()), this, SLOT(efsReadAll()));
	QObject::connect(ui->efsSubsysCommandValue, SIGNAL(editingFinished()), this, SLOT(onEfsSubsysCommandCodeChange()));
	QObject::connect(ui->efsSubsysIdValue, SIGNAL(editingFinished()), this, SLOT(onEfsSubsysIdChange()));


	ui->efsDirectoryTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->efsDirectoryTree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onEfsTreeNodeRightClick(const QPoint &)));

	// EFS Manage Sub Tab
	QObject::connect(ui->efsFileRetrieveButton, SIGNAL(clicked()), this, SLOT(efsGetFile()));
	QObject::connect(ui->efsFileGetInfoButton, SIGNAL(clicked()), this, SLOT(efsGetFileInfo()));
	QObject::connect(ui->efsFileDeleteButton, SIGNAL(clicked()), this, SLOT(efsDeleteFile()));

	// File Action Menu
	//QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT());

	// EFS Action Menu 
	QObject::connect(ui->actionEfsHello, SIGNAL(triggered()), this, SLOT(efsHello()));
	QObject::connect(ui->actionEfsGetDeviceInfo, SIGNAL(triggered()), this, SLOT(efsGetDeviceInfo()));
	QObject::connect(ui->actionEfsQuery, SIGNAL(triggered()), this, SLOT(efsQuery()));
	QObject::connect(ui->actionEfsExtractFactoryImage, SIGNAL(triggered()), this, SLOT(efsExtractFactoryImage()));
	QObject::connect(ui->actionEfsMakeGoldenCopy, SIGNAL(triggered()), this, SLOT(efsMakeGoldenCopy()));
	QObject::connect(ui->actionEfsFilesystemImage, SIGNAL(triggered()), this, SLOT(efsFilesystemImage()));

	
	// Help Action Menu
	QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));


	// TODO: remov eme
	QObject::connect(ui->sendCommandButton, SIGNAL(clicked()), this, SLOT(sendCommand()));
	QObject::connect(ui->readButton, SIGNAL(clicked()), this, SLOT(readSome()));

	
	
	qRegisterMetaType<QcdmEfsDirectoryTreeWorkerRequest>("QcdmEfsDirectoryTreeWorkerRequest");
	qRegisterMetaType<QcdmEfsFileReadWorkerRequest>("QcdmEfsFileReadWorkerRequest");
	qRegisterMetaType<QcdmEfsFileWriteWorkerRequest>("QcdmEfsFileWriteWorkerRequest");
	qRegisterMetaType<QcdmMemoryReadWorkerRequest>("QcdmMemoryReadWorkerRequest");
	qRegisterMetaType<QcdmPrlWriteWorkerRequest>("QcdmPrlWriteWorkerRequest");
	qRegisterMetaType<QcdmNvItemReadWorkerRequest>("QcdmNvItemReadWorkerRequest");
	qRegisterMetaType<QcdmNvItemReadWorkerResponse>("QcdmNvItemReadWorkerResponse");

	// load nv item selection list
	QFile nvItemXmlList(":/xml/nv_items.xml");
	QDomDocument document;
	document.setContent(&nvItemXmlList);
	QDomNodeList nodes = document.elementsByTagName("NvItem");
	QString tmp;
	for (int i = 0; i < nodes.count(); i++){
		QDomNode elm = nodes.at(i);
		QDomElement e = elm.toElement();
		QListWidgetItem* item = new QListWidgetItem(ui->nvReadSelectionList);
		item->setText(tmp.sprintf("Item #%d - %s", e.attribute("id").toInt(), e.text().toStdString().c_str()));
		item->setData(1, e.attribute("id").toInt());
		ui->nvReadSelectionList->addItem(item);
	}
}

/**
* @brief QcdmWindow::~QcdmWindow
*/
QcdmWindow::~QcdmWindow()
{
    delete ui;
}


/**
* @brief QcdmWindow::updatePortList
*/
void QcdmWindow::updatePortList()
{
    std::vector<serial::PortInfo> devices = serial::list_ports();
    std::vector<serial::PortInfo>::iterator iter = devices.begin();

    ui->portListComboBox->clear();
    ui->portListComboBox->addItem("- Select a Port -", 0);

    while (iter != devices.end()) {
        serial::PortInfo device = *iter++;

        QString item = device.port.c_str();
        item.append(" - ").append(device.description.c_str());

        ui->portListComboBox->addItem(item, device.port.c_str());

        QString logMsg = "Found ";
        logMsg.append(device.hardware_id.c_str()).append(" on ").append(device.port.c_str());

        if (device.description.length()) {
            logMsg.append(" - ").append(device.description.c_str());
        }

        log(kLogTypeDebug, logMsg);
    }
}

/**
* @brief QcdmWindow::connectPort
*/
void QcdmWindow::connectPort()
{
	QString tmp;
	QString selected = ui->portListComboBox->currentData().toString();

    if (selected.compare("0") == 0) {
        log(kLogTypeWarning, "Select a Port First");
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
        log(kLogTypeError, "Invalid Port Type");
        return;
    }

    try {
        port.setPort(currentPort.port);

        port.open();

        ui->portConnectButton->setEnabled(false);
        ui->portDisconnectButton->setEnabled(true);
        ui->portListRefreshButton->setEnabled(false);
        ui->portListComboBox->setEnabled(false);
        enableUI();

        QString connectedText = "Connected to ";
        connectedText.append(currentPort.port.c_str());
        log(kLogTypeInfo, connectedText);
     

    } catch (serial::IOException e) {
        log(kLogTypeError, "Error Connecting To Serial Port");
        log(kLogTypeError, e.getErrorNumber() == 13 ? "Permission Denied. Try Running With Elevated Privledges." : e.what());
		return;
	} catch(std::exception e) {
		log(kLogTypeError, e.what());
		return;
	}

	getVersionInfo();
	getStatus();
	getGuid();

}

/**
* @brief QcdmWindow::disconnectPort
*/
void QcdmWindow::disconnectPort()
{
    if (port.isOpen()) {
        QString closeText = "Disconnected from ";
        closeText.append(currentPort.port.c_str());
        log(kLogTypeInfo, closeText);

        port.close();

        ui->portConnectButton->setEnabled(true);
        ui->portDisconnectButton->setEnabled(false);
        ui->portListRefreshButton->setEnabled(true);
        ui->portListComboBox->setEnabled(true);
        disableUI();
    }
}

/**
* @brief QcdmWindow::getVersion
*/
void QcdmWindow::getVersionInfo()
{
	QcdmVersionResponse version;
	QString tmp;
	uint16_t diagVersion;

	try {
		version = port.getVersion();
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}


	log(kLogTypeInfo, "===========\nVersion Info\n===========");
	log(kLogTypeInfo, tmp.sprintf("Compiled %.11s %.8s", version.cdate, version.ctime));
	log(kLogTypeInfo, tmp.sprintf("Released %.11s %.8s", version.rdate, version.rtime));
	log(kLogTypeInfo, tmp.sprintf("Model: %.11s", version.ver_dir));
	log(kLogTypeInfo, tmp.sprintf("Firmware Rev: %d", version.firmwareRev));
	log(kLogTypeInfo, tmp.sprintf("HW Version %d.%d", version.hwVersionMajor, version.hwVersionMinor));
	log(kLogTypeInfo, tmp.sprintf("Station Class Mark: %d", version.caiRev));
	log(kLogTypeInfo, tmp.sprintf("CAI Rev: %d", version.caiRev));
	log(kLogTypeInfo, tmp.sprintf("Slot Cycle Index: %d", version.slotCycleIndex));

	try {

		diagVersion = port.getDiagVersion();

		log(kLogTypeInfo, tmp.sprintf("Diag Version: %d", diagVersion));

	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
	}
}

/**
* @brief QcdmWindow::getStatus
*/
void QcdmWindow::getStatus()
{
	QcdmStatusResponse status;
	QString tmp;

	try {
		status = port.getStatus();
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	log(kLogTypeInfo, "===========\nStatus\n===========");
	log(kLogTypeInfo, tmp.sprintf("MIN1: %d %d", status.min1[0], status.min1[1]));
	log(kLogTypeInfo, tmp.sprintf("MIN2: %d %d", status.min2[0], status.min2[1]));
	log(kLogTypeInfo, tmp.sprintf("MIN Index: %d", status.minIndex));
	log(kLogTypeInfo, tmp.sprintf("CDMA RX State: %04X", status.cdmaRxState));
	log(kLogTypeInfo, tmp.sprintf("CDMA Good Frames: %d", status.cdmaGoodFrames));
	log(kLogTypeInfo, tmp.sprintf("Current Channel: %d", status.currentChannel));
	log(kLogTypeInfo, tmp.sprintf("SID: %d", status.sid));
	log(kLogTypeInfo, tmp.sprintf("NID: %d", status.nid));
	log(kLogTypeInfo, tmp.sprintf("Location ID: %d", status.locationId));
	log(kLogTypeInfo, tmp.sprintf("RSSI Level: %d", status.rssiLevel));
	log(kLogTypeInfo, tmp.sprintf("Power: %d", status.power));
}
/**
* @brief QcdmWindow::getGuid
*/
void QcdmWindow::getGuid()
{
	QcdmGuidResponse response;
	QString tmp;

	try {
		response = port.getGuid();
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("GUID: %04X-%04X-%04X-%04X", response.guid[0], response.guid[1], response.guid[2], response.guid[3]));
}

/**
* @brief QcdmWindow::sendSpc
*/
void QcdmWindow::sendSpc()
{
	bool valid;
	
	if (ui->sendSpcValue->text().length() != 6) {
        log(kLogTypeWarning, "Enter a Valid 6 Digit SPC");
        return;
    }

	try {
		valid = port.sendSpc(ui->sendSpcValue->text().toStdString());
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	if (valid) {
		log(kLogTypeInfo, "SPC Accepted: " + ui->sendSpcValue->text());
	} else {
		log(kLogTypeError, "SPC Not Accepted: " + ui->sendSpcValue->text());
	}
}

/**
* @brief QcdmWindow::sendPassword
*/
void QcdmWindow::sendPassword()
{
	if (ui->sendPasswordValue->text().length() != 16) {
        log(kLogTypeWarning, "Enter a Valid 16 Digit Password");
        return;
    }

	try {

		if (port.sendPassword(ui->sendPasswordValue->text().toStdString())) {
			log(kLogTypeInfo, "Password Accepted: " + ui->sendPasswordValue->text());
		} else {
			log(kLogTypeError, "Password Not Accepted: " + ui->sendPasswordValue->text());
		}
	}
	catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
}

/**
* @brief QcdmWindow::setPhoneMode
*/
void QcdmWindow::setPhoneMode()
{
	QcdmPhoneMode mode = (QcdmPhoneMode)ui->phoneModeValue->currentIndex();

	bool success;

	try {
		port.setPhoneMode(mode);

	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	if (mode == kQcdmPhoneModeReset) {
        disconnectPort();
        updatePortList();
    }

}
/**
* @brief QcdmWindow::switchToDload
*/
void QcdmWindow::switchToDload()
{
	try {
		port.switchToDload();

	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	disconnectPort();
	updatePortList();
}

/**
* @brief QcdmWindow::readMeid
*/
void QcdmWindow::readMeid()
{
	QcdmNvResponse nvItem;
	QString meid;
	QString tmp;

	int i;

	if (ui->hexMeidValue->text().length() != 0) {
        ui->hexMeidValue->setText("");
    }

	try {
		nvItem = port.readNV(NV_MEID_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	std::reverse((uint8_t*)&nvItem.data, ((uint8_t*)&nvItem.data) + 7);

	for (i = 0; i < 7; i++) {
		meid.append(tmp.sprintf((i == 6 ? "%02X" : "%02X "), nvItem.data[i]));
	}

	hexdump((uint8_t*)&nvItem.data, 7);

	ui->hexMeidValue->setText(meid);
}

/**
* @brief QcdmWindow::writeMeid
*/
void QcdmWindow::writeMeid()
{
    /*if (ui->hexMeidValue->text().length() != 14) {
        log(kLogTypeWarning, "Enter a Valid 14 Character MEID");
    }

    long data = HexToBytes(ui->hexMeidValue->text().toStdString());

    QcdmNvRawRequest packet;
    memcpy(&packet.data, &data, sizeof(data));

    int rx = port.setNvItem(NV_MEID_I, (const char *)&packet, sizeof(packet));

    if (rx == kDiagNvWrite) {
        log(kLogTypeInfo, "Write Success - MEID: " + ui->hexMeidValue->text());
    } else {
        log(kLogTypeError, "Write Failure - MEID");
    }*/
}

/**
* @brief QcdmWindow::readImei
*/
void QcdmWindow::readImei() {
	QcdmNvResponse nvItem;
	QString imei;
	QString tmp;
	int i;

	try {
		nvItem = port.readNV(NV_UE_IMEI_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	for (i = 1; i < 9; i++) {
		if (i == 1) {
			imei.append(tmp.sprintf("%X", (nvItem.data[i] & 0xF0) >> 4));
		} else {
			imei.append(tmp.sprintf("%X", (nvItem.data[i] & 0xF0) >> 4 | (nvItem.data[i] & 0x0F) << 4));
		}
	}  
	
	ui->imeiValue->setText(imei);
}

/**
* @brief
*/
void QcdmWindow::nvReadSelectionToLog()
{
	if (nvItemReadWorker != nullptr) {
		log(kLogTypeError, "Operation currently in progress");
		return;
	}
		
	QcdmNvItemReadWorkerRequest request = {};
	QList<QListWidgetItem*> selectedItems;
	QString tmp; 

	request.type = QcdmNvItemReadWorkerRequestTypeLog;

	selectedItems = ui->nvReadSelectionList->selectedItems();

	for (auto item : selectedItems) {
		request.items.push_back(item->data(1).toInt());
	}


	return nvReadRequest(request);
}

/**
* @brief
*/
void QcdmWindow::nvReadSelectionToText()
{
	if (nvItemReadWorker != nullptr) {
		log(kLogTypeError, "Operation currently in progress");
		return;
	}

	QString outPath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("*.txt"));

	if (!outPath.length()) {
		log(kLogTypeError, "Operation Cancelled");
		return;
	}

	QcdmNvItemReadWorkerRequest request = {};
	QList<QListWidgetItem*> selectedItems;
	QListWidgetItem* item;
	QString tmp;

	request.type = QcdmNvItemReadWorkerRequestTypeText;
	request.outFilePath = outPath.toStdString();

	selectedItems = ui->nvReadSelectionList->selectedItems();

	for (auto item : selectedItems) {
		request.items.push_back(item->data(1).toInt());
	}

	return nvReadRequest(request);
}


/**
* @brief
*/
void QcdmWindow::nvReadSelectionToBinary()
{
	if (nvItemReadWorker != nullptr) {
		log(kLogTypeError, "Operation currently in progress");
		return;
	}

	QString outPath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("*.bin"));

	if (!outPath.length()) {
		log(kLogTypeError, "Operation Cancelled");
		return;
	}

	QcdmNvItemReadWorkerRequest request = {};
	QList<QListWidgetItem*> selectedItems;
	QString tmp;

	request.type = QcdmNvItemReadWorkerRequestTypeBinary;
	request.outFilePath = outPath.toStdString();

	selectedItems = ui->nvReadSelectionList->selectedItems();

	for (auto item : selectedItems) {
		request.items.push_back(item->data(1).toInt());
	}

	return nvReadRequest(request);
}


/**
* @brief
*/
void QcdmWindow::nvReadSelectionClearSelection()
{
	ui->nvReadSelectionList->clearSelection();
}

/**
* @brief
*/
void QcdmWindow::nvReadSelectionSelectAll()
{
	ui->nvReadSelectionList->selectAll();
}


/**
* @brief
*/
void QcdmWindow::nvReadRangeToLog()
{
	QcdmNvItemReadWorkerRequest request = {};
	QString tmp;
	int i;
	int start, end;

	if (nvItemReadWorker != nullptr) {
		log(kLogTypeError, "Operation currently in progress");
		return;
	}

	start = std::stoul(ui->nvReadRangeFromValue->text().toStdString().c_str(), nullptr, 10);
	end = std::stoul(ui->nvReadRangeToValue->text().toStdString().c_str(), nullptr, 10);

	if (start > end) {
		log(kLogTypeError, "Starting item must be less than ending item.");
		return;
	}
	
	request.type = QcdmNvItemReadWorkerRequestTypeLog;

	for (i = start; i <= end; i++) {
		request.items.push_back(i);
	}

	return nvReadRequest(request);
}

/**
* @brief
*/
void QcdmWindow::nvReadRangeToText()
{
	QcdmNvItemReadWorkerRequest request = {};
	QString tmp;
	QString outPath;
	int i;
	int start, end;

	if (nvItemReadWorker != nullptr) {
		log(kLogTypeError, "Operation currently in progress");
		return;
	}

	start = std::stoul(ui->nvReadRangeFromValue->text().toStdString().c_str(), nullptr, 10);
	end = std::stoul(ui->nvReadRangeToValue->text().toStdString().c_str(), nullptr, 10);

	if (start > end) {
		log(kLogTypeError, "Starting item must be less than ending item.");
		return;
	}

	outPath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("*.txt"));

	if (!outPath.length()) {
		log(kLogTypeError, "Operation Cancelled");
		return;
	}

	request.type = QcdmNvItemReadWorkerRequestTypeText;
	request.outFilePath = outPath.toStdString();

	for (i = start; i <= end; i++) {
		request.items.push_back(i);
	}

	return nvReadRequest(request);
}

/**
* @brief
*/
void QcdmWindow::nvReadRangeToBinary()
{
	QcdmNvItemReadWorkerRequest request = {};
	QString tmp; 
	QString outPath;
	int i;
	int start, end;

	if (nvItemReadWorker != nullptr) {
		log(kLogTypeError, "Operation currently in progress");
		return;
	}

	start = std::stoul(ui->nvReadRangeFromValue->text().toStdString().c_str(), nullptr, 10);
	end = std::stoul(ui->nvReadRangeToValue->text().toStdString().c_str(), nullptr, 10);

	if (start > end) {
		log(kLogTypeError, "Starting item must be less than ending item.");
		return;
	}

	outPath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("*.bin"));

	if (!outPath.length()) {
		log(kLogTypeError, "Operation Cancelled");
		return;
	}

	request.type = QcdmNvItemReadWorkerRequestTypeBinary;
	request.outFilePath = outPath.toStdString();

	for (i = start; i <= end; i++) {
		request.items.push_back(i);
	}

	return nvReadRequest(request);
}

void QcdmWindow::nvReadSingleToLog()
{
	QcdmNvResponse nvItem;
	QString tmp;
	uint16_t item;

	if (!ui->readNvSingleValue->text().length()) {
		log(kLogTypeError, "Enter a valid item number.");
		return;
	}

	item = std::stoul(ui->readNvSingleValue->text().toStdString().c_str(), nullptr, 10);

	if (item < 0) {
		log(kLogTypeError, "Enter a valid item number.");
		return;
	}

	try {
		nvItem = port.readNV(item);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	hexdump(nvItem.data, DIAG_NV_ITEM_SIZE, tmp, true);

	log(kLogTypeInfo, tmp);
}

void QcdmWindow::nvReadRequest(QcdmNvItemReadWorkerRequest &request)
{
	QString tmp;
	
	ui->progressBar->reset();
	ui->progressBar->setMaximum(request.items.size());
	ui->progressBar->setMinimum(0);
	ui->progressBar->setValue(0);

	ui->progressBarLabelRight->setText(tmp.sprintf("0 / %lu items", request.items.size()));

	nvItemReadWorker = new QcdmNvItemReadWorker(port, request, this);

	connect(nvItemReadWorker, &QcdmNvItemReadWorker::update, this, &QcdmWindow::nvItemReadUpdate, Qt::QueuedConnection);
	connect(nvItemReadWorker, &QcdmNvItemReadWorker::complete, this, &QcdmWindow::nvItemReadComplete);
	connect(nvItemReadWorker, &QcdmNvItemReadWorker::error, this, &QcdmWindow::nvItemReadError);
	connect(nvItemReadWorker, &QcdmNvItemReadWorker::finished, nvItemReadWorker, &QObject::deleteLater);

	disableUI();

	nvItemReadWorker->start();
}


void QcdmWindow::nvItemReadUpdate(QcdmNvItemReadWorkerResponse response)
{
	QString tmp;
	QString result;
	QListWidgetItem* listItem;

	ui->progressBar->setValue(ui->progressBar->value() + 1);

	ui->progressBarLabelRight->setText(tmp.sprintf("%lu / %lu items", ui->progressBar->value(), ui->progressBar->maximum()));
	
	if (response.type == QcdmNvItemReadWorkerRequestTypeLog) {

		listItem = ui->nvReadSelectionList->item(response.item);

		if (listItem) {
			log(kLogTypeInfo, tmp.sprintf("===========\nItem %d - %s\n===========", response.item, ui->nvReadSelectionList->item(response.item)->text().toStdString().c_str()));

		} else {
			log(kLogTypeInfo, tmp.sprintf("===========\nItem %d\n===========", response.item));

		}
		hexdump(response.data, DIAG_NV_ITEM_SIZE, result, true);
		log(kLogTypeInfo, result);
	}
}

/**
* @brief
*/
void QcdmWindow::nvItemReadComplete(QcdmNvItemReadWorkerRequest request)
{
	nvItemReadWorker = nullptr;
	enableUI();
}

/**
* @brief
*/
void QcdmWindow::nvItemReadError(QcdmNvItemReadWorkerRequest request, QString msg)
{
	QString tmp;

	ui->progressBar->setValue(ui->progressBar->value() + 1);

	ui->progressBarLabelRight->setText(tmp.sprintf("%lu / %lu items", ui->progressBar->value(), ui->progressBar->maximum())); 
	

	if (request.type == QcdmNvItemReadWorkerRequestTypeLog) {
		log(kLogTypeError, tmp.sprintf("===========\nError Reading Item %d - %s\n===========", request.current, ui->nvReadSelectionList->item(request.current)->text().toStdString().c_str()));
		log(kLogTypeError, msg);
	} else {
		log(kLogTypeError, tmp.sprintf("Error reading item %d: %s", request.current, msg.toStdString().c_str()));
	}

}

void QcdmWindow::nvItemWriteSingle()
{
	QString tmp;
	uint8_t item[DIAG_NV_ITEM_SIZE] = {};
	QString input = ui->nvWriteSingleValue->toPlainText();

	input.remove(" ");

	if (!input.length()) {
		log(kLogTypeError, "Please enter data to write");
		return;
	} else if (input.length() / 2 > DIAG_NV_ITEM_SIZE) {
		log(kLogTypeError, tmp.sprintf("Data is too long. Must be a maxium of %d bytes", DIAG_NV_ITEM_SIZE));
		return;
	}

	do {
		input.append(0x00);
	} while (input.length() < DIAG_NV_ITEM_SIZE);

	hexdump((unsigned char *)input.toStdString().c_str(), input.size());
}

/**
* @brief QcdmWindow::readNam
*/
void QcdmWindow::readNam() {
    // readMdn();
    // readMin();
    // readSid();
    // readSystemPref();
    // readPrefMode();
    // readPrefServ();
    // readRoamPref();

    ReadPapUserId();
    ReadPapPassword();
    ReadPppUserId();
    ReadPppPassword();
    ReadHdrAnUserId();
    ReadHdrAnPassword();
    ReadHdrAnLongUserId();
    ReadHdrAnLongPassword();
    ReadHdrAnPppUserId();
}

/**
* @brief QcdmWindow::writeNam
*/
void QcdmWindow::writeNam() {
    // writeMdn();
    // writeSystemPref();
    // writePrefServ();
    // writeRoamPref();
}

/**
* @brief QcdmWindow::ReadMdn
*/
void QcdmWindow::ReadMdn() {

	QcdmNvResponse nvItem;
	QString hex;

	try {
		nvItem = port.readNV(NV_DIR_NUMBER_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	/*
	log(kLogTypeInfo, "Read Success - Item Number: " + ui->nvItemValue->text() + "<br>" + hex);
	
	
	uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_DIR_NUMBER_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        std::string tmp = hexToString((char *)rxPacket->data, 9);
        QString mdnValue = QString::fromStdString(tmp);

        ui->mdnValue->setText(mdnValue);

        log(kLogTypeInfo, "Read Success - MDN: " + mdnValue);
    } else {
        log(kLogTypeError, "Read Failure - MDN");
    }*/
}

/**
* @brief QcdmWindow::WriteMdn
*/
void QcdmWindow::WriteMdn()
{
    /*if (ui->mdnValue->text().length() != 10) {
        log(kLogTypeWarning, "Enter a Valid 10 Digit MDN");
        return;
    }

    QcdmNvAltRequest packet;
    memcpy(&packet.data, ui->mdnValue->text().toStdString().c_str(), 10);

    int rx = port.setNvItem(NV_DIR_NUMBER_I, (const char *)&packet, 11);

    if (rx == kDiagNvWrite) {
        log(kLogTypeInfo, "Write Success - MDN: " + ui->mdnValue->text());
    } else {
        log(kLogTypeError, "Write Failure - MDN");
    }*/
}

/**
* @brief QcdmWindow::ReadMin
*/
void QcdmWindow::ReadMin() {
	
	QcdmNvResponse min1;
	QcdmNvResponse min2;

	try {
		min1 = port.readNV(NV_MIN1_I);
		min2 = port.readNV(NV_MIN2_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*unsigned char min1Chunk[3], min2Chunk[1];
    std::string min1, min2;

    uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_MIN1_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        min1Chunk[0] = rxPacket->data[3];
        min1Chunk[1] = rxPacket->data[2];
        min1Chunk[2] = rxPacket->data[1];
        min1Chunk[3] = rxPacket->data[0];

        min1 = min1Decode(min1Chunk);
    }

    resp = nullptr;

    rx = port.getNvItem(NV_MIN2_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        min2Chunk[0] = rxPacket->data[3];
        min2Chunk[1] = rxPacket->data[2];

        min2 = min2Decode(min2Chunk);
    }

    QString decodedMin;
    decodedMin.append(QString::fromStdString(min2));
    decodedMin.append(QString::fromStdString(min1));

    if (decodedMin.length() == 10) {
        ui->minValue->setText(decodedMin);

        log(kLogTypeInfo, "Read Success - MIN: " + decodedMin);
    } else {
        log(kLogTypeError, "Read Failure - MIN");
    }*/
}

/**
* @brief QcdmWindow::ReadSid
*/
void QcdmWindow::ReadSid() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_HOME_SID_NID_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_HOME_SID_NID_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        unsigned char data[2];

        data[0] = rxPacket->data[1];
        data[1] = rxPacket->data[0];

        std::string tmp = sidDecode(data);
        QString result = QString::fromStdString(tmp);
        result = FixedEmptyTrim(result);

        ui->sidValue->setText(result);

        log(kLogTypeInfo, "Read Success - SID: " + result);
    } else {
        log(kLogTypeError, "Read Failure - SID");
    }*/
}

/**
* @brief QcdmWindow::ReadSystemPref
*/
void QcdmWindow::ReadSystemPref()
{
	QcdmNvResponse nvItem; 
	
	try {
		nvItem = port.readNV(NV_SYSTEM_PREF_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_SYSTEM_PREF_I, &resp);

    if (rx == kDiagNvRead) {
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        ui->systemPrefValue->setCurrentIndex(rxPacket->data[0] + 1);

        log(kLogTypeInfo, "Read Success - System Pref: " + ui->systemPrefValue->currentText());
    } else {
        log(kLogTypeError, "Read Failure - System Pref");
    }*/
}

/**
* @brief QcdmWindow::WriteSystemPref
*/
void QcdmWindow::WriteSystemPref()
{
    /*int mode = ui->systemPrefValue->currentIndex() - 1;

    if (mode < 0) {
        log(kLogTypeWarning, "Select a System Pref to Write");
        return;
    }

    uint8_t* resp = nullptr;

    QcdmNvAltRequest packet;
    memcpy(&packet.data, static_cast<const char *>(static_cast<void*>(&mode)), 1);

    int rx = port.setNvItem(NV_SYSTEM_PREF_I, (const char *)&packet, 2, &resp);

    if (rx == kDiagNvWrite) {
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        ui->systemPrefValue->setCurrentIndex(rxPacket->data[0] + 1);

        log(kLogTypeInfo, "Write Success - System Pref: " + ui->systemPrefValue->currentText());
    } else {
        log(kLogTypeError, "Write Failure - System Pref");
    }*/
}

/**
* @brief QcdmWindow::ReadPrefMode
*/
void QcdmWindow::ReadPrefMode()
{
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_PREF_MODE_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_PREF_MODE_I, &resp);

    if (rx == kDiagNvRead) {
        QString result = "NOT_IMPLEMENTED";

        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        int newIndex = 0;

        switch (rxPacket->data[0]) {

        case PREF_MODE_AUTOMATIC:
            result = "AUTOMATIC";
            newIndex = 1;
            break;
        case PREF_MODE_CDMA_GSM_WCDMA:
            result = "CDMA_GSM_WCDMA";
            newIndex = 2;
            break;
        case PREF_MODE_CDMA_HDR:
            result = "CDMA_HDR";
            newIndex = 3;
            break;
        case PREF_MODE_CDMA_HDR_GSM_WCDMA:
            result = "CDMA_HDR_GSM_WCDMA";
            newIndex = 4;
            break;
        case PREF_MODE_CDMA_IS2000:
            result = "CDMA_IS2000";
            newIndex = 5;
            break;
        case PREF_MODE_CDMA_IS95:
            result = "CDMA_IS95";
            newIndex = 6;
            break;
        case PREF_MODE_GSM_GPRS_EDGE:
            result = "GSM_GPRS_EDGE";
            newIndex = 7;
            break;
        case PREF_MODE_GSM_WCDMA:
            result = "GSM_WCDMA";
            newIndex = 8;
            break;
        case PREF_MODE_HDR:
            result = "HDR";
            newIndex = 9;
            break;
        case PREF_MODE_LTE:
            result = "LTE";
            newIndex = 10;
            break;
        case PREF_MODE_LTE_CDMA:
            result = "LTE_CDMA";
            newIndex = 11;
            break;
        case PREF_MODE_LTE_CDMA_GSM:
            result = "LTE_CDMA_GSM";
            newIndex = 12;
            break;
        case PREF_MODE_LTE_CDMA_HDR:
            result = "LTE_CDMA_HDR";
            newIndex = 13;
            break;
        case PREF_MODE_LTE_CDMA_WCDMA:
            result = "LTE_CDMA_WCDMA";
            newIndex = 14;
            break;
        case PREF_MODE_LTE_GSM:
            result = "LTE_GSM";
            newIndex = 15;
            break;
        case PREF_MODE_LTE_GSM_WCDMA:
            result = "LTE_GSM_WCDMA";
            newIndex = 16;
            break;
        case PREF_MODE_LTE_HDR:
            result = "LTE_HDR";
            newIndex = 17;
            break;
        case PREF_MODE_LTE_HDR_GSM:
            result = "LTE_HDR_GSM";
            newIndex = 18;
            break;
        case PREF_MODE_LTE_HDR_WCDMA:
            result = "LTE_HDR_WCDMA";
            newIndex = 19;
            break;
        case PREF_MODE_LTE_WCDMA:
            result = "LTE_WCDMA";
            newIndex = 20;
            break;
        case PREF_MODE_WCDMA_HSDPA:
            result = "WCDMA_HSPDA";
            newIndex = 20;
            break;
        }

        ui->prefModeValue->setCurrentIndex(newIndex);

        log(kLogTypeInfo, "Read Success - Pref Mode: " + result);
    } else {
        log(kLogTypeError, "Read Failure - Pref Mode");
    }*/
}

/**
* @brief QcdmWindow::ReadPrefServ
*/
void QcdmWindow::ReadPrefServ()
{
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_CDMA_PREF_SERV_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_CDMA_PREF_SERV_I, &resp);

    if (rx == kDiagNvRead) {
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        ui->prefServValue->setCurrentIndex(rxPacket->data[0] + 1);

        log(kLogTypeInfo, "Read Success - Pref Serv: " + ui->prefServValue->currentText());
    } else {
        log(kLogTypeError, "Read Failure - Pref Serv");
    }*/
}

/**
* @brief QcdmWindow::WritePrefServ
*/
void QcdmWindow::WritePrefServ()
{
    /*int mode = ui->prefServValue->currentIndex() - 1;

    if (mode < 0) {
        log(kLogTypeWarning, "Select a System Pref to Write");
        return;
    }

    uint8_t* resp = nullptr;

    QcdmNvAltRequest packet;
    memcpy(&packet.data, static_cast<const char *>(static_cast<void*>(&mode)), 1);

    int rx = port.setNvItem(NV_CDMA_PREF_SERV_I, (const char *)&packet, 2, &resp);

    if (rx == kDiagNvWrite) {
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        ui->prefServValue->setCurrentIndex(rxPacket->data[0] + 1);

        log(kLogTypeInfo, "Write Success - System Pref: " + ui->prefServValue->currentText());
    } else {
        log(kLogTypeError, "Write Failure - System Pref");
    }*/
}

/**
* @brief QcdmWindow::ReadRoamPref
*/
void QcdmWindow::ReadRoamPref()
{
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_ROAM_PREF_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_ROAM_PREF_I, &resp);

    if (rx == kDiagNvRead) {
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        int i = 0;

        switch (rxPacket->data[0]) {
        case ROAM_PREF_HOME:
            i = 1;
            break;
        case ROAM_PREF_AFFILIATED:
            i = 2;
            break;
        case ROAM_PREF_AUTOMATIC:
            i = 3;
            break;
        }

        ui->roamPrefValue->setCurrentIndex(i);

        log(kLogTypeInfo, "Read Success - Roam Pref: " + ui->roamPrefValue->currentText());
    } else {
        log(kLogTypeError, "Read Failure - Roam Pref");
    }*/
}

/**
* @brief QcdmWindow::WriteRoamPref
*/
void QcdmWindow::WriteRoamPref()
{
   /* int mode = ui->roamPrefValue->currentIndex() - 1;

    if (mode < 0) {
        log(kLogTypeWarning, "Select a Roam Pref to Write");
        return;
    }

    uint8_t* resp = nullptr;

    QcdmNvAltRequest packet;

    switch(mode) {
    case 0:
        packet.data[0] = ROAM_PREF_HOME;
        break;
    case 1:
        packet.data[0] = ROAM_PREF_AFFILIATED;
        break;
    case 2:
        packet.data[0] = ROAM_PREF_AUTOMATIC;
        break;
    }

    int rx = port.setNvItem(NV_ROAM_PREF_I, (const char *)&packet, 2, &resp);

    if (rx == kDiagNvWrite) {
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        int i = 0;

        switch (rxPacket->data[0]) {
        case ROAM_PREF_HOME:
            i = 1;
            break;
        case ROAM_PREF_AFFILIATED:
            i = 2;
            break;
        case ROAM_PREF_AUTOMATIC:
            i = 3;
            break;
        }

        ui->roamPrefValue->setCurrentIndex(i);

        log(kLogTypeInfo, "Write Success - Roam Pref: " + ui->roamPrefValue->currentText());
    } else {
        log(kLogTypeError, "Write Failure - Roam Pref");
    }*/
}

/**
* @brief QcdmWindow::readSpc
*/
void QcdmWindow::readSpc()
{

	/*QcdmNvResponse resp = {};

    int ret = 0;

	switch (ui->readSpcMethod->currentIndex()) {
		case kSpcReadTypeNv:
			ret = port.getNvItem(NV_SEC_CODE_I, &resp);
			break;
		case kSpcReadTypeHtc:
			port.sendHtcNvUnlock(); // HTC Method
			ret = port.getNvItem(NV_SEC_CODE_I, &resp);
			break;
/*case kSpcReadTypeLg:
			port.sendLgNvUnlock(); // LG Method
			ret = port.getLgSpc(&resp);
			break;
		case kSpcReadTypeSamsung:
		case kSpcReadTypeEfs:
			LOGD("Method not implemented");
			break;
    }

    if (ret != kDiagNvRead){
        QcdmNvResponse* rxPacket = (QcdmNvResponse*)resp;

        std::string result = hexToString((char *)rxPacket->data, 5);
		
        ui->decSpcValue->setText(QString::fromStdString(result));

        log(kLogTypeInfo, "Read Success - SPC: " + result);
    } else {
        log(kLogTypeError, "Read Failure - SPC");
    }*/
}

/**
* @brief QcdmWindow::writeSpc
*/
void QcdmWindow::writeSpc()
{
   /* if (ui->decSpcValue->text().length() != 6) {
        log(kLogTypeWarning, "Enter a Valid 6 Digit SPC");
        return;
    }

    int rx = port.setNvItem(NV_SEC_CODE_I, ui->decSpcValue->text().toStdString().c_str(), 6);

    if (rx == kDiagNvWrite) {
        log(kLogTypeInfo, "Write Success - SPC: " + ui->decSpcValue->text());
    } else {
        log(kLogTypeError, "Write Failure - SPC");
    }*/
}

/**
* @brief QcdmWindow::readSubscription
*/
void QcdmWindow::readSubscription() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_RTRE_CONFIG_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}	
	
	ui->subscriptionValue->setCurrentIndex(ui->subscriptionValue->findData(nvItem.data[0]));
}

/**
* @brief QcdmWindow::writeSubscription
*/
void QcdmWindow::writeSubscription()
{
	bool result;
	QcdmNvRequest packet = {};

	int mode = ui->subscriptionValue->currentData().toUInt();

	if (mode < 0) {
        log(kLogTypeError, "Select a Subsciption Mode to Write");
        return;
    }

	packet.data[0] = mode;

	try {
		port.writeNV(NV_RTRE_CONFIG_I, &packet.data[0], sizeof(packet.data));	
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	ui->subscriptionValue->setCurrentIndex(ui->subscriptionValue->findData(packet.data[0]));
	
	log(kLogTypeInfo, "Write Success - Subscription Mode: " + ui->subscriptionValue->currentText());

}

/**
* @brief QcdmWindow::ReadPapUserId
*/
void QcdmWindow::ReadPapUserId() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_PAP_USER_ID_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_PAP_USER_ID_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAlt2Response* rxPacket = (QcdmNvAlt2Response*)resp;

        std::string tmp = hexToString((char *)rxPacket->data, DIAG_NV_ITEM_SIZE);
        QString result = QString::fromStdString(tmp);
        result = FixedEmptyTrim(result);

        ui->papUserIdValue->setText(result);

        log(kLogTypeInfo, "Read Success - PAP User ID: " + result);
    } else {
        log(kLogTypeError, "Read Failure - PAP User ID");
    }*/
}

/**
* @brief QcdmWindow::ReadPapPassword
*/
void QcdmWindow::ReadPapPassword() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_PAP_PASSWORD_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	/*
    int rx = port.getNvItem(NV_PAP_PASSWORD_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        std::string tmp = bytesToHex((unsigned char *)rxPacket->data, 16, false);
        tmp = tmp.erase(tmp.find_last_not_of("0") + 1);
        QString result = QString::fromStdString(tmp).toUpper();

        ui->papPasswordValue->setText(result);

        log(kLogTypeInfo, "Read Success - PAP Password: " + result);
    } else {
        log(kLogTypeError, "Read Failure - PAP Password");
    }*/
}

/**
* @brief QcdmWindow::ReadPppUserId
*/
void QcdmWindow::ReadPppUserId() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_PPP_USER_ID_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_PPP_USER_ID_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAlt2Response* rxPacket = (QcdmNvAlt2Response*)resp;

        std::string tmp = hexToString((char *)rxPacket->data, DIAG_NV_ITEM_SIZE);
        QString result = QString::fromStdString(tmp);
        result = FixedEmptyTrim(result);

        ui->pppUserIdValue->setText(result);

        log(kLogTypeInfo, "Read Success - PPP User ID: " + result);
    } else {
        log(kLogTypeError, "Read Failure - PPP User ID");
    }*/
}

/**
* @brief QcdmWindow::ReadPppPassword
*/
void QcdmWindow::ReadPppPassword() {
	QcdmNvResponse nvItem;
	
	try {
		nvItem = port.readNV(NV_PPP_PASSWORD_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	//ui->pppPasswordValue->setText(nvItem.data);

    /*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_PPP_PASSWORD_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        std::string tmp = bytesToHex((unsigned char *)rxPacket->data, 16, false);
        tmp = tmp.erase(tmp.find_last_not_of("0") + 1);
        QString result = QString::fromStdString(tmp).toUpper();

        

        log(kLogTypeInfo, "Read Success - PPP Password: " + result);
    } else {
        log(kLogTypeError, "Read Failure - PPP Password");
    }*/
}

/**
* @brief QcdmWindow::ReadHdrAnUserId
*/
void QcdmWindow::ReadHdrAnUserId() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_HDR_AN_AUTH_NAI_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	/*
    int rx = port.getNvItem(NV_HDR_AN_AUTH_NAI_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAlt2Response* rxPacket = (QcdmNvAlt2Response*)resp;

        std::string tmp = hexToString((char *)rxPacket->data, DIAG_NV_ITEM_SIZE);
        QString result = QString::fromStdString(tmp);
        result = FixedEmptyTrim(result);

        ui->hdrAnUserIdValue->setText(result);

        log(kLogTypeInfo, "Read Success - HDR AN User ID: " + result);
    } else {
        log(kLogTypeError, "Read Failure - HDR AN User ID");
    }*/
}

/**
* @brief QcdmWindow::ReadHdrAnPassword
*/
void QcdmWindow::ReadHdrAnPassword() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_HDR_AN_AUTH_PASSWORD_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_HDR_AN_AUTH_PASSWORD_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        std::string tmp = bytesToHex((unsigned char *)rxPacket->data, 16, false);
        tmp = tmp.erase(tmp.find_last_not_of("0") + 1);
        QString result = QString::fromStdString(tmp).toUpper();

        ui->hdrAnPasswordValue->setText(result);

        log(kLogTypeInfo, "Read Success - HDR AN Password: " + result);
    } else {
        log(kLogTypeError, "Read Failure - HDR AN Password");
    }*/
}

/**
* @brief QcdmWindow::ReadHdrAnLongUserId
*/
void QcdmWindow::ReadHdrAnLongUserId() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_HDR_AN_AUTH_USER_ID_LONG_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_HDR_AN_AUTH_USER_ID_LONG_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAlt2Response* rxPacket = (QcdmNvAlt2Response*)resp;

        std::string tmp = hexToString((char *)rxPacket->data, DIAG_NV_ITEM_SIZE);
        QString result = QString::fromStdString(tmp);
        result = FixedEmptyTrim(result);

        ui->hdrAnLongUserIdValue->setText(result);

        log(kLogTypeInfo, "Read Success - HDR AN LONG User ID: " + result);
    } else {
        log(kLogTypeError, "Read Failure - HDR AN LONG User ID");
    }*/
}

/**
* @brief QcdmWindow::ReadHdrAnLongPassword
*/
void QcdmWindow::ReadHdrAnLongPassword() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_HDR_AN_AUTH_PASSWD_LONG_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_HDR_AN_AUTH_PASSWORD_LONG_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        std::string tmp = bytesToHex((unsigned char *)rxPacket->data, 16, false);
        tmp = tmp.erase(tmp.find_last_not_of("0") + 1);
        QString result = QString::fromStdString(tmp).toUpper();

        ui->hdrAnLongPasswordValue->setText(result);

        log(kLogTypeInfo, "Read Success - HDR AN LONG Password: " + result);
    } else {
        log(kLogTypeError, "Read Failure - HDR AN LONG Password");
    }*/
}

/**
* @brief QcdmWindow::ReadHdrAnPppUserId
*/
void QcdmWindow::ReadHdrAnPppUserId() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_HDR_AN_PPP_USER_ID_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}

	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_HDR_AN_AUTH_USER_ID_PPP_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAlt2Response* rxPacket = (QcdmNvAlt2Response*)resp;

        std::string tmp = hexToString((char *)rxPacket->data, DIAG_NV_ITEM_SIZE);
        QString result = QString::fromStdString(tmp);
        result = FixedEmptyTrim(result);

        ui->hdrAnPppUserIdValue->setText(result);

        log(kLogTypeInfo, "Read Success - HDR AN PPP User ID: " + result);
    } else {
        log(kLogTypeError, "Read Failure - HDR AN PPP User ID");
    }*/
}

/**
* @brief QcdmWindow::ReadHdrAnPppPassword
*/
void QcdmWindow::ReadHdrAnPppPassword() {
	QcdmNvResponse nvItem;

	try {
		nvItem = port.readNV(NV_HDR_AN_PPP_PASSWORD_I);
	} catch (std::exception &e) {
		log(kLogTypeError, e.what());
		return;
	}
	
	/*uint8_t* resp = nullptr;

    int rx = port.getNvItem(NV_HDR_AN_AUTH_PASSWORD_PPP_I, &resp);

    if (rx == kDiagNvRead){
        QcdmNvAltResponse* rxPacket = (QcdmNvAltResponse*)resp;

        std::string tmp = bytesToHex((unsigned char *)rxPacket->data, 16, false);
        tmp = tmp.erase(tmp.find_last_not_of("0") + 1);
        QString result = QString::fromStdString(tmp).toUpper();

        ui->hdrAnLongPasswordValue->setText(result);

        log(kLogTypeInfo, "Read Success - HDR AN PPP Password: " + result);
    } else {
        log(kLogTypeError, "Read Failure - HDR AN PPP Password");
    }*/
}

/**
* @brief QcdmWindow::onSpcTextChanged
*/
void QcdmWindow::onSpcTextChanged(QString value) {
    if (value.length() == 6) {
        std::string tmp = bytesToHex((unsigned char *)value.toStdString().c_str(), 6, false);
        QString result = QString::fromStdString(tmp);

        ui->hexSpcValue->setText(result);
    }
}

/**
* @brief QcdmWindow::writeSubscription
* Fix odd QString::trimmed() behavior
*/
QString QcdmWindow::FixedEmptyTrim(QString input) {
    return input.trimmed() == "." ? "" : input.trimmed();
}

/**
* @brief QcdmWindow::disableUI
*/
void QcdmWindow::disableUI() {

	ui->securityGroupBox->setEnabled(false);
	ui->modeGroupBox->setEnabled(false);
	ui->qcdmTabWidget->setEnabled(false);
}

/**
* @brief QcdmWindow::enableUI
*/
void QcdmWindow::enableUI() {
	ui->securityGroupBox->setEnabled(true);
	ui->modeGroupBox->setEnabled(true);
	ui->qcdmTabWidget->setEnabled(true);
}

/**
* @brief QcdmWindow::efsHello
*/
void QcdmWindow::efsHello() {
	
	QString tmp;
	QcdmEfsHelloResponse response;
		
	if(!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}
	
	try {
		response = efsManager.hello();
	} catch (std::exception e) {
		log(kLogTypeError, "Error sending hello");
		return;
	}
		
	log(kLogTypeInfo, tmp.sprintf("Command: 0x%02X - %d", response.header.command, response.header.command));
	log(kLogTypeInfo, tmp.sprintf("Subsys ID: 0x%02X - %d", response.header.subsysId, response.header.subsysId));
	log(kLogTypeInfo, tmp.sprintf("Subsys Command: 0x%02X - %d", response.header.subsysCommand, response.header.subsysCommand));
	log(kLogTypeInfo, tmp.sprintf("Target Packet Window Size: %d", response.targetPacketWindowSize));
	log(kLogTypeInfo, tmp.sprintf("Target Packet Window Size in Bytes: %d", response.targetPacketWindowByteSize));
	log(kLogTypeInfo, tmp.sprintf("Host Packet Window Size: %d", response.hostPacketWindowSize));
	log(kLogTypeInfo, tmp.sprintf("Host Packet Window Size in Bytes: %d", response.hostPacketWindowByteSize));
	log(kLogTypeInfo, tmp.sprintf("Directory Iterator Window Size: %d", response.dirIteratorWindowSize));
	log(kLogTypeInfo, tmp.sprintf("Directory Iterator Window Size in Bytes: %d", response.dirIteratorWindowByteSize));
	log(kLogTypeInfo, tmp.sprintf("Version: %d", response.version));
	log(kLogTypeInfo, tmp.sprintf("Min Version: %d", response.minVersion));
	log(kLogTypeInfo, tmp.sprintf("Feature Bits: 0x%04X", response.featureBits));
}

/**
* @brief QcdmWindow::efsGetDeviceInfo
*/
void QcdmWindow::efsGetDeviceInfo() {

	QcdmEfsDeviceInfoResponse response;
	QString tmp;

	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	try {
		response = efsManager.getDeviceInfo();
	} catch (std::exception e) {
		log(kLogTypeError, "Error getting device info");
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("Command: 0x%02X - %d", response.header.command, response.header.command));
	log(kLogTypeInfo, tmp.sprintf("Subsys ID: 0x%02X - %d", response.header.subsysId, response.header.subsysId));
	log(kLogTypeInfo, tmp.sprintf("Subsys Command: 0x%02X - %d", response.header.subsysCommand, response.header.subsysCommand));
	log(kLogTypeInfo, tmp.sprintf("Error: %d", response.error));
	log(kLogTypeInfo, tmp.sprintf("Total Number of Blocks: %d", response.totalNumberOfBlocks));
	log(kLogTypeInfo, tmp.sprintf("Pages Per Block: %d", response.pagesPerBlock));
	log(kLogTypeInfo, tmp.sprintf("Page Size: %d bytes", response.pageSize));
	log(kLogTypeInfo, tmp.sprintf("Total Page Size: %d", response.totalPageSize));
	log(kLogTypeInfo, tmp.sprintf("Maker ID: %d", response.makerId));
	log(kLogTypeInfo, tmp.sprintf("Device ID: %d", response.deviceId));
	log(kLogTypeInfo, tmp.sprintf("Device Type: %d", response.deviceType));
	log(kLogTypeInfo, tmp.sprintf("Device Name: %s", response.name));
}

/**
* @brief QcdmWindow::efsQuery
*/
void QcdmWindow::efsQuery() {
	QcdmEfsQueryResponse response;
	QString tmp;

	//efsQueryButton
	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	try {
		response = efsManager.query();
	} catch (std::exception e) {
		log(kLogTypeError, "Error querying for efs settings");
		return;
	}
	
	log(kLogTypeInfo, tmp.sprintf("Command: 0x%02X - %d", response.header.command, response.header.command));
	log(kLogTypeInfo, tmp.sprintf("Subsys ID: 0x%02X - %d", response.header.subsysId, response.header.subsysId));
	log(kLogTypeInfo, tmp.sprintf("Subsys Command: 0x%02X - %d", response.header.subsysCommand, response.header.subsysCommand));
	log(kLogTypeInfo, tmp.sprintf("Max File Name Length: %d", response.maxFilenameLength));
	log(kLogTypeInfo, tmp.sprintf("Max Path Name Length: %d", response.mapPathnameLength));
	log(kLogTypeInfo, tmp.sprintf("Max Symlink Depth: %d", response.maxSymlinkDepth));
	log(kLogTypeInfo, tmp.sprintf("Max File Size: %d", response.maxFileSize));
	log(kLogTypeInfo, tmp.sprintf("Max Directories: %d", response.maxDirectories));
	log(kLogTypeInfo, tmp.sprintf("Max Mounts: %d", response.maxMounts));
}



/**
* @brief QcdmWindow::efsListDirectories
*/
void QcdmWindow::efsListDirectories() {

	QTreeWidgetItem *rootNode;
	QcdmEfsDirectoryTreeWorker* worker;
	QString tmp;
	std::vector<DmEfsNode> contents;
	QcdmEfsDirectoryTreeWorkerRequest request = {};

	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	disableUI();

	worker = new QcdmEfsDirectoryTreeWorker(efsManager, request, this);
		
	workers.insert(workers.end(), reinterpret_cast<QThread*>(worker));
	log(kLogTypeError, tmp.sprintf("%d", workers.size()));

	enableUI();
	return;
	/*connect(readWorker, &StreamingDloadReadWorker::chunkReady, this, &StreamingDloadWindow::readChunkReadyHandler, Qt::QueuedConnection);
	connect(readWorker, &StreamingDloadReadWorker::complete, this, &StreamingDloadWindow::readCompleteHandler);
	connect(readWorker, &StreamingDloadReadWorker::error, this, &StreamingDloadWindow::readChunkErrorHandler);
	connect(readWorker, &StreamingDloadReadWorker::finished, readWorker, &QObject::deleteLater);*/
	
	try {
		contents = efsManager.readDir("/", true);
	} catch (std::exception& e) {
		enableUI();
		log(kLogTypeError, e.what());
		return;
	}
	
	ui->efsDirectoryTree->clear();

	log(kLogTypeInfo, tmp.sprintf("Directory / containts %d entries", contents.size()));
	
	rootNode = new QTreeWidgetItem(ui->efsDirectoryTree);
	rootNode->setText(kEfsBrowserColumnName, "/");
	rootNode->setIcon(kEfsBrowserColumnName, QIcon(":/images/folder-2x.png"));
	rootNode->setExpanded(true);

	EfsAddTreeNodes(rootNode, contents);

	enableUI();
}
/**
* @brief QcdmWindow::efsReadAll
*/
void QcdmWindow::efsReadAll()
{
	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("*.*"));

	if (!fileName.length()) {
		log(kLogTypeInfo, "Operation Cancelled");
		return;
	}

    log(kLogTypeInfo, "Operation Cancelled");

    /*if (efsManager.read("/SUPL/Cert0", fileName.toStdString()) != efsManager.kDmEfsSuccess) {
		log(kLogTypeError, "Error reading file");
    }*/
}

/**
* @brief QcdmWindow::efsExtractFactoryImage
*/
void QcdmWindow::efsExtractFactoryImage()
{	
	
	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	std::vector<uint8_t> data;

	if (efsManager.factoryImagePrepare() == efsManager.kDmEfsSuccess) {
		
		if (efsManager.factoryImageStart() == efsManager.kDmEfsSuccess) {
			
			if (efsManager.factoryImageRead(data) == efsManager.kDmEfsSuccess) {
				
			} else {
				log(kLogTypeError, "Error Reading");
			}

		} else {
			log(kLogTypeError, "Error Starting");
		}

	} else {
		log(kLogTypeError, "Error Preparing");
	}


}
void QcdmWindow::efsMakeGoldenCopy()
{
	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}
	
	if (efsManager.makeGoldenCopy("/") != efsManager.kDmEfsSuccess) {
		log(kLogTypeError, "Error Making Golden Copy Request");
		return;
	}

	log(kLogTypeInfo, "Golden Copy Generating. Device should restart upon successfuly generation.");
}

void QcdmWindow::efsFilesystemImage()
{
	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	int32_t handle;
	if (efsManager.openFilesystemImage("/", kDiagEfsFsTypeTar, handle) != efsManager.kDmEfsSuccess) {
		log(kLogTypeError, "Error Opening For Filesystem Image Request");
		return;
	}

	log(kLogTypeError, "Operation Success. Unimplemented functionality required to continue, stopping.");
}

void QcdmWindow::efsGetFileInfo()
{

	QString tmp;
	QString md5;
	QString path;
	int i;
	QcdmEfsStatResponse statResponse = {};
	QcdmEfsMd5SumResponse md5Response = {};

	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	path = ui->efsFilePathValue->text();

	if (!path.length()) {
		log(kLogTypeError, "Please enter a path");
		return;
	}


	log(kLogTypeInfo, "---------------------------");
	log(kLogTypeInfo, tmp.sprintf("Info For File: %s", path.toStdString().c_str()));
	log(kLogTypeInfo, "---------------------------");

	try {
		statResponse = efsManager.stat(path.toStdString().c_str());
	} catch (std::exception e) {
		log(kLogTypeError, "Stat Error");
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("Size: %lu", statResponse.size));
	log(kLogTypeInfo, tmp.sprintf("Mode: %08X", statResponse.mode));
	log(kLogTypeInfo, tmp.sprintf("Link Count: %d", statResponse.linkCount));
	log(kLogTypeInfo, tmp.sprintf("Last Access: %lu", statResponse.atime));
	log(kLogTypeInfo, tmp.sprintf("Last Modified: %lu", statResponse.mtime));
	log(kLogTypeInfo, tmp.sprintf("Creation Time: %lu", statResponse.ctime));

	try {
		md5Response = efsManager.md5sum(path.toStdString().c_str());
	} catch (std::exception e) {
		log(kLogTypeError, "Stat Error");
		return;
	}
	if (md5Response.error) {
		log(kLogTypeError, "MD5 Error");
	} else {

		for (i = 0; i < 16; i++) {
			md5.append(tmp.sprintf("%02X", md5Response.hash[i]));
		}

		log(kLogTypeInfo, tmp.sprintf("MD5: %s", md5.toStdString().c_str()));
	}
	
	log(kLogTypeInfo, "---------------------------");
}

void QcdmWindow::efsGetFile()
{
	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	QString tmp;
	QString path = ui->efsFilePathValue->text();

	if (!path.length()) {
		log(kLogTypeError, "Please enter a path");
		return;
	}
		
	QStringList pathParts = path.split("/");
	QString pathFileName = pathParts.value(pathParts.length() - 1);

	QString outFile = QFileDialog::getSaveFileName(this, tr("Save File"), pathFileName, tr("*.*"));

	if (!outFile.length()) {
		log(kLogTypeInfo, "Operation Cancelled");
		return;
	}

	if (efsManager.read(path.toStdString(), outFile.toStdString()) != efsManager.kDmEfsSuccess) {
		log(kLogTypeError, tmp.sprintf("Error reading file %s", path.toStdString().c_str()));
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("File %s saved to %s", path.toStdString().c_str(), outFile.toStdString().c_str()));
}

void QcdmWindow::efsDeleteFile()
{
	if (!port.isOpen()) {
		log(kLogTypeError, "Connect to a port first");
		return;
	}

	QString tmp;
	QString path = ui->efsFilePathValue->text();

	if (!path.length()) {
		log(kLogTypeError, "Please enter a path");
		return;
	}

	try {
		efsManager.unlink(path.toStdString());
	} catch (std::exception e) {
		log(kLogTypeError, tmp.sprintf("Error removing file %s: %s", path.toStdString().c_str(), e.what()));
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("File %s deleted", path.toStdString().c_str()));
}

void QcdmWindow::EfsAddTreeNodes(QTreeWidgetItem* parent, std::vector<DmEfsNode>& entries)
{
	QString tmp;

	for (auto entry : entries) {
		QTreeWidgetItem *treeNode = new QTreeWidgetItem(parent);
		
		treeNode->setText(kEfsBrowserColumnName, entry.name.c_str());
			
		if (entry.isFile()) {
			treeNode->setText(kEfsBrowserColumnType, tr("File"));
			treeNode->setIcon(kEfsBrowserColumnName, QIcon(entry.error ? ":/images/file-protected-2x.png" : ":/images/file-2x.png"));
		} else if (entry.isDir()) {
			treeNode->setText(kEfsBrowserColumnType, tr("Directory"));
			treeNode->setIcon(kEfsBrowserColumnName, QIcon(entry.error ? ":/images/folder-protected-2x.png" : ":/images/folder-2x.png"));
		} else if (entry.isLink()) {
			treeNode->setText(kEfsBrowserColumnType, tr("Link"));
		} else if (entry.isImmovable()) {
			treeNode->setText(kEfsBrowserColumnType, tr("Immovable"));
		} else {
			treeNode->setText(kEfsBrowserColumnType, tr("Unknown"));
		}

		treeNode->setText(kEfsBrowserColumnSize, tmp.sprintf("%lu", entry.size));
		treeNode->setText(kEfsBrowserColumnMode, tmp.sprintf("%08X", entry.mode));
		treeNode->setText(kEfsBrowserColumnATime, tmp.sprintf("%lu", entry.atime));
		treeNode->setText(kEfsBrowserColumnMTime, tmp.sprintf("%lu", entry.mtime));
		treeNode->setText(kEfsBrowserColumnCTime, tmp.sprintf("%lu", entry.ctime));

		if (entry.path.compare("/") == 0) {
			treeNode->setText(kEfsBrowserColumnFullPath, tmp.sprintf("/%s", entry.name.c_str()));
		} else {
			treeNode->setText(kEfsBrowserColumnFullPath, tmp.sprintf("%s%s", entry.path.c_str(), entry.name.c_str()));
		}

		if (entry.children.size()) {
			EfsAddTreeNodes(treeNode, entry.children);
		}
	}
}

void QcdmWindow::onEfsSubsysCommandCodeChange()
{
	if (!ui->efsSubsysCommandValue->isModified()) {
		return;
	}

	uint32_t code = std::stoul(ui->efsSubsysCommandValue->text().toStdString().c_str(), nullptr, 10);

	efsManager.setSubsystemCommand(code);

	log(kLogTypeInfo, "Subsystem Command Code Changed");
}

void QcdmWindow::onEfsSubsysIdChange()
{
	if (!ui->efsSubsysIdValue->isModified()) {
		return;
	}

	uint32_t id = std::stoul(ui->efsSubsysIdValue->text().toStdString().c_str(), nullptr, 10);

	efsManager.setSubsystemId(id);

	log(kLogTypeInfo, "Subsystem ID Changed");

}

void QcdmWindow::About()
{
	aboutWindow = new AboutDialog();
	
	aboutWindow->show();

	//AboutDialog dialog(this);
	//dialog.exec(); 
}

/**
* @brief QcdmWindow::clearLog
*/
void QcdmWindow::clearLog() {
    ui->log->clear();
}

/**
* @brief QcdmWindow::saveLog
*/
void QcdmWindow::saveLog() {
    log(kLogTypeWarning, "Not Implemented Yet");
}


/**
* @brief QcdmWindow::log
* @param message
*/
void QcdmWindow::log(int type, const char* message) {
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
* @brief QcdmWindow::log
* @param message
*/
void QcdmWindow::log(int type, std::string message) 
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
* @brief QcdmWindow::log
* @param type
* @param message
*/
void QcdmWindow::log(int type, QString message) 
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

void QcdmWindow::onEfsTreeNodeRightClick(const QPoint& point)
{

	if (point.isNull()) {
		return;
	}

	QTreeWidgetItem* node = ui->efsDirectoryTree->itemAt(point);

	QMenu menu(this);

	if (node != nullptr) {
		if (node->text(kEfsBrowserColumnType).compare("file", Qt::CaseInsensitive) == 0) {
			menu.addAction(QIcon(":/images/data-transfer-download-2x.png"), "Save file", this, SLOT(efsContextMenuSaveFile()))
				->setData(node->text(kEfsBrowserColumnFullPath));
		
			menu.addAction(QIcon(":/images/trash-2.png"), "Delete", this, SLOT(efsContextMenuDeleteFile()))
				->setData(node->text(kEfsBrowserColumnFullPath));

			menu.addAction(QIcon(":/images/clipboard-2.png"), "Copy path to clipboard", this, SLOT(efsContextMenuCopyPathToClipboard()))
				->setData(node->text(kEfsBrowserColumnFullPath));

			menu.exec(ui->efsDirectoryTree->mapToGlobal(point));

		} else if (node->text(kEfsBrowserColumnType).compare("directory", Qt::CaseInsensitive)== 0) {
						
			menu.addAction(QIcon(":/images/data-transfer-download-2x.png"), "Save directory", this, SLOT(efsContextMenuSaveDirectory()))
				->setData(node->text(kEfsBrowserColumnFullPath));

			menu.addAction(QIcon(":/images/data-transfer-download-2x.png"), "Save directory as compressed zip", this, SLOT(efsContextMenuSaveDirectoryCompressed()))
				->setData(node->text(kEfsBrowserColumnFullPath));

			menu.addAction(QIcon(":/images/data-transfer-upload-2x.png"), "Upload file", this, SLOT(efsContextMenuUploadFile()))
				->setData(node->text(kEfsBrowserColumnFullPath));
			
			menu.addAction(QIcon(":/images/plus-2x.png"), "Create Directory", this, SLOT(efsContextMenuCreateDirectory()))
				->setData(node->text(kEfsBrowserColumnFullPath));
			
			menu.addAction(QIcon(":/images/link-2x.png"), "Create Link", this, SLOT(efsContextMenuCreateLink()))
				->setData(node->text(kEfsBrowserColumnFullPath));

			menu.addAction(QIcon(":/images/clipboard-2.png"), "Copy path to clipboard", this, SLOT(efsContextMenuCopyPathToClipboard()))
				->setData(node->text(kEfsBrowserColumnFullPath));

			menu.addAction(QIcon(":/images/trash-2x.png"), "Delete", this, SLOT(efsContextMenuDeleteDirectory()))
				->setData(node->text(kEfsBrowserColumnFullPath));

			menu.exec(ui->efsDirectoryTree->mapToGlobal(point));
		}
	}
	
}

void QcdmWindow::efsContextMenuSaveFile()
{
	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (!path.length()) {
		log(kLogTypeError, "Invalid Path");
		return;
	}

	QStringList pathParts = path.split("/");
	QString filename = pathParts.value(pathParts.length() - 1);

	QString outPath = QFileDialog::getSaveFileName(this, tr("Save File"), filename, tr("*.*"));

	if (!outPath.length()) {
		log(kLogTypeInfo, "Operation Cancelled");
		return;
	}

	if (efsManager.read(path.toStdString(), outPath.toStdString()) != efsManager.kDmEfsSuccess) {
		log(kLogTypeError, "Error reading file");
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("File %s saved to %s", path.toStdString().c_str(), outPath.toStdString().c_str()));

}

void QcdmWindow::efsContextMenuSaveDirectory()
{
	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (!path.length()) {
		log(kLogTypeError, "Invalid path");
		return;
	}

}

void QcdmWindow::efsContextMenuSaveDirectoryCompressed()
{
	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (!path.length()) {
		log(kLogTypeError, "Invalid path");
		return;
	}


}

void QcdmWindow::efsContextMenuUploadFile()
{
	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (!path.length()) {
		log(kLogTypeError, "Invalid path");
		return;
	}
}

void QcdmWindow::efsContextMenuCopyPathToClipboard()
{
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (path.length()) {
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(path);
		log(kLogTypeInfo, "Path copied to clipboard");
	}

}


void QcdmWindow::efsContextMenuDeleteDirectory()
{
	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (!path.length()) {
		log(kLogTypeError, "Invalid Path");
		return;
	}

	QMessageBox::StandardButton confirmDelete = QMessageBox::question(this, tr("Confirm Directory Deletion"), tmp.sprintf("Really remove directory %s and all its contents?", path.toStdString().c_str()));

	if (confirmDelete != QMessageBox::Yes) {
		return;
	}

	try {
		efsManager.deltree(path.toStdString());
	} catch (std::exception e) {
		log(kLogTypeError, tmp.sprintf("Error deleting %s: %s", path.toStdString().c_str(), e.what()));
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("Directory %s deleted", path.toStdString().c_str()));

}


void QcdmWindow::efsContextMenuDeleteFile()
{

	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (!path.length()) {
		log(kLogTypeError, "Invalid Path");
		return;
	}

	QMessageBox::StandardButton confirmDelete = QMessageBox::question(this, tr("Confirm File Deletion"), tmp.sprintf("Really delete %s?", path.toStdString().c_str()));

	if (confirmDelete != QMessageBox::Yes) {
		return;
	}

	try {
		efsManager.unlink(path.toStdString());
	} catch (std::exception e) {
		log(kLogTypeError, tmp.sprintf("Error deleting %s: %s", path.toStdString().c_str(), e.what()));
		return;
	}

	log(kLogTypeInfo, tmp.sprintf("File %s deleted", path.toStdString().c_str()));
}


void QcdmWindow::efsContextMenuCreateDirectory()
{

	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString basePath = sendingAction->data().toString();
	QString path;

	if (!basePath.length()) {
		log(kLogTypeError, "Invalid Path");
		return;
	}

	QString directoryName;
	int mode;
	bool collectedName;
	bool collectedMode;
	
	directoryName = QInputDialog::getText(this, tr("Directory Name"), tr("Directory name:"), QLineEdit::Normal, NULL, &collectedName);
	
	if (collectedName && !directoryName.isEmpty()) {
		
		mode = QInputDialog::getInt(this, tr("Directory Mode"), tr("Mode:"), 644, 0, 1000, 1, &collectedMode);
		
		if (collectedMode && mode > 0) {
			path.sprintf("%s%s", basePath.toStdString().c_str(), directoryName.toStdString().c_str());

			try {
				efsManager.mkdir(path.toStdString(), mode);
			} catch (std::exception e) {
				log(kLogTypeError, tmp.sprintf("Error creating directory %s: %s", path.toStdString().c_str(), e.what()));
				return;
			}

			log(kLogTypeInfo, tmp.sprintf("Directory %s created", path.toStdString().c_str()));
		}
	}		
}


void QcdmWindow::efsContextMenuCreateLink()
{
	QString tmp;
	QAction* sendingAction = (QAction*)sender();
	QString path = sendingAction->data().toString();

	if (!path.length()) {
		log(kLogTypeError, "Invalid Path");
		return;
	}

	bool collectedTarget, collectedName;

	QString targetPath = QInputDialog::getText(this, tr("Target Path"), tr("Target Path:"), QLineEdit::Normal, NULL, &collectedTarget);

	if (collectedTarget && !targetPath.isEmpty()) {
		QString name = QInputDialog::getText(this, tr("Link Name"), tr("Link name:"), QLineEdit::Normal, NULL, &collectedName);
		if (collectedName && !name.isEmpty()) {
			path.append(name);

			try {
				efsManager.symlink(targetPath.toStdString(), path.toStdString());
			} catch (std::exception e) {
				log(kLogTypeError, tmp.sprintf("Error creating link from %s to %s: %s", path.toStdString().c_str(), targetPath.toStdString().c_str(), e.what()));
				return;
			}

			log(kLogTypeInfo, tmp.sprintf("Created link from %s to %s", path.toStdString().c_str(), targetPath.toStdString().c_str()));
		}
	}

}


void QcdmWindow::probeCommands()
{

	//port.getLog();


	/*QcdmVersionResponse version = {};
	
	port.getVersion(version);

	printf("Version: %d\n", version.version);
	*/

	/*
	QcdmQueryQpstCookieRequest r = {};
	r.header.command = kDiagSubsysCmd;
	r.header.subsysId = kDiagSubsysDiagServ;
	r.header.subsysCommand = 0x0054;
	r.version = 1;
	*/
	QcdmTestStruct r = {};
	r.header.command = kDiagSubsysCmd;
	r.header.subsysId = kDiagSubsysDiagServ;
	r.header.subsysCommand = 0x080A;
	r.processorId = 0x00;

	port.write((uint8_t*)&r, sizeof(r));

	uint8_t b[DIAG_MAX_PACKET_SIZE];
	port.read(b, DIAG_MAX_PACKET_SIZE);

	return;


	QString tmp;
	uint8_t buf[DIAG_MAX_PACKET_SIZE];

	QcdmGenericRequest packet = {};

	size_t rxSize, txSize;

	for (int i = 0; i <= kDiagMax; i++) {
		packet.command = i;

		if (i == kDiagDload) {
			continue;
		}
		
		
		
		try {
			txSize = port.write(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
			
			if (!txSize) {
				continue;
			}

			rxSize = port.read(buf, DIAG_MAX_PACKET_SIZE);

			if (!rxSize) {
				continue;
			}

		}
		catch (...) {
			log(kLogTypeError, "Encountered Exception, Possible Reset");
			return;
		}

		tmp.sprintf("Command %d - 0x%02X - Response %d - 0x%02X - Resp Size: %lu\n", i, i, buf[0], buf[0], rxSize);

		if (buf[0] == i) {
			printf(tmp.toStdString().c_str());
			log(kLogTypeInfo, tmp);
			tmp.clear();
			hexdump(buf, rxSize, tmp, true);
			tmp.append("<br />");
			log(kLogTypeDebug, tmp);
		}
		else if (buf[0] != kDiagBadCmd) {
			tmp.prepend("ERROR - ");
			log(kLogTypeError, tmp);
			if (rxSize > 2) {
				tmp.clear();
				hexdump(buf, rxSize, tmp, true);
				tmp.append("<br />");
				log(kLogTypeDebug, tmp);

			}
		}

	}
}

void QcdmWindow::readSome()
{
	uint8_t buf[DIAG_MAX_PACKET_SIZE];

	size_t readSize = 1;

	while (readSize > 0) {
		readSize = port.read(buf, DIAG_MAX_PACKET_SIZE, false);
		hexdump(buf, readSize);
		printf("\n");
	}
}

void QcdmWindow::sendCommand()
{
	uint8_t command = std::stoul(ui->sendCommandValue->text().toStdString().c_str(), nullptr, 16);

	QString tmp;
	uint8_t buf[DIAG_MAX_PACKET_SIZE];

	QcdmGenericRequest packet = { command };
	size_t txSize = port.write(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
	hexdump(reinterpret_cast<uint8_t*>(&packet), txSize);

	port.read(buf, DIAG_MAX_PACKET_SIZE);
}
