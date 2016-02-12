/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_window.cpp
* @class StreamingDloadWindow
* @package OpenPST
* @brief Streaming DLOAD GUI interface class
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#include "streaming_dload_window.h"

using namespace OpenPST::GUI;

StreamingDloadWindow::StreamingDloadWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::StreamingDloadWindow),
	port("", 115200),
	readWorker(nullptr),
	streamWriteWorker(nullptr)
{
	ui->setupUi(this);
	 
	this->statusBar()->setSizeGripEnabled(false);
	
	ui->securityModeValue->addItem("0x01 - Trusted", kStreamingDloadSecurityMode_TRUSTED);
	ui->securityModeValue->addItem("0x00 - Untrusted", kStreamingDloadSecurityMode_UNTRUSTED);
	ui->securityModeValue->setCurrentIndex(0);
	
	ui->openModeValue->addItem("0x01 - Bootloader Download", kStreamingDloadOpenModeBootloader);
	ui->openModeValue->addItem("0x02 - Bootable Image Download", kStreamingDloadOpenModeBootableImage);
	ui->openModeValue->addItem("0x03 - CEFS Image Download", kStreamingDloadOpenModeCefs);
	ui->openModeValue->addItem("0x04 - Factory", kStreamingDloadOpenModeFactory);
	ui->openModeValue->setCurrentIndex(0);

	ui->eccSetValue->addItem("0x00 - Disable", 0x00);
	ui->eccSetValue->addItem("0x01 - Enable", 0x01);
	ui->eccSetValue->setCurrentIndex(0);

	ui->openMultiValue->addItem("0x01 - PBL", kStreamingDloadOpenModeMultiPbl);
	ui->openMultiValue->addItem("0x02 - QC SBL Header & Config", kStreamingDloadOpenModeMultiQcsblhHdCfg);
	ui->openMultiValue->addItem("0x03 - QC SBL", kStreamingDloadOpenModeMultiQcsbl);
	ui->openMultiValue->addItem("0x04 - OEM SBL", kStreamingDloadOpenModeMultiOemsbl);
	ui->openMultiValue->addItem("0x05 - AMSS", kStreamingDloadOpenModeMultiAmss);
	ui->openMultiValue->addItem("0x06 - APPS", kStreamingDloadOpenModeMultiApps);
	ui->openMultiValue->addItem("0x07 - OBL - MSM6250", kStreamingDloadOpenModeMultiObl);
	ui->openMultiValue->addItem("0x08 - FOTA UI", kStreamingDloadOpenModeMultiFotaui);
	ui->openMultiValue->addItem("0x09 - CEFS", kStreamingDloadOpenModeMultiCefs);
	ui->openMultiValue->addItem("0x0A - AMSS applications boot loader", kStreamingDloadOpenModeMultiApps_CEFS);
	ui->openMultiValue->addItem("0x0B - Apps CEFS", kStreamingDloadOpenModeMultiFlashBin);
	ui->openMultiValue->addItem("0x0C - Flash.bin for Windows Mobile", kStreamingDloadOpenModeMultiDsp1);
	ui->openMultiValue->addItem("0x0D - DSP1 runtime image", kStreamingDloadOpenModeMultiDsp1);
	ui->openMultiValue->addItem("0x0E - User defined partition", kStreamingDloadOpenModeMultiCustom);
	ui->openMultiValue->addItem("0x0F - DBL - Secure Boot 2.0", kStreamingDloadOpenModeMultiDbl);
	ui->openMultiValue->addItem("0x10 - OSBL - Secure Boot 2.0", kStreamingDloadOpenModeMultiOsbl);
	ui->openMultiValue->addItem("0x11 - FSBL - Secure Boot 2.0", kStreamingDloadOpenModeMultiFsbl);
	ui->openMultiValue->addItem("0x12 - DSP2 executable ", kStreamingDloadOpenModeMultiDsp2);
	ui->openMultiValue->addItem("0x13 - Apps EFS2 raw image ", kStreamingDloadOpenModeMultiRaw);
	ui->openMultiValue->addItem("0x14 - ROFS1 - Symbian", kStreamingDloadOpenModeMultiRofs1);
	ui->openMultiValue->addItem("0x15 - ROFS2 - Symbian", kStreamingDloadOpenModeMultiRofs2);
	ui->openMultiValue->addItem("0x16 - ROFS3 - Symbian", kStreamingDloadOpenModeMultiRofs3);
	ui->openMultiValue->addItem("0x21 - EMMC USER partition ", kStreamingDloadOpenModeMultiEmmcUser);
	ui->openMultiValue->addItem("0x22 - EMMC BOOT0 partition ", kStreamingDloadOpenModeMultiEmmcBoot0);
	ui->openMultiValue->addItem("0x23 - EMMC BOOT1 partition ", kStreamingDloadOpenModeMultiEmmcBoot1);
	ui->openMultiValue->addItem("0x24 - EMMC RPMB", kStreamingDloadOpenModeMultiRpmb);
	ui->openMultiValue->addItem("0x25 - EMMC GPP1", kStreamingDloadOpenModeMultiGpp1);
	ui->openMultiValue->addItem("0x26 - EMMC GPP2", kStreamingDloadOpenModeMultiGpp2);
	ui->openMultiValue->addItem("0x27 - EMMC GPP3", kStreamingDloadOpenModeMultiGpp3);
	ui->openMultiValue->addItem("0x28 - EMMC GPP4", kStreamingDloadOpenModeMultiGpp4);
	ui->openMultiValue->setCurrentIndex(0);

	QObject::connect(ui->portRefreshButton, SIGNAL(clicked()), this, SLOT(updatePortList()));
	QObject::connect(ui->portDisconnectButton, SIGNAL(clicked()), this, SLOT(disconnectPort()));
	QObject::connect(ui->portConnectButton, SIGNAL(clicked()), this, SLOT(connectToPort()));	
	QObject::connect(ui->helloButton, SIGNAL(clicked()), this, SLOT(sendHello()));	
	QObject::connect(ui->unlockButton, SIGNAL(clicked()), this, SLOT(sendUnlock()));
	QObject::connect(ui->securityModeButton, SIGNAL(clicked()), this, SLOT(setSecurityMode()));
	QObject::connect(ui->nopButton, SIGNAL(clicked()), this, SLOT(sendNop()));
	QObject::connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(sendReset()));
	QObject::connect(ui->powerDownButton, SIGNAL(clicked()), this, SLOT(sendPowerDown()));
	QObject::connect(ui->eccReadButton, SIGNAL(clicked()), this, SLOT(readEccState()));
	QObject::connect(ui->eccSetButton, SIGNAL(clicked()), this, SLOT(setEccState()));
	QObject::connect(ui->openModeButton, SIGNAL(clicked()), this, SLOT(openMode()));
	QObject::connect(ui->openMultiButton, SIGNAL(clicked()), this, SLOT(openMultiMode()));
	QObject::connect(ui->closeModeButton, SIGNAL(clicked()), this, SLOT(closeMode()));
	QObject::connect(ui->openMultiCloseButton, SIGNAL(clicked()), this, SLOT(closeMode()));
	QObject::connect(ui->readButton, SIGNAL(clicked()), this, SLOT(read()));
	QObject::connect(ui->writePartitionTableButton, SIGNAL(clicked()), this, SLOT(writePartitionTable()));
	QObject::connect(ui->cancelOperationButton, SIGNAL(clicked()), this, SLOT(cancelOperation()));
	QObject::connect(ui->clearLogButton, SIGNAL(clicked()), this, SLOT(clearLog()));
	QObject::connect(ui->writePartitionTableFileBrowseButton, SIGNAL(clicked()), this, SLOT(browseForParitionTable()));
	QObject::connect(ui->writeFileBrowseButton, SIGNAL(clicked()), this, SLOT(browseForWriteFile()));
	QObject::connect(ui->streamWriteButton, SIGNAL(clicked()), this, SLOT(streamWrite()));

	qRegisterMetaType<StreamingDloadReadWorkerRequest>("StreamingDloadReadWorkerRequest");
	qRegisterMetaType<StreamingDloadStreamWriteWorkerRequest>("StreamingDloadStreamWriteWorkerRequest");
	
	updatePortList();
}

