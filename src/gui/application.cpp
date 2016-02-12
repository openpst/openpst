/**
* LICENSE PLACEHOLDER
*
* @file application.h
* @package OpenPST
* @brief GUI main application wrapper
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "application.h"

using namespace OpenPST::GUI;

Application::Application(int argc, char *argv[], const char* translatorPrefix) :
	QApplication(argc, argv)
{

#if defined (_WIN32) && defined (DEBUG)
	AllocConsole();
	freopen_s(&sout, "CONOUT$", "w", stdout);
	freopen_s(&serr, "CONOUT$", "w", stderr);
#endif

	qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	translator.load(translatorPrefix + QLocale::system().name(), "languages");

	this->installTranslator(&qtTranslator);
	this->installTranslator(&translator);
}


Application::~Application()
{
#if defined (_WIN32) && defined (DEBUG)
	fclose(sout);
	fclose(serr);
	FreeConsole();
#endif
}