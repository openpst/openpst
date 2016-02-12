/**
* LICENSE PLACEHOLDER
*
* @file 
* @class 
* @package OpenPST
* @brief
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#include "gui/widget/log_widget.h"
#include "ui_log_widget.h"

using namespace OpenPST::GUI;

LogWidget::LogWidget(QWidget *parent) :
    QGroupBox(parent),
	ui(new Ui::LogWidget)
{
	ui->setupUi(this);

	QObject::connect(ui->clearLogButton,			SIGNAL(clicked()), this, SLOT(clear()));
	QObject::connect(ui->saveLogButton,				SIGNAL(clicked()), this, SLOT(save()));
}

LogWidget::~LogWidget()
{
	delete ui;
}


void LogWidget::clear()
{
	ui->log->clear();
}

void LogWidget::save()
{

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log"), "", tr("Log Files (*.log)"));

	if (!fileName.length()) {
		return;
	}

	std::ofstream file(fileName.toStdString().c_str(), std::ios::out | std::ios::trunc);

	if (!file.is_open()) {
		log("Error opening file for writing");
		return;
	}

	QString content = ui->log->toPlainText();

	file.write(content.toStdString().c_str(), content.size());
	file.close();
}


void LogWidget::log(const char* message)
{
	ui->log->appendPlainText(message);
}


void LogWidget::log(std::string message)
{
	ui->log->appendPlainText(message.c_str());
}


void LogWidget::log(QString message)
{
	ui->log->appendPlainText(message);
}

