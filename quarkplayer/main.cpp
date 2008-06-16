/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"

#include "config/Config.h"
#include "version.h"
#include "config.h"

#include <tkutil/Translator.h>
#include <tkutil/TkIcon.h>

#ifdef KDE4_FOUND
	#include <KApplication>
	#include <KAboutData>
	#include <KCmdLineArgs>
#else
	#include <QtGui/QApplication>
#endif	//KDE4_FOUND

#include <QtGui/QStyleFactory>

#include <QtCore/QSettings>

int main(int argc, char * argv[]) {
	Q_INIT_RESOURCE(quarkplayer);
	Q_INIT_RESOURCE(quarkplayer_oxygen);
	Q_INIT_RESOURCE(quarkplayer_silk);

#ifdef Q_OS_WIN
	//Under Windows, do not use registry database
	//use INI format instead
	//Under other OS, use the default storage format
	QSettings::setDefaultFormat(QSettings::IniFormat);
#endif	//Q_OS_WIN

#ifdef KDE4_FOUND
	KAboutData aboutData(
			//appName
			"quarkplayer",
			//catalogName
			NULL,
			//programName
			ki18n("QuarkPlayer"),
			//version
			QUARKPLAYER_VERSION,
			//shortDescription
			ki18n("QuarkPlayer, a Phonon media player"),
			KAboutData::License_GPL,
			//copyrightStatement
			ki18n("Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>"),
			//text
			ki18n(""),
			//homePageAddress
			"http://phonon-vlc-mplayer.googlecode.com",
			//bugsEmailAddress
			"phonon-vlc-mplayer@googlegroups.com"
	);

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication app;
#else
	QApplication app(argc, argv);

	//General infos
 	app.setOrganizationName("QuarkPlayer");
	app.setOrganizationDomain("quarkplayer.org");
	app.setApplicationName("QuarkPlayer");
	app.setApplicationVersion(QUARKPLAYER_VERSION);

	//By default QuarkPlayerStyle: specific style for QuarkPlayer
	//Fix some ugly things under Windows XP
	app.setStyle(QStyleFactory::create(Config::instance().style()));
#endif	//KDE4_FOUND

	app.setQuitOnLastWindowClosed(true);

	//Translator
	Translator::instance().load(Config::instance().language());

	//Icons
	TkIcon::setIconTheme(Config::instance().iconTheme());
	TkIcon::setIconSize(16);

	MainWindow window(NULL);
	window.show();

	return app.exec();
}