StreamingDloadWindow::~StreamingDloadWindow()
{
	if (port.isOpen()) {
		port.close();
	}

	this->close();
	delete ui;
}

/**
* @brief StreamingDloadWindow::updatePortList
*/
void StreamingDloadWindow::updatePortList()
{
	if (port.isOpen()) {
		log("Port is currently open");
		return;
	}

	std::vector<serial::PortInfo> devices = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter = devices.begin();

	ui->portList->clear();
	ui->portList->addItem("- Select a Port -");

	QString tmp;

	log(tmp.sprintf("Found %d devices", devices.size()));

	while (iter != devices.end()) {
		serial::PortInfo device = *iter++;

		log(tmp.sprintf("%s %s %s",
			device.port.c_str(),
			device.hardware_id.c_str(),
			device.description.c_str()
		));

		ui->portList->addItem(tmp, device.port.c_str());
	}
}
/**
* @brief StreamingDloadWindow::connectToPort
*/
void StreamingDloadWindow::connectToPort()
{
	QString selected = ui->portList->currentData().toString();
	QString tmp;

	if (selected.compare("0") == 0) {
		log("Select a Port First");
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
		log("Invalid Port Type");
		return;
	}

	try {
		port.setPort(currentPort.port);

		if (!port.isOpen()){
			port.open();
		}
	} catch (serial::IOException e) {
		log(tmp.sprintf("Error Connecting To Port %s", currentPort.port.c_str()));
		log(e.getErrorNumber() == 13 ? "Permission Denied. Try Running With Elevated Privledges." : e.what());
		return;
	}

	log(tmp.sprintf("Connected to %s", currentPort.port.c_str()));
}

/**
* @brief StreamingDloadWindow::disconnectPort
*/
void StreamingDloadWindow::disconnectPort()
{
	if (port.isOpen()) {
		port.close();
		log("Port Closed");
	}
}

/**
* @brief StreamingDloadWindow::sendHello
*/
void StreamingDloadWindow::sendHello()
{

	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	if (!ui->helloMagicValue->text().length()) {
		log("Magic is required\n");
		return;
	}

	if (!ui->helloVersionValue->text().length()) {
		log("Version is required\n");
		return;
	}

	if (!ui->helloCompatibleVersionValue->text().length()) {
		log("Compatible version is required\n");
		return;
	}

	if (!ui->helloFeatureBitsValue->text().length()) {
		log("Feature bits are required\n");
		return;
	}
	
	QString tmp;
	
	std::string magic = ui->helloMagicValue->text().toStdString().c_str();
	uint8_t version = std::stoul(ui->helloVersionValue->text().toStdString().c_str(), nullptr, 16);
	uint8_t compatibleVersion = std::stoul(ui->helloCompatibleVersionValue->text().toStdString().c_str(), nullptr, 16);
	uint8_t featureBits = std::stoul(ui->helloFeatureBitsValue->text().toStdString().c_str(), nullptr, 16);
	
	if (!port.sendHello(magic, version, compatibleVersion, featureBits)) {
		log("Error Sending Hello");
		return;
	}	
	
	log(tmp.sprintf("Hello Response: %02X", port.state.hello.command));
	log(tmp.sprintf("Magic: %s", port.state.hello.magic));
	log(tmp.sprintf("Version: %02X", port.state.hello.version));
	log(tmp.sprintf("Compatible Version: %02X", port.state.hello.compatibleVersion));
	log(tmp.sprintf("Max Prefered Block Size: %d", port.state.hello.maxPreferredBlockSize));
	log(tmp.sprintf("Base Flash Address: 0x%08X", port.state.hello.baseFlashAddress));
	log(tmp.sprintf("Flash ID Length: %d", port.state.hello.flashIdLength));	
	log(tmp.sprintf("Flash Identifier: %s", port.state.hello.flashIdenfier));
	log(tmp.sprintf("Window Size: %d", port.state.hello.windowSize));
	log(tmp.sprintf("Number of Sectors: %d", port.state.hello.numberOfSectors));
	
	
	// dump all sector sizes
	for (int i = 0; i < port.state.hello.numberOfSectors; i++) {
		log(tmp.sprintf("Sector %d: %d", i, port.state.hello.sectorSizes[i*4]));
	}

	log(tmp.sprintf("Feature Bits: %04X", port.state.hello.featureBits));
}

/**
* @brief StreamingDloadWindow::setSecurityMode
*/
void StreamingDloadWindow::setSecurityMode()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}
	
	QString tmp;
	uint8_t mode = ui->securityModeValue->currentData().toUInt();

	if (!port.setSecurityMode(mode)) {
		log(tmp.sprintf("Error setting security mode to %s", mode == 0x00 ? "Untrusted" : "Trusted" ));
		return;
	}

	log(tmp.sprintf("Setting security mode set to %s", mode == 0x00 ? "Untrusted" : "Trusted"));
}

/**
* @brief StreamingDloadWindow::sendUnlock
*/
void StreamingDloadWindow::sendUnlock()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	if (!ui->unlockCodeValue->text().length()) {
		ui->unlockCodeValue->setText("0000");
	}

	if (!port.sendUnlock(ui->unlockCodeValue->text().toStdString())) {
		log("Error Sending Unlock");
		return;
	}
}


