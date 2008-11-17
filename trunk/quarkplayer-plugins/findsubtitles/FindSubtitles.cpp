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

#include "FindSubtitles.h"

#include "FindSubtitlesWindow.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/ActionCollection.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <phonon/mediacontroller.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(findsubtitles, FindSubtitlesFactory);

PluginInterface * FindSubtitlesFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new FindSubtitles(quarkPlayer, uuid);
}

FindSubtitles::FindSubtitles(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: QObject(quarkPlayer.mainWindow()),
	PluginInterface(quarkPlayer, uuid) {

	//FIXME See MainWindow.cpp MediaController.cpp FindSubtitles.cpp QuarkPlayer.h
	//Need to implement a full plugin system like Qt Creator has
	//Let's wait for Qt Creator source code to be released...
	//This way MainWindow would be also a real plugin!
	connect(ActionCollection::action("findSubtitles"), SIGNAL(triggered()),
		SLOT(findSubtitles()));
	connect(ActionCollection::action("uploadSubtitles"), SIGNAL(triggered()),
		SLOT(uploadSubtitles()));
}

FindSubtitles::~FindSubtitles() {
}

void FindSubtitles::findSubtitles() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	if (mediaObject) {
		Phonon::MediaSource source = mediaObject->currentSource();
		QString fileName(source.fileName());

		static FindSubtitlesWindow * findSubtitlesWindow = NULL;
		if (!findSubtitlesWindow) {
			findSubtitlesWindow = new FindSubtitlesWindow(quarkPlayer().mainWindow());
			connect(findSubtitlesWindow, SIGNAL(subtitleDownloaded(const QString &)),
				SLOT(loadSubtitle(const QString &)));
		}
		findSubtitlesWindow->setMovieFileName(fileName);
		findSubtitlesWindow->setLanguage(Config::instance().language());
		findSubtitlesWindow->show();
	}
}

void FindSubtitles::loadSubtitle(const QString & fileName) {
	if (fileName.isEmpty()) {
		return;
	}

	QHash<QByteArray, QVariant> properties;
	properties.insert("type", "file");
	properties.insert("name", fileName);

	int id = 0;
	Phonon::SubtitleDescription subtitle(id, properties);

	Phonon::MediaController * mediaController = quarkPlayer().mediaController();
	if (mediaController) {
		mediaController->setCurrentSubtitle(subtitle);
	}
}

void FindSubtitles::uploadSubtitles() {
	QDesktopServices::openUrl(QUrl("http://www.opensubtitles.org/upload"));
}