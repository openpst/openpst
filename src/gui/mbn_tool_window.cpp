/**
* LICENSE PLACEHOLDER
*
* @file mbn_tool_window.cpp
* @class MbnToolWindow
* @package OpenPST
* @brief MBN Tool GUI interface class
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "mbn_tool_window.h"

using namespace OpenPST::GUI;

MbnToolWindow::MbnToolWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MbnToolWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->fileBrowseButton, SIGNAL(clicked()), this, SLOT(browseForFile()));
    QObject::connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(loadFile()));
	QObject::connect(ui->readCodeButton, SIGNAL(clicked()), this, SLOT(readCode()));
	QObject::connect(ui->readSignatureButton, SIGNAL(clicked()), this, SLOT(readSignature()));
	QObject::connect(ui->readx509ChainButton, SIGNAL(clicked()), this, SLOT(readX509Chain()));

}

MbnToolWindow::~MbnToolWindow()
{
    delete ui;
}


void MbnToolWindow::browseForFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image To Inspect", "", "Image Files (*.mbn *.bin *.img)");

    if (fileName.length()) {
        ui->filePathInput->setText(fileName);
    }
}

void MbnToolWindow::loadFile()
{
    QString tmp;
    QString fileName = ui->filePathInput->text();

    if (!fileName.length()) {
        log("Please Enter or Browse For a Image File");
        return;
    }

	std::ifstream file(fileName.toStdString(), std::ios::in | std::ios::binary);     

    if (!file.is_open()) {
        log("Error Opening File");
        return;
    }    

	file.seekg(0, file.end);
    size_t fileSize = file.tellg();
	file.seekg(0, file.beg);

	uint8_t headerBuffer[MBN_HEADER_MAX_SIZE];

	file.read((char*)headerBuffer, MBN_HEADER_MAX_SIZE);

	bool asDec = ui->displayDataAsSelect->currentText().compare("DEC");
	bool isEightyByte = true;

	MbnSblHeader* header;
	if (isEightyByteHeader(headerBuffer)) {
		header = (MbnSblHeader*)headerBuffer;
	} else if (isFourtyByteHeader(headerBuffer)) {
		MbnHeader* header = (MbnHeader*)headerBuffer;
		isEightyByte = false;
	} else {
		log("Not a valid MBN image");
		file.close();
		return;
	}

	file.close();

    log(tmp.sprintf("File Size: %lu bytes", fileSize));

	const char* format = asDec ? "%04X" : "%lu";

    if (ui->flipEndianCheckbox->isChecked()) {
        log(tmp.sprintf("Image Size: %lu bytes", flip_endian32(header->image_size)));
        log(tmp.sprintf("Code Size: %lu bytes", flip_endian32(header->code_size)));
        log(tmp.sprintf("Signature Size: %lu bytes", flip_endian32(header->signature_size)));
        log(tmp.sprintf("Cert Chain Size: %lu bytes", flip_endian32(header->cert_chain_size)));

		if (isEightyByte) {
			ui->mbnHeaderCodewordValue->setText(tmp.sprintf(format, flip_endian32(header->codeword)));
			ui->mbnHeaderMagicValue->setText(tmp.sprintf(format, flip_endian32(header->magic)));
			ui->mbnHeaderReserved1Value->setText(tmp.sprintf(format, flip_endian32(header->reserved1)));
			ui->mbnHeaderReserved2Value->setText(tmp.sprintf(format, flip_endian32(header->reserved2)));
			ui->mbnHeaderOemRootCertSelectionValue->setText(tmp.sprintf(format, flip_endian32(header->oem_root_cert_sel)));
			ui->mbnHeaderOemNumberRootCertsValue->setText(tmp.sprintf(format, flip_endian32(header->oem_num_root_certs)));
			ui->mbnHeaderReserved3Value->setText(tmp.sprintf(format, flip_endian32(header->reserved3)));
			ui->mbnHeaderReserved4Value->setText(tmp.sprintf(format, flip_endian32(header->reserved4)));
			ui->mbnHeaderReserved5Value->setText(tmp.sprintf(format, flip_endian32(header->reserved5)));
			ui->mbnHeaderReserved6Value->setText(tmp.sprintf(format, flip_endian32(header->reserved6)));
			ui->mbnHeaderReserved7Value->setText(tmp.sprintf(format, flip_endian32(header->reserved7)));
		}

        ui->mbnHeaderImageIdValue->setText(tmp.sprintf(format, flip_endian32(header->image_id)));
        ui->mbnHeaderImageSrcValue->setText(tmp.sprintf(format, flip_endian32(header->image_src)));
        ui->mbnHeaderImageDestPtrValue->setText(tmp.sprintf(format, flip_endian32(header->image_dest_ptr)));
        ui->mbnHeaderImageSizeValue->setText(tmp.sprintf(format, flip_endian32(header->image_size)));
        ui->mbnHeaderCodeSizeValue->setText(tmp.sprintf(format, flip_endian32(header->code_size)));
		ui->mbnHeaderSignaturePtrValue->setText(tmp.sprintf(format, flip_endian32(header->signature_ptr)));
        ui->mbnHeaderSignatureSizeValue->setText(tmp.sprintf(format, flip_endian32(header->signature_size)));
		ui->mbnHeaderCertChainPtrValue->setText(tmp.sprintf(format, flip_endian32(header->cert_chain_ptr)));
        ui->mbnHeaderCertChainSizeValue->setText(tmp.sprintf(format, flip_endian32(header->cert_chain_size)));

    } else {
        log(tmp.sprintf("Image Size: %lu bytes", header->image_size));
        log(tmp.sprintf("Code Size: %lu bytes", header->code_size));
        log(tmp.sprintf("Signature Size: %lu bytes  Offset: 0x%08X", header->signature_size, (header->signature_ptr - header->image_dest_ptr)));
        log(tmp.sprintf("Cert Chain Size: %lu bytes  Offset: 0x%08X", header->cert_chain_size, (header->cert_chain_ptr - header->image_dest_ptr)));

		if (isEightyByte) {
			ui->mbnHeaderCodewordValue->setText(tmp.sprintf(format, header->codeword));
			ui->mbnHeaderMagicValue->setText(tmp.sprintf(format, header->magic));
			ui->mbnHeaderReserved1Value->setText(tmp.sprintf(format, header->reserved1));
			ui->mbnHeaderReserved2Value->setText(tmp.sprintf(format, header->reserved2));
			ui->mbnHeaderOemRootCertSelectionValue->setText(tmp.sprintf(format, header->oem_root_cert_sel));
			ui->mbnHeaderOemNumberRootCertsValue->setText(tmp.sprintf(format, header->oem_num_root_certs));
			ui->mbnHeaderReserved3Value->setText(tmp.sprintf(format, header->reserved3));
			ui->mbnHeaderReserved4Value->setText(tmp.sprintf(format, header->reserved4));
			ui->mbnHeaderReserved5Value->setText(tmp.sprintf(format, header->reserved5));
			ui->mbnHeaderReserved6Value->setText(tmp.sprintf(format, header->reserved6));
			ui->mbnHeaderReserved7Value->setText(tmp.sprintf(format, header->reserved7));
		}

		ui->mbnHeaderImageIdValue->setText(tmp.sprintf(format, header->image_id));
        ui->mbnHeaderImageSrcValue->setText(tmp.sprintf(format, header->image_src));
        ui->mbnHeaderImageDestPtrValue->setText(tmp.sprintf(format, header->image_dest_ptr));
        ui->mbnHeaderImageSizeValue->setText(tmp.sprintf(format, header->image_size));
        ui->mbnHeaderCodeSizeValue->setText(tmp.sprintf(format, header->code_size));
        ui->mbnHeaderSignaturePtrValue->setText(tmp.sprintf(format, header->signature_ptr));
        ui->mbnHeaderSignatureSizeValue->setText(tmp.sprintf(format, header->signature_size));
        ui->mbnHeaderCertChainPtrValue->setText(tmp.sprintf(format, header->cert_chain_ptr));
        ui->mbnHeaderCertChainSizeValue->setText(tmp.sprintf(format, header->cert_chain_size));
    }


}

void MbnToolWindow::extractSegment(int segment)
{
	QString tmp;
	QString fileName = ui->filePathInput->text();

	if (!fileName.length()) {
		log("Please Enter or Browse For a Image File");
		return;
	}

	std::ifstream file(fileName.toStdString(), std::ios::in | std::ios::binary);

	file.seekg(0, file.end);
	size_t fileSize = file.tellg();
	file.seekg(0, file.beg);

	if (MBN_HEADER_MAX_SIZE > fileSize) {
		log("File is not large enough to be an mbn image");
		file.close();
		return;
	}

	uint8_t headerBuffer[MBN_HEADER_MAX_SIZE];

	file.read((char*)headerBuffer, MBN_HEADER_MAX_SIZE);

	size_t segmentOffset,
		segmentSize;

	MbnSblHeader* header;

	if (isEightyByteHeader(headerBuffer)) {
		header = (MbnSblHeader*)headerBuffer;
	} else if (isFourtyByteHeader(headerBuffer)) {
		MbnHeader* header = (MbnHeader*)headerBuffer;
	} else {
		log("Not a valid MBN image");
		file.close();
		return;
	}

	switch (segment) {
		case kMbnSegmentCode:
			segmentOffset	= header->image_src;
			segmentSize		= header->code_size;
			break;
		case kMbnSegmentSignature:
			segmentOffset	= header->image_src + header->code_size;
			segmentSize		= header->signature_size;
			break;
		case kMbnSegmentX509Chain:
			segmentOffset	= header->image_src + header->code_size + header->signature_size;
			segmentSize		= header->cert_chain_size;
			break;
		default:
			log("Invalid Segment Selected");
			return;
			break;
	}

	if (segmentSize > fileSize) {
		log("Error. Certificate size exceeds file size");
		file.close();
		return;
	}

	QString outFilePath = QFileDialog::getSaveFileName(this, tr("Save image segment to file"), "", tr("Binary Files (*.bin)"));

	std::ofstream outFile(outFilePath.toStdString(), std::ios::out | std::ios::binary | std::ios::trunc);

	if (!outFile.is_open()) {
		log("Could not open file for writing");
		file.close();
	}

	uint8_t* outBuffer = new uint8_t[segmentSize];

	file.seekg(segmentOffset, file.beg);

	file.read((char *)outBuffer, segmentSize);

    if (segment == kMbnSegmentX509Chain) {

    }

	outFile.write((char *)outBuffer, segmentSize);

	outFile.close();

	free(outBuffer);

	file.close();

	log(tmp.sprintf("Image segment saved to %s. Size: %lu", outFilePath.toStdString().c_str(), segmentSize));
}



void MbnToolWindow::readCode()
{
	return extractSegment(kMbnSegmentCode);
}

void MbnToolWindow::readSignature()
{
	return extractSegment(kMbnSegmentSignature);
}

void MbnToolWindow::readX509Chain()
{
	return extractSegment(kMbnSegmentX509Chain);
}

/**
 * @brief MbnToolWindow::log
 * @param message
 */
void MbnToolWindow::log(const char* message)
{
    ui->log->appendPlainText(message);
}

/**
 * @brief MbnToolWindow::log
 * @param message
 */
void MbnToolWindow::log(std::string message)
{
    ui->log->appendPlainText(message.c_str());
}

/**
 * @brief MbnToolWindow::log
 * @param message
 */
void MbnToolWindow::log(QString message)
{
    ui->log->appendPlainText(message);
}

bool MbnToolWindow::isValidHeader(uint8_t* data)
{
	return isEightyByteHeader(data) || isFourtyByteHeader(data);
}

bool MbnToolWindow::isEightyByteHeader(uint8_t* data)
{
	MbnSblHeader* header = (MbnSblHeader*)data;

	if (header->magic == MBN_EIGHTY_BYTE_MAGIC) {
		return true;
	}

	return false;
}

bool MbnToolWindow::isFourtyByteHeader(uint8_t* data)
{
	MbnHeader* header = (MbnHeader*)data;

	if (header->image_id >= MBN_IMAGE_LAST) {
		return true;
	}

	return false;
}
