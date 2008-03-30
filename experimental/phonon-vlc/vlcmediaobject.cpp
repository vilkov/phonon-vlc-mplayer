/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "vlcmediaobject.h"

#include "vlcloader.h"
#include "vlc_symbols.h"

#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC
{

//VLC returns a strange position... have to multiply by VLC_POSITION_RESOLUTION
static const int VLC_POSITION_RESOLUTION = 1000;

VLCMediaObject::VLCMediaObject(QObject * parent)
	: QObject(parent) {

	//MediaPlayer
	_vlcMediaPlayer = NULL;
	_vlcMediaPlayerEventManager = NULL;

	//Media
	_vlcMedia = NULL;
	_vlcMediaEventManager = NULL;

	//MediaList
	_vlcMediaList = NULL;
	_vlcMediaListEventManager = NULL;

	//MediaListView
	_vlcMediaListView = NULL;
	_vlcMediaListViewEventManager = NULL;

	//MediaListPlayer
	_vlcMediaListPlayer = NULL;
	_vlcMediaListPlayerEventManager = NULL;

	//MediaDiscoverer
	_vlcMediaDiscoverer = NULL;
	_vlcMediaDiscovererEventManager = NULL;
}

VLCMediaObject::~VLCMediaObject() {
}

void VLCMediaObject::loadMedia(const QString & filename) {
	qDebug() << "VLCMediaObject::loadMedia:" << filename;

	//See code from http://wiki.videolan.org/LibVLC_Media_List_Management

	//Create a new media from a filename
	_vlcMedia = p_libvlc_media_new(_vlcInstance, filename.toAscii(), _vlcException);
	checkException();

	//Create a media player environement
	_vlcMediaPlayer = p_libvlc_media_player_new_from_media(_vlcMedia, _vlcException);

	//No need to keep the media now
	//p_libvlc_media_release(_vlcMedia);

	//connectToAllVLCEvents() at the end since it needs _vlcMediaPlayer
	connectToAllVLCEvents();

	qDebug() << "duration:" << p_libvlc_media_get_duration(_vlcMedia, _vlcException);

	//updateMetaData();
	emit stateChanged(Phonon::StoppedState);
}

void VLCMediaObject::play() {
	//Get our media player to use our window
	//FIXME This code does not work inside libvlc!!!
	//Check VideoWidget.cpp p_libvlc_video_set_parent()
	p_libvlc_media_player_set_drawable(_vlcMediaPlayer, _vlcMediaPlayerWidgetId, _vlcException);
	checkException();

	//Play
	p_libvlc_media_player_play(_vlcMediaPlayer, _vlcException);
	checkException();

	updateMetaData();
}

void VLCMediaObject::pause() {
	p_libvlc_media_player_pause(_vlcMediaPlayer, _vlcException);
	checkException();
}

void VLCMediaObject::stop() {
	p_libvlc_media_player_stop(_vlcMediaPlayer, _vlcException);
	checkException();

	//VLC does not send a StoppedState when running libvlc_media_player_stop() :/
	emit stateChanged(Phonon::StoppedState);
}

void VLCMediaObject::seek(qint64 milliseconds) {
	p_libvlc_media_player_set_time(_vlcMediaPlayer, milliseconds, _vlcException);
	checkException();
}

Phonon::State VLCMediaObject::state() const {
	libvlc_state_t st = libvlc_Stopped;
	if (_vlcMediaPlayer) {
		st = p_libvlc_media_player_get_state(_vlcMediaPlayer, _vlcException);
		checkException();
	}

	Phonon::State state;

	switch (st) {
	case libvlc_NothingSpecial:
		state = Phonon::LoadingState;
		break;
	case libvlc_Stopped:
		state = Phonon::StoppedState;
		break;
	case libvlc_Opening:
		state = Phonon::LoadingState;
		break;
	case libvlc_Buffering:
		state = Phonon::BufferingState;
		break;
	case libvlc_Ended:
		state = Phonon::StoppedState;
		break;
	case libvlc_Error:
		state = Phonon::ErrorState;
		break;
	case libvlc_Playing:
		state = Phonon::PlayingState;
		break;
	case libvlc_Paused:
		state = Phonon::PausedState;
		break;
	default:
		qCritical() << "error: unknown VLC state=" << st;
		break;
	}

	return state;
}

QString VLCMediaObject::errorString() const {
	return p_libvlc_exception_get_message(_vlcException);
}

bool VLCMediaObject::hasVideo() const {
	bool hasVideo = p_libvlc_media_player_has_vout(_vlcMediaPlayer, _vlcException);
	checkException();

	return hasVideo;
}

bool VLCMediaObject::isSeekable() const {
	bool isSeekable = p_libvlc_media_player_is_seekable(_vlcMediaPlayer, _vlcException);
	checkException();

	return isSeekable;
}

void VLCMediaObject::connectToAllVLCEvents() {

	//MediaPlayer
	_vlcMediaPlayerEventManager = p_libvlc_media_player_event_manager(_vlcMediaPlayer, _vlcException);
	libvlc_event_type_t eventsMediaPlayer[] = {
		libvlc_MediaInstancePlayed,
		libvlc_MediaInstancePaused,
		libvlc_MediaInstanceReachedEnd,
		libvlc_MediaInstanceEncounteredError,
		libvlc_MediaInstanceTimeChanged,
		libvlc_MediaInstancePositionChanged,
		libvlc_MediaInstanceSeekableChanged,
		libvlc_MediaInstancePausableChanged,
	};
	int nbEvents = sizeof(eventsMediaPlayer) / sizeof(*eventsMediaPlayer);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaPlayerEventManager, eventsMediaPlayer[i], libvlc_callback, this, _vlcException);
	}


	//Media
	_vlcMediaEventManager = p_libvlc_media_event_manager(_vlcMedia, _vlcException);
	libvlc_event_type_t eventsMedia[] = {
		libvlc_MediaDescriptorMetaChanged,
		libvlc_MediaDescriptorSubItemAdded,
		libvlc_MediaDescriptorDurationChanged,
		//libvlc_MediaDescriptorPreparsedChanged,
		libvlc_MediaDescriptorFreed,
		libvlc_MediaDescriptorStateChanged,
	};
	nbEvents = sizeof(eventsMedia) / sizeof(*eventsMedia);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaEventManager, eventsMedia[i], libvlc_callback, this, _vlcException);
		checkException();
	}


	//MediaList
	/*
	_vlcMediaListEventManager = p_libvlc_media_list_event_manager(_vlcMediaList, _vlcException);
	libvlc_event_type_t eventsMediaList[] = {
		libvlc_MediaListItemAdded,
		libvlc_MediaListWillAddItem,
		libvlc_MediaListItemDeleted,
		libvlc_MediaListWillDeleteItem,
	};
	nbEvents = sizeof(eventsMediaList) / sizeof(*eventsMediaList);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaListEventManager, eventsMediaList[i], libvlc_callback, this, _vlcException);
	}
	*/


	//MediaListView
	//FIXME why libvlc_media_list_view_event_manager() does not take a libvlc_exception_t?
	/*
	_vlcMediaListViewEventManager = p_libvlc_media_list_view_event_manager(_vlcMediaListView);
	libvlc_event_type_t eventsMediaListView[] = {
		libvlc_MediaListViewItemAdded,
		libvlc_MediaListViewWillAddItem,
		libvlc_MediaListViewItemDeleted,
		libvlc_MediaListViewWillDeleteItem,
	};
	nbEvents = sizeof(eventsMediaListView) / sizeof(*eventsMediaListView);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaListViewEventManager, eventsMediaListView[i], libvlc_callback, this, _vlcException);
	}
	*/


	//MediaListPlayer
	//FIXME why there is no libvlc_media_list_player_event_manager()?
	/*
	_vlcMediaListPlayerEventManager = p_libvlc_media_list_event_manager(_vlcMediaListPlayer, _vlcException);
	libvlc_event_type_t eventsMediaListPlayer[] = {
		libvlc_MediaListPlayerPlayed,
		libvlc_MediaListPlayerNextItemSet,
		libvlc_MediaListPlayerStopped,
	};
	nbEvents = sizeof(eventsMediaListPlayer) / sizeof(*eventsMediaListPlayer);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaListPlayerEventManager, eventsMediaListPlayer[i], libvlc_callback, this, _vlcException);
	}
	*/


	//MediaDiscoverer
	//FIXME why libvlc_media_discoverer_event_manager() does not take a libvlc_exception_t?
	/*
	_vlcMediaDiscovererEventManager = p_libvlc_media_discoverer_event_manager(_vlcMediaDiscoverer);
	libvlc_event_type_t eventsMediaDiscoverer[] = {
		libvlc_MediaDiscovererStarted,
		libvlc_MediaDiscovererEnded
	};
	nbEvents = sizeof(eventsMediaDiscoverer) / sizeof(*eventsMediaDiscoverer);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(_vlcMediaDiscovererEventManager, eventsMediaDiscoverer[i], libvlc_callback, this, _vlcException);
	}
	*/
}

