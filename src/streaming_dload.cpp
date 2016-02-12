/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload.cpp
* @package OpenPST
* @brief streaming dload program entry point
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "gui/application.h"
#include "gui/streaming_dload_window.h"

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
	OpenPST::GUI::Application application(argc, argv, "streaming_dload_");

    OpenPST::GUI::StreamingDloadWindow window;
    
    window.show();

    return application.exec();
}
