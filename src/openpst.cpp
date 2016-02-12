/**
* LICENSE PLACEHOLDER
*
* @file main.cpp
* @package OpenPST
* @brief program entry point
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/

#include <QApplication>
#include <QMetaType>
#include <QMessageBox>
#include "gui/main_window.h"
#ifdef _WIN32
    #include <windows.h>
#endif

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray); 
    
    QApplication application(argc, argv);
    

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("OS does not have a viable system tray"));
        return 1;
    }
    
    QApplication::setQuitOnLastWindowClosed(false);

    openpst::MainWindow mainWindow;

    mainWindow.show();

    return application.exec();
}
