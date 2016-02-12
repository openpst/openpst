/**
* LICENSE PLACEHOLDER
*
* @file qcsamunlock.cpp
* @package OpenPST
* @brief qcdm program entry point
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/

#include "gui/application.h"
#include "gui/qcsamunlock_window.h"

/**
* @brief main
* @param argc
* @param argv
* @return
*/
int main(int argc, char *argv[])
{
    OpenPST::Application application(argc, argv, "qcsamunlock_");

	OpenPST::QcSamUnlockWindow window;

    window.show();

    return application.exec();
}