void VLCMediaObject::libvlc_callback(const libvlc_event_t * event, void * user_data) {
	VLCMediaObject * vlcMediaObject = (VLCMediaObject *) user_data;

	qDebug() << "event=" << (int) vlcMediaObject << p_libvlc_event_type_name(event->type);

	if (event->type == libvlc_MediaInstanceTimeChanged) {
		//new_time / VLC_POSITION_RESOLUTION since VLC adds * VLC_POSITION_RESOLUTION, don't know why...
		qDebug() << "new_time:" << event->u.media_player_time_changed.new_time;
		vlcMediaObject->totalTime();
		vlcMediaObject->currentTime();
		//emit vlcMediaObject->tick(event->u.media_instance_time_changed.new_time / VLC_POSITION_RESOLUTION);
		emit vlcMediaObject->tick(vlcMediaObject->currentTime());
		////////BUG POSITION//////////
	}

	//void stateChanged(Phonon::State newState, Phonon::State oldState);
	//~ Phonon::LoadingState
	//~ Phonon::StoppedState
	//~ Phonon::PlayingState
	//~ Phonon::BufferingState
	//~ Phonon::PausedState
	//~ Phonon::ErrorState

	//~ libvlc_MediaInstancePlayed,
	//~ libvlc_MediaInstancePaused,
	//~ libvlc_MediaInstanceReachedEnd,
	//~ libvlc_MediaInstanceEncounteredError,
	//~ libvlc_MediaInstanceTimeChanged,
	//~ libvlc_MediaInstancePositionChanged,
	//~ libvlc_MediaInstanceSeekableChanged,
	//~ libvlc_MediaInstancePausableChanged,

	//Media instance event

	if (event->type == libvlc_MediaInstancePlayed) {
		emit vlcMediaObject->stateChanged(Phonon::PlayingState);
		vlcMediaObject->updateMetaData();
	}

	if (event->type == libvlc_MediaInstancePaused) {
		emit vlcMediaObject->stateChanged(Phonon::PausedState);
	}

	if (event->type == libvlc_MediaInstanceReachedEnd) {
		emit vlcMediaObject->stateChanged(Phonon::StoppedState);
	}

	//Meta descriptor event

	if (event->type == libvlc_MediaDescriptorDurationChanged) {
		//new_duration / VLC_POSITION_RESOLUTION since VLC adds * VLC_POSITION_RESOLUTION, don't know why...
		qDebug() << "new_duration:" << event->u.media_duration_changed.new_duration / 1000;

		emit vlcMediaObject->totalTimeChanged(event->u.media_duration_changed.new_duration / 1000);
		//emit vlcMediaObject->tick(vlcMediaObject->totalTime());
	}

	if (event->type == libvlc_MediaDescriptorMetaChanged) {
		vlcMediaObject->updateMetaData();
	}
}

void VLCMediaObject::updateMetaData() {

	qDebug() << "ARTIST:" << p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Artist, _vlcException);


	//~ libvlc_meta_Title,
	//~ libvlc_meta_Artist,
	//~ libvlc_meta_Genre,
	//~ libvlc_meta_Copyright,
	//~ libvlc_meta_Album,
	//~ libvlc_meta_TrackNumber,
	//~ libvlc_meta_Description,
	//~ libvlc_meta_Rating,
	//~ libvlc_meta_Date,
	//~ libvlc_meta_Setting,
	//~ libvlc_meta_URL,
	//~ libvlc_meta_Language,
	//~ libvlc_meta_NowPlaying,
	//~ libvlc_meta_Publisher,
	//~ libvlc_meta_EncodedBy,
	//~ libvlc_meta_ArtworkURL,
	//~ libvlc_meta_TrackID

	//~ Phonon::ArtistMetaData
	//~ Phonon::AlbumMetaData
	//~ Phonon::TitleMetaData
	//~ Phonon::DateMetaData
	//~ Phonon::GenreMetaData
	//~ Phonon::TracknumberMetaData
	//~ Phonon::DescriptionMetaData

	QMultiMap<QString, QString> metaDataMap;

	metaDataMap.insert(QLatin1String("ARTIST"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Artist, _vlcException)));
	metaDataMap.insert(QLatin1String("ALBUM"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Album, _vlcException)));
	metaDataMap.insert(QLatin1String("TITLE"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Title, _vlcException)));
	metaDataMap.insert(QLatin1String("DATE"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Date, _vlcException)));
	metaDataMap.insert(QLatin1String("GENRE"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Genre, _vlcException)));
	metaDataMap.insert(QLatin1String("TRACKNUMBER"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_TrackNumber, _vlcException)));
	metaDataMap.insert(QLatin1String("DESCRIPTION"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_Description, _vlcException)));

	metaDataMap.insert(QLatin1String("COPYRIGHT"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_TrackNumber, _vlcException)));
	metaDataMap.insert(QLatin1String("URL"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_URL, _vlcException)));
	metaDataMap.insert(QLatin1String("ENCODEDBY"), QString::fromUtf8(p_libvlc_media_get_meta(_vlcMedia, libvlc_meta_EncodedBy, _vlcException)));

	emit metaDataChanged(metaDataMap);
}

qint64 VLCMediaObject::totalTime() const {
	libvlc_time_t time = 0;

	if (_vlcMediaPlayer) {
		time = p_libvlc_media_player_get_length(_vlcMediaPlayer, _vlcException);
		checkException();
	}

	qDebug() << "totalTime:" << time;

	return time;
}

qint64 VLCMediaObject::currentTime() const {
	libvlc_time_t time = p_libvlc_media_player_get_time(_vlcMediaPlayer, _vlcException);
	checkException();

	qDebug() << "currentTime:" << time;

	return time;
}

}}	//Namespace Phonon::VLC