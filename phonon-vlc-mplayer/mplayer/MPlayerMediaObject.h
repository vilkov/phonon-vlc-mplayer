/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PHONON_VLC_MPLAYER_MPLAYERMEDIAOBJECT_H
#define PHONON_VLC_MPLAYER_MPLAYERMEDIAOBJECT_H

//FIXME global variable
#include <mplayer/MPlayerLoader.h>
#include <mplayer/MPlayerProcess.h>

#include <phonon/mediaobjectinterface.h>

#include <QtCore/QObject>
#include <QtCore/QProcess>

class MPlayerProcess;

namespace Phonon
{
namespace VLC_MPlayer
{

/**
 *
 *
 * @author Tanguy Krotoff
 */
class MPlayerMediaObject : public QObject {
	Q_OBJECT
public:

	MPlayerMediaObject(QObject * parent);
	~MPlayerMediaObject();

	void setVideoWidgetId(int videoWidgetId);

	void loadMedia(const QString & filename);
	void play();
	void pause();
	void stop();
	void seek(qint64 milliseconds);

	bool hasVideo() const;
	bool isSeekable() const;

	qint64 currentTime() const;
	qint64 totalTime() const;

	QString errorString() const;

	/**
	 * Gets the MPlayerProcess used by this class.
	 *
	 * If NULL is returned, this means that method play()
	 * has not been called yet.
	 *
	 * @return the MPlayerProcess or NULL
	 */
	MPlayerProcess * getMPlayerProcess() const;

signals:

	//void aboutToFinish()
	//void bufferStatus(int percentFilled);
	//void currentSourceChanged(const MediaSource & newSource);
	void finished();
	void hasVideoChanged(bool hasVideo);
	void metaDataChanged(const QMultiMap<QString, QString> & metaData);
	//void prefinishMarkReached(qint32 msecToEnd);
	void seekableChanged(bool isSeekable);
	void stateChanged(Phonon::State newState);
	void tick(qint64 time);
	void totalTimeChanged(qint64 newTotalTime);

private slots:

	void finished(int exitCode, QProcess::ExitStatus exitStatus);

	void stateChanged(MPlayerProcess::State state);

	void tickInternal(double seconds);

	void loadMediaInternal();
	void mediaLoaded();

private:

	void setState(Phonon::State newState);

	/** MPlayer process. */
	MPlayerProcess * _process;

	/** MPlayer process for loading media data. */
	MPlayerProcess * _mediaDataLoader;

	bool _playRequestReached;

	qint64 _currentTime;
	qint64 _totalTime;
	Phonon::State _currentState; 
	QString _filename;

	int _videoWidgetId;
};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_VLC_MPLAYER_MPLAYERMEDIAOBJECT_H