/**
* @brief sendNop
*/
void StreamingDloadWindow::sendNop()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	if (!port.sendNop()) {
		log("Error Sending NOP");
		return;
	}

	log("NOP Success");
}

/**
* @brief sendReset
*/
void StreamingDloadWindow::sendReset()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	if (!port.sendReset()) {
		log("Error Sending Reset");
		return;
	}

	log("Device Resetting");
	port.close();
}

/**
* @brief sendPowerDown
*/
void StreamingDloadWindow::sendPowerDown()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	if (!port.sendPowerOff()) {
		log("Error Sending Power Down");
		return;
	}

	log("Device Powering Down");
	port.close();
}

/**
* @brief closeMode
*/
void StreamingDloadWindow::openMode()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	QString tmp;
	uint8_t mode = ui->openModeValue->currentData().toUInt();

	if (!port.openMode(mode)) {
		log(tmp.sprintf("Error Opening Mode %s", port.getNamedOpenMode(mode)));
		return;
	}

	log(tmp.sprintf("Opened Mode %s", port.getNamedOpenMode(mode)));
}


/**
* @brief closeMode
*/
void StreamingDloadWindow::closeMode()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	if (!port.closeMode()) {
		log("Error Closing Mode");
		return;
	}

	log("Mode Closed");
}

/**
* @brief readEccState
*/
void StreamingDloadWindow::readEccState()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	uint8_t status;

	if (!port.readEcc(status)) {
		log("Error Reading ECC");
		return;
	}

	if (status == 0x01) {
		log("ECC Enabled");
	} else if (status == 0x00) {
		log("ECC Disabled");
	} else {
		log("Unknown ECC State");
	}

	// set the ecc set choice box value to the matching
	// state
	int choiceIdx = ui->eccSetValue->findData(status);
	if (choiceIdx) {
		ui->eccSetValue->setCurrentIndex(1);
	}
}

/**
* @brief setEccState
*/
void StreamingDloadWindow::setEccState()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	uint8_t state = ui->eccSetValue->currentData().toUInt();
	if (!port.setEcc(state)) {
		log("Error Setting ECC State");
		return;
	}

	if (state == 0x00) {
		log("ECC Disabled");
	} else if (state == 0x01) {
		log("ECC Enabled");
	}
}


/**
* @brief StreamingDloadWindow::read - Read address and size
*/
void StreamingDloadWindow::read()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	uint32_t address = std::stoul(ui->readAddressValue->text().toStdString().c_str(), nullptr, 16);
	size_t	 size = std::stoi(ui->readSizeValue->text().toStdString().c_str(), nullptr, 10);
	size_t	 stepSize = std::stoi(ui->readStepSizeValue->currentText().toStdString().c_str(), nullptr, 10);

	if (size <= 0) {
		log("Enter a valid size to read");
		return;
	}

	QString tmp;
	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Read Data"), "", tr("Binary Files (*.bin)"));


	if (!fileName.length()) {
		log("Read operation cancelled");
		return;
	}

	log(tmp.sprintf("Attempting Read %lu bytes starting from address 0x%08X to file %s", size, address, fileName.toStdString().c_str()));

	StreamingDloadReadWorkerRequest request;	
	request.address = address;
	request.size = size;
	request.stepSize = stepSize;
	request.outFilePath = fileName.toStdString();

	// setup progress bar
	ui->progressBar->reset();
	ui->progressBar->setMaximum(request.size);
	ui->progressBar->setMinimum(0);
	ui->progressBar->setValue(0);

	ui->progressBarTextLabel2->setText(tmp.sprintf("%s", request.outFilePath.c_str()));
	ui->progressBarTextLabel->setText(tmp.sprintf("0 / %lu bytes", request.size));

	disableControls();

	readWorker = new StreamingDloadReadWorker(port, request, this);
	connect(readWorker, &StreamingDloadReadWorker::chunkReady, this, &StreamingDloadWindow::readChunkReadyHandler, Qt::QueuedConnection);
	connect(readWorker, &StreamingDloadReadWorker::complete, this, &StreamingDloadWindow::readCompleteHandler);
	connect(readWorker, &StreamingDloadReadWorker::error, this, &StreamingDloadWindow::readChunkErrorHandler);
	connect(readWorker, &StreamingDloadReadWorker::finished, readWorker, &QObject::deleteLater);

	readWorker->start();
}

