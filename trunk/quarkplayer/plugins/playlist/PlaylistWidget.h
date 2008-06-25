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

#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <quarkplayer/PluginInterface.h>

#include <QtGui/QWidget>

#include <QtCore/QList>

#include <phonon/phononnamespace.h>

namespace Ui { class PlaylistWidget; }

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
	class MediaSource;
}

class QToolBar;

/**
 * Playlist.
 *
 * @author Tanguy Krotoff
 */
class PlaylistWidget : public QWidget, public PluginInterface {
	Q_OBJECT
public:

	PlaylistWidget(QuarkPlayer & quarkPlayer);

	~PlaylistWidget();

private slots:

	void addFiles();

	void aboutToFinish();

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

	void tableDoubleClicked(int row, int column);

	void retranslate();

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

private:

	void populateActionCollection();

	void createPlaylistToolBar();

	void addFiles(const QStringList & files);

	QString convertMilliseconds(qint64 totalTime);

	void dragEnterEvent(QDragEnterEvent * event);

	void dropEvent(QDropEvent * event);

	Phonon::MediaObject * _metaObjectInfoResolver;

	QList<Phonon::MediaSource> _mediaSources;

	Ui::PlaylistWidget * _ui;

	QToolBar * _playlistToolBar;
};

#include <quarkplayer/PluginFactory.h>

class PlaylistWidgetFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	PluginInterface * create(QuarkPlayer & quarkPlayer) const;
};

#endif	//PLAYLISTWIDGET_H