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
#include "gui/widget/progress_group_widget.h"
#include "ui_progress_group_widget.h"

using namespace OpenPST::GUI;

ProgressGroupWidget::ProgressGroupWidget(QWidget *parent) :
    QGroupBox(parent),
	ui(new Ui::ProgressGroupWidget)
{
	ui->setupUi(this);

	cancelButton = ui->cancelButton;
	cancelAllButton = ui->cancelAllButton;
}

ProgressGroupWidget::~ProgressGroupWidget()
{
	delete ui;
}

void ProgressGroupWidget::setProgress(int value)
{
	ui->progressBar->setValue(value);
}

void ProgressGroupWidget::setProgress(int min, int max, int value)
{
	ui->progressBar->setMinimum(min);
	ui->progressBar->setMaximum(max);
	ui->progressBar->setValue(value);
}

void ProgressGroupWidget::setTaskCount(int count)
{
	QString tmp;
	ui->taskCountLabel->setText(tmp.sprintf("%d Tasks"));
}

void ProgressGroupWidget::setTextLeft(QString text)
{
	ui->textLeft->setText(text);
}

void ProgressGroupWidget::setTextRight(QString text)
{
	ui->textRight->setText(text);
}

void ProgressGroupWidget::reset()
{
	setProgress(0, 100, 0);
	setTextLeft("");
	setTextRight("");
	disableCancel();
	disableCancelAll();
}

void ProgressGroupWidget::enableCancel()
{
	ui->cancelButton->setEnabled(true);
}

void ProgressGroupWidget::disableCancel()
{
	ui->cancelButton->setEnabled(false);
}

void ProgressGroupWidget::enableCancelAll()
{
	ui->cancelAllButton->setEnabled(true);
}

void ProgressGroupWidget::disableCancelAll()
{
	ui->cancelAllButton->setEnabled(false);

}