/**
* @brief streamWrite - Stream write file starting at spcified address
*/
void StreamingDloadWindow::streamWrite()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	uint32_t address = std::stoul(ui->writeAddressValue->text().toStdString().c_str(), nullptr, 16);

	QString tmp;
	QString filePath = ui->writeFileValue->text();

	if (!filePath.length()) {
		log("No file specified");
		return;
	}

	std::ifstream file(filePath.toStdString(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		log("Error opening file for reading");
		return;
	}

	file.seekg(0, file.end);

	size_t fileSize = file.tellg();
	
	file.close();

	StreamingDloadStreamWriteWorkerRequest request;
	request.address = address;
	request.filePath = filePath.toStdString();
	request.unframed = ui->unframedWriteCheckbox->isChecked();

	// setup progress bar
	ui->progressBar->reset();
	ui->progressBar->setMaximum(fileSize);
	ui->progressBar->setMinimum(0);
	ui->progressBar->setValue(0);

	ui->progressBarTextLabel2->setText(tmp.sprintf("%s", request.filePath.c_str()));
	ui->progressBarTextLabel->setText(tmp.sprintf("0 / %lu bytes", fileSize));

	disableControls();

	streamWriteWorker = new StreamingDloadStreamWriteWorker(port, request, this);
	connect(streamWriteWorker, &StreamingDloadStreamWriteWorker::chunkComplete, this, &StreamingDloadWindow::streamWriteChunkCompleteHandler, Qt::QueuedConnection);
	connect(streamWriteWorker, &StreamingDloadStreamWriteWorker::complete, this, &StreamingDloadWindow::streamWriteCompleteHandler);
	connect(streamWriteWorker, &StreamingDloadStreamWriteWorker::error, this, &StreamingDloadWindow::streamWriteErrorHandler);
	connect(streamWriteWorker, &StreamingDloadStreamWriteWorker::finished, streamWriteWorker, &QObject::deleteLater);

	streamWriteWorker->start();

}

/**
* @brief StreamingDloadWindow::readChunkReadyHandler
*/
void StreamingDloadWindow::readChunkReadyHandler(StreamingDloadReadWorkerRequest request)
{
	// update progress bar
	QString tmp;
	ui->progressBar->setValue(request.outSize);
	ui->progressBarTextLabel->setText(tmp.sprintf("%lu / %lu bytes", ui->progressBar->value(), request.size));
}

/**
* @brief StreamingDloadWindow::readCompleteHandler
*/
void StreamingDloadWindow::readCompleteHandler(StreamingDloadReadWorkerRequest request)
{
	QString tmp;

	enableControls(); 
	
	log(tmp.sprintf("Read complete. Contents dumped to %s. Final size is %lu bytes", request.outFilePath.c_str(), request.outSize));

	readWorker = nullptr;
}

/**
* @brief StreamingDloadWindow::readChunkErrorHandler
*/
void StreamingDloadWindow::readChunkErrorHandler(StreamingDloadReadWorkerRequest request, QString msg)
{
	log(msg);

	enableControls();

	readWorker = nullptr;
}

/**
* @brief StreamingDloadWindow::openMultiMode
*/
void StreamingDloadWindow::openMultiMode()
{
	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	uint8_t imageType = ui->openMultiValue->currentData().toUInt();
	QString tmp;

	if (!port.openMultiImage(imageType)) {
		log(tmp.sprintf("Error opening multi image mode for %s", port.getNamedMultiImage(imageType)));
		return;
	}

	log(tmp.sprintf("Opening multi image mode for %s", port.getNamedMultiImage(imageType)));
	
}

/**
* @brief StreamingDloadWindow::browseForParitionTable
*/
void StreamingDloadWindow::browseForParitionTable()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Select Partition Image To Send", "", "Partition Image Files (*.bin)");

	if (fileName.length()) {
		ui->writePartitionTableFileValue->setText(fileName);
	}
}

/**
* @brief StreamingDloadWindow::writePartitionTable
*/
void StreamingDloadWindow::writePartitionTable()
{

	if (!port.isOpen()) {
		log("Port Not Open");
		return;
	}

	QString fileName = ui->writePartitionTableFileValue->text();

	if (!fileName.length()) {
		log("Enter Partition File Path");
		return;
	}

	bool overwrite = ui->writeParitionTableOverrideExistingCheckbox->isChecked();
	uint8_t status = NULL;
	QString tmp;

	if (!port.writePartitionTable(fileName.toStdString(), status, overwrite)) {
		if (status) {
			log(tmp.sprintf("Error sending partition write. Status Received: %02X", status));
		} else {
			log("Error sending partition write");
		}
		return;
	}
	
	log(tmp.sprintf("Partition Table Response: %02X", status));
}


