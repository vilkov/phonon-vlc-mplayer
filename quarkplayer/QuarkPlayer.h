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

#ifndef QUARKPLAYER_H
#define QUARKPLAYER_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QObject>
#include <QtCore/QList>

#include <phonon/phononnamespace.h>
#include <phonon/path.h>

class MainWindow;
class Config;

namespace Phonon {
	class MediaObject;
	class AudioOutput;
	class MediaSource;
	class VideoWidget;
	class MediaController;
}

/**
 * QuarkPlayer main API.
 *
 * API stands for Application programming interface.
 *
 * Gives access to all important objects of the application.
 * This is the main class for writing plugins.
 *
 * Facade pattern.
 *
 * @see http://en.wikipedia.org/wiki/Facade_pattern
 * @see http://en.wikipedia.org/wiki/API
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API QuarkPlayer : public QObject {
	Q_OBJECT
public:

	/**
	 * Creates a QuarkPlayer instance.
	 *
	 * This is called only once inside main.cpp
	 *
	 * @param parent QuarkPlayer QObject parent, should be QCoreApplication
	 * @see main.cpp
	 */
	QuarkPlayer(QObject * parent);

	~QuarkPlayer();

	/**
	 * Registers all the QMetaType needed by QuarkPlayer.
	 *
	 * First thing to do inside main.cpp.
	 *
	 * @see qRegisterMetaType()
	 * @see qRegisterMetaTypeStreamOperators()
	 */
	static void registerMetaTypes();

	/**
	 * Gets QuarkPlayer main window.
	 *
	 * MainWindow always exists and thus this method cannot return NULL.
	 * One could think about returning a reference (&) but I prefer to return
	 * a pointer since Qt works this way regarding widgets.
	 *
	 * @return QuarkPlayer main window (cannot be NULL or there is a bug somewhere...)
	 * @see MainWindow
	 * @see QMainWindow
	 */
	MainWindow * mainWindow() const;

	/**
	 * Changes the current media object to be used when playing a media.
	 *
	 * @param mediaObject new media object to be used
	 * @see currentMediaObject()
	 */
	void setCurrentMediaObject(Phonon::MediaObject * mediaObject);

	/**
	 * Gets the current media object used.
	 *
	 * There can be no media object (will return NULL) if the user
	 * has not played any media yet.
	 *
	 * @return current media object or NULL
	 * @see setCurrentMediaObject()
	 * @see mediaObjectList()
	 */
	Phonon::MediaObject * currentMediaObject() const;

	/**
	 * Gets the list of all available media objects.
	 *
	 * Most of the time there is only one media object
	 * since the user will play only one media at a time.
	 *
	 * @return the list of available media objects
	 * @see currentMediaObject()
	 */
	QList<Phonon::MediaObject *> mediaObjectList() const;

	/**
	 * Gets the music/video title of the current media object.
	 *
	 * This is a utility function.
	 */
	QString currentMediaObjectTitle() const;

	/**
	 * Plays a file, an url, whatever... using the current media object available.
	 *
	 * Passing a QString is OK since Phonon::MediaSource constructor takes it.
	 *
	 * @param mediaSource media to play
	 */
	void play(const Phonon::MediaSource & mediaSource);

	/**
	 * Adds a list of files to the current playlist.
	 *
	 * This is a function that will send a signal to the
	 * playlist plugin if any exist.
	 *
	 * @param files files to add to the current playlist
	 * @see addFilesToCurrentPlaylist()
	 */
	void addFilesToPlaylist(const QStringList & files);

	/**
	 * Returns the current audio output associated with the current media object.
	 *
	 * @see currentMediaObject()
	 */
	Phonon::AudioOutput * currentAudioOutput() const;

	/**
	 * Returns the current audio output path associated with the current media object.
	 *
	 * @see currentMediaObject()
	 */
	Phonon::Path currentAudioOutputPath() const;

	/**
	 * Returns the current video widget associated with the current media object.
	 *
	 * A video widget shows the video of a DVD playing for example.
	 * This should be taking care of by a plugin since by default QuarkPlayer does not show
	 * any video. VideoWidgetPlugin does the job.
	 *
	 * @see currentMediaObject()
	 * @see VideoWidgetPlugin
	 */
	Phonon::VideoWidget * currentVideoWidget() const;

	/**
	 * FIXME See MediaController.h
	 * Need to implement a full plugin system like Qt Creator has
	 * Let's wait for Qt Creator source code to be released...
	 * This way MainWindow would be also a real plugin!
	 */
	void setMediaController(Phonon::MediaController * mediaController);
	Phonon::MediaController * mediaController() const;

public slots:

	/**
	 * Creates a new media object.
	 *
	 * This will send mediaObjectAdded() signal.
	 */
	Phonon::MediaObject * createNewMediaObject();

signals:

	/**
	 * The current media object has been changed.
	 *
	 * @param mediaObject new current media object to be used
	 * @see setCurrentMediaObject()
	 */
	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	/**
	 * A new media object has been added/created.
	 *
	 * @param mediaObject the new media object that has been added
	 * @see createNewMediaObject()
	 */
	void mediaObjectAdded(Phonon::MediaObject * mediaObject);

	/**
	 * Signal emitted when files should be added to the current playlist.
	 *
	 * This signal should be catched by a playlist plugin if any exist.
	 *
	 * @param files list of files to be added
	 * @see addFilesToPlaylist()
	 */
	void addFilesToCurrentPlaylist(const QStringList & files);

private slots:

	/**
	 * Volume output has been changed, let's save it.
	 * FIXME Moves this somewhere else?
	 */
	void volumeChanged(qreal volume);

	/**
	 * Volume output has been muted, let's save it.
	 * FIXME Moves this somewhere else?
	 */
	void mutedChanged(bool muted);

	/** All plugins have been loaded. */
	void allPluginsLoaded();

private:

	/**
	 * QuarkPlayer main window.
	 *
	 * @see MainWindow
	 * @see QMainWindow
	 */
	MainWindow * _mainWindow;

	/** Current media object, can be NULL if none. */
	Phonon::MediaObject * _currentMediaObject;

	/** List of available media objects. */
	QList<Phonon::MediaObject *> _mediaObjectList;

	Phonon::MediaController * _mediaController;
};

#endif	//QUARKPLAYER_H
