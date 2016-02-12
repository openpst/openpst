/**
* LICENSE PLACEHOLDER
*
* @file mbn_tool.cpp
* @package OpenPST
* @brief mbn tool program entry point
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "gui/application.h"
#include "gui/mbn_tool_window.h"

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{

    OpenPST::GUI::Application application(argc, argv, "mbn_tool_");

    OpenPST::GUI::MbnToolWindow window;

    window.show();

    return application.exec();
}
