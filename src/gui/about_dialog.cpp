/**
* LICENSE PLACEHOLDER
*
* @file about_dialog.cpp
* @class AboutDialog
* @package OpenPST
* @brief Shared program about dialog window
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#include "gui/about_dialog.h"
#include "ui_about_dialog.h"

using namespace OpenPST::GUI;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