void StreamingDloadWindow::eraseFlash()
{
	QMessageBox::StandardButton confirmation = QMessageBox::question(this, "DANGEROUS OPERATION", "If this operation fails, it can make the device inoperable and only able to be restored by JTAG. Continue?");

	if (confirmation == QMessageBox::Yes) {

	}

}


/**
* @brief StreamingDloadWindow::browseForWriteFile
*/
void StreamingDloadWindow::browseForWriteFile()
{

	QString fileName = QFileDialog::getOpenFileName(this, "Browse For File", "", "Image Files (*.bin, *.mbn)");

	if (fileName.length()) {
		ui->writeFileValue->setText(fileName);
	}
}

/**
* @brief StreamingDloadWindow::disableControls
*/
void StreamingDloadWindow::disableControls()
{
	ui->tabSet->setEnabled(false);
	ui->deviceContainer->setEnabled(false);
	ui->cancelOperationButton->setEnabled(true);
}

/**
* @brief StreamingDloadWindow::enableControls
*/
void StreamingDloadWindow::enableControls()
{
	ui->tabSet->setEnabled(true);
	ui->deviceContainer->setEnabled(true);
	ui->progressBarTextLabel2->setText("");
	ui->progressBarTextLabel->setText("");
	ui->progressBar->setValue(0);
	ui->cancelOperationButton->setEnabled(false);
}

/**
* @brief StreamingDloadWindow::cancelOperation
*/
void StreamingDloadWindow::cancelOperation()
{

	if (nullptr != readWorker && readWorker->isRunning()) {
		QMessageBox::StandardButton userResponse = QMessageBox::question(this, "Confirm", "Really cancel operation?");

		if (userResponse == QMessageBox::Yes) {
			readWorker->cancel();
			if (!readWorker->wait(5000)) {
				readWorker->terminate();
				readWorker->wait();
			}
			readWorker = nullptr;
			log("Read cancelled");
		}
	} else if (nullptr != streamWriteWorker && streamWriteWorker->isRunning()) {
		QMessageBox::StandardButton userResponse = QMessageBox::question(this, "Confirm", "Really cancel operation?");

		if (userResponse == QMessageBox::Yes) {
			streamWriteWorker->cancel();
			if (!streamWriteWorker->wait(5000)) {
				streamWriteWorker->terminate();
				streamWriteWorker->wait();
			}
			streamWriteWorker = nullptr;
			log("Read cancelled");
		}
	} else {
		log("No operation currently running");
	}

	enableControls();
}

/**
* @brief StreamingDloadWindow::streamWriteSegmentCompleteHandler
*/
void StreamingDloadWindow::streamWriteChunkCompleteHandler(StreamingDloadStreamWriteWorkerRequest request)
{
	// update progress bar
	QString tmp;
	ui->progressBar->setValue(request.outSize);
	ui->progressBarTextLabel->setText(tmp.sprintf("%lu / %lu bytes", ui->progressBar->value(), ui->progressBar->maximum()));
}

/**
* @brief StreamingDloadWindow::streamWriteCompleteHandler
*/
void StreamingDloadWindow::streamWriteCompleteHandler(StreamingDloadStreamWriteWorkerRequest request)
{
	QString tmp;

	enableControls(); 
	
	log(tmp.sprintf("Write complete"));

	streamWriteWorker = nullptr;
}

/**
* @brief StreamingDloadWindow::streamWriteErrorHandler
*/
void StreamingDloadWindow::streamWriteErrorHandler(StreamingDloadStreamWriteWorkerRequest request, QString msg)
{
	log(msg);

	enableControls();

	streamWriteWorker = nullptr;
}

/**
* @brief StreamingDloadWindow::clearLog
*/
void StreamingDloadWindow::clearLog()
{
	ui->log->clear();
}

/**
* @brief StreamingDloadWindow::saveLog
*/
void StreamingDloadWindow::saveLog()
{
	log("Not Implemented Yet");
}

/**
* @brief StreamingDloadWindow::log
* @param message
*/
void StreamingDloadWindow::log(const char* message)
{
	ui->log->appendPlainText(message);
}

/**
* @brief StreamingDloadWindow::log
* @param message
*/
void StreamingDloadWindow::log(QString message)
{
	ui->log->appendPlainText(message);
}
