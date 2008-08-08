/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *               2008       Lukas Durfina <lukas.durfina@gmail.com>
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

#include "VLCMediaObject.h"

#include "VideoWidget.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

#include <QtCore/QTimer>
#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC
{

//VLC returns a strange position... have to multiply by VLC_POSITION_RESOLUTION
static const int VLC_POSITION_RESOLUTION = 1000;

VLCMediaObject::VLCMediaObject( QObject * parent )
	: MediaObject( parent ), VLCMediaController()
{
	//MediaPlayer
	p_vlc_media_player = NULL;
	p_vlc_media_player_event_manager = NULL;

	//Media
	p_vlc_media = NULL;
	p_vlc_media_event_manager = NULL;

	//MediaDiscoverer
	p_vlc_media_discoverer = NULL;
	p_vlc_media_discoverer_event_manager = NULL;

	i_total_time = 0;
	b_has_video = false;
	b_seekable = false;
}

VLCMediaObject::~VLCMediaObject()
{
	//unloadMedia();
}

void VLCMediaObject::unloadMedia()
{
	if( p_vlc_media_player )
	{
		p_libvlc_media_player_release( p_vlc_media_player );
		p_vlc_media_player = NULL;
	}

	if( p_vlc_media )
	{
		p_libvlc_media_release( p_vlc_media );
		p_vlc_media = NULL;
	}
}

void VLCMediaObject::loadMediaInternal( const QString & filename )
{
	qDebug() << __FUNCTION__ << filename;

	//Create a new media from a filename
	p_vlc_media = p_libvlc_media_new( p_vlc_instance, filename.toAscii(), p_vlc_exception );
	checkException();

	//Create a media player environement
	p_vlc_media_player = p_libvlc_media_player_new_from_media( p_vlc_media, p_vlc_exception );
	checkException();

	//No need to keep the media now
	//p_libvlc_media_release(p_vlc_media);

	//connectToAllVLCEvents() at the end since it needs p_vlc_media_player
	connectToAllVLCEvents();

	b_play_request_reached = false;

	//Optimization:
	//wait to see if play() is run just after loadMedia()
	//100 milliseconds should be OK
	QTimer::singleShot( 100, this, SLOT( loadMediaInternal() ) );

	//Gets meta data (artist, title...)
	updateMetaData();

	//Updates available audio channels/subtitles/angles/chapters...
	//i.e everything from MediaController
	//There is no audio channel/subtitle/angle/chapter events inside libvlc
	//so let's send our own events...
	//This will reset the GUI
    clearMediaController();
}

void VLCMediaObject::loadMediaInternal()
{
	if( b_play_request_reached )
	{
		//We are already playing the media,
		//so there no need to load it
		return;
	}

	emit stateChanged( Phonon::StoppedState );
}

void VLCMediaObject::setVLCWidgetId()
{
	//Get our media player to use our window
	//FIXME This code does not work inside libvlc!
	//p_libvlc_media_player_set_drawable(p_vlc_media_player, (libvlc_drawable_t) _videoWidgetId, p_vlc_exception);
	//checkException();

	p_libvlc_video_set_parent( p_vlc_instance, (libvlc_drawable_t) i_video_widget_id, p_vlc_exception );
	checkException();
}

void VLCMediaObject::playInternal()
{
	b_play_request_reached = true;

	//Clear subtitles/chapters...
	clearMediaController();

	p_vlc_current_media_player = p_vlc_media_player;

	setVLCWidgetId();

	//Play
	p_libvlc_media_player_play( p_vlc_media_player, p_vlc_exception );
	checkException();
}

void VLCMediaObject::pause()
{
	p_libvlc_media_player_pause( p_vlc_media_player, p_vlc_exception );
	checkException();
}

void VLCMediaObject::stop()
{
	p_libvlc_media_player_stop( p_vlc_media_player, p_vlc_exception );
	checkException();
	//unloadMedia();
}

void VLCMediaObject::seekInternal(qint64 milliseconds)
{
	qDebug() << __FUNCTION__ << milliseconds;
	p_libvlc_media_player_set_time( p_vlc_media_player, milliseconds, p_vlc_exception );
	checkException();
}

QString VLCMediaObject::errorString() const
{
	return p_libvlc_exception_get_message( p_vlc_exception );
}

bool VLCMediaObject::hasVideo() const
{
	return b_has_video;
}

bool VLCMediaObject::isSeekable() const
{
	return b_seekable;
}

void VLCMediaObject::connectToAllVLCEvents()
{
	//MediaPlayer
	p_vlc_media_player_event_manager = p_libvlc_media_player_event_manager( p_vlc_media_player, p_vlc_exception );
	libvlc_event_type_t eventsMediaPlayer[] = {
		libvlc_MediaPlayerPlaying,
		libvlc_MediaPlayerPaused,
		libvlc_MediaPlayerEndReached,
		libvlc_MediaPlayerStopped,
		libvlc_MediaPlayerEncounteredError,
		libvlc_MediaPlayerTimeChanged,
		libvlc_MediaPlayerTitleChanged,
		libvlc_MediaPlayerPositionChanged,
		libvlc_MediaPlayerSeekableChanged,
		libvlc_MediaPlayerPausableChanged,
	};
	int i_nbEvents = sizeof( eventsMediaPlayer ) / sizeof( *eventsMediaPlayer );
	for( int i = 0; i < i_nbEvents; i++ )
	{
		p_libvlc_event_attach( p_vlc_media_player_event_manager, eventsMediaPlayer[i],
		                       libvlc_callback, this, p_vlc_exception );
        checkException();
	}


	//Media
	p_vlc_media_event_manager = p_libvlc_media_event_manager( p_vlc_media, p_vlc_exception );
	libvlc_event_type_t eventsMedia[] = {
		libvlc_MediaMetaChanged,
		libvlc_MediaSubItemAdded,
		libvlc_MediaDurationChanged,
		//FIXME libvlc does not know this event
		//libvlc_MediaPreparsedChanged,
		libvlc_MediaFreed,
		libvlc_MediaStateChanged,
	};
	i_nbEvents = sizeof( eventsMedia ) / sizeof( *eventsMedia );
	for( int i = 0; i < i_nbEvents; i++ )
	{
		p_libvlc_event_attach( p_vlc_media_event_manager, eventsMedia[i], libvlc_callback, this, p_vlc_exception );
		checkException();
	}

	//MediaDiscoverer
	//FIXME why libvlc_media_discoverer_event_manager() does not take a libvlc_exception_t?
	/*
	p_vlc_media_discoverer_event_manager = p_libvlc_media_discoverer_event_manager(p_vlc_media_discoverer);
	libvlc_event_type_t eventsMediaDiscoverer[] = {
		libvlc_MediaDiscovererStarted,
		libvlc_MediaDiscovererEnded
	};
	nbEvents = sizeof(eventsMediaDiscoverer) / sizeof(*eventsMediaDiscoverer);
	for (int i = 0; i < nbEvents; i++) {
		p_libvlc_event_attach(p_vlc_media_discoverer_event_manager, eventsMediaDiscoverer[i], libvlc_callback, this, p_vlc_exception);
	}
	*/
}

void VLCMediaObject::libvlc_callback(const libvlc_event_t *p_event, void *p_user_data)
{
	static int i_first_time_media_player_time_changed = 0;
	static bool b_media_player_title_changed = false;

	VLCMediaObject *p_vlc_mediaObject = (VLCMediaObject *) p_user_data;

	//qDebug() << (int) pp_vlc_mediaObject << "event:" << p_libvlc_event_type_name(event->type);


	/* Media player events */

	if( p_event->type == libvlc_MediaPlayerTimeChanged )
	{

		i_first_time_media_player_time_changed++;

        //FIXME this is ugly, it should be solved by some events in libvlc
		if( i_first_time_media_player_time_changed == 15 )
		{
		    // update metadata
		    p_vlc_mediaObject->updateMetaData();
			//Checks if the file is seekable
			bool b_seekable = p_libvlc_media_player_is_seekable( p_vlc_mediaObject->p_vlc_media_player, p_vlc_exception );
			checkException();
			if( b_seekable != p_vlc_mediaObject->b_seekable )
			{
				qDebug() << "libvlc_callback(): isSeekable:" << b_seekable;
				p_vlc_mediaObject->b_seekable = b_seekable;
				emit p_vlc_mediaObject->seekableChanged( p_vlc_mediaObject->b_seekable );
			}

			//Video width/height
			int i_width = p_libvlc_video_get_width( p_vlc_mediaObject->p_vlc_media_player, p_vlc_exception );
			checkException();
			int i_height = p_libvlc_video_get_height( p_vlc_mediaObject->p_vlc_media_player, p_vlc_exception );
			checkException();
			emit p_vlc_mediaObject->videoWidgetSizeChanged( i_width, i_height);

			//Checks if the file is a video
			bool b_has_video = p_libvlc_media_player_has_vout( p_vlc_mediaObject->p_vlc_media_player, p_vlc_exception );
			checkException();
			if( b_has_video != p_vlc_mediaObject->b_has_video )
			{
				p_vlc_mediaObject->b_has_video = b_has_video;
				emit p_vlc_mediaObject->hasVideoChanged( p_vlc_mediaObject->b_has_video );
			}

            if( b_has_video )
            {
			    // give info about audio tracks
			    p_vlc_mediaObject->refreshAudioChannels();
                // give info about subtitle tracks
                p_vlc_mediaObject->refreshSubtitles();

                // if there is no chapter, then it isnt title/chapter media
                if( p_libvlc_media_player_get_chapter_count( 
                    p_vlc_mediaObject->p_vlc_media_player, p_vlc_exception ) > 0 )
                {
                    checkException();
                    // give info about title
                    // only first time, no when title changed
                    if( !b_media_player_title_changed )
                    { 
                        libvlc_track_description_t *p_info = p_libvlc_video_get_title_description(
                            p_vlc_mediaObject->p_vlc_media_player, p_vlc_exception);
                        checkException();
                        while( p_info )
                        {
                            p_vlc_mediaObject->titleAdded( p_info->i_id, p_info->psz_name );
                            p_info = p_info->p_next;
                        }
                        libvlc_track_description_release( p_info );
                    }

                    // give info about chapters for actual title 0
                    if( b_media_player_title_changed )
                        p_vlc_mediaObject->refreshChapters( p_libvlc_media_player_get_title(
                            p_vlc_mediaObject->p_vlc_media_player, p_vlc_exception) );
                    else
                        p_vlc_mediaObject->refreshChapters( 0 );
                }
                if( b_media_player_title_changed )
                    b_media_player_title_changed = false;
            }

			//Bugfix with mediaplayer example from Trolltech
			//Now we are in playing state
			emit p_vlc_mediaObject->stateChanged( Phonon::PlayingState );
		}

		emit p_vlc_mediaObject->tickInternal( p_vlc_mediaObject->currentTime() );
	}

	if( p_event->type == libvlc_MediaPlayerPlaying )
	{
		if( p_vlc_mediaObject->state() != Phonon::LoadingState )
		{
			//Bugfix with mediaplayer example from Trolltech
			emit p_vlc_mediaObject->stateChanged( Phonon::PlayingState );
		}
	}

	if( p_event->type == libvlc_MediaPlayerPaused )
	{
		emit p_vlc_mediaObject->stateChanged( Phonon::PausedState );
	}

	if( p_event->type == libvlc_MediaPlayerEndReached )
	{
		i_first_time_media_player_time_changed = 0;
		p_vlc_mediaObject->clearMediaController();
		emit p_vlc_mediaObject->stateChanged( Phonon::StoppedState );
		emit p_vlc_mediaObject->finished();
	}

	if( p_event->type == libvlc_MediaPlayerStopped )
	{
		i_first_time_media_player_time_changed = 0;
		p_vlc_mediaObject->clearMediaController();
		emit p_vlc_mediaObject->stateChanged( Phonon::StoppedState );
	}

	if( p_event->type == libvlc_MediaPlayerTitleChanged )
	{
		i_first_time_media_player_time_changed = 0;
		b_media_player_title_changed = true;
	}

	/* Media events */

	if( p_event->type == libvlc_MediaDurationChanged )
	{
		//Checks if the file total time changed
		libvlc_time_t totalTime = p_libvlc_media_get_duration( p_vlc_mediaObject->p_vlc_media, p_vlc_exception );
		checkException();
		totalTime = totalTime / VLC_POSITION_RESOLUTION;

		if( totalTime != p_vlc_mediaObject->i_total_time )
		{
			p_vlc_mediaObject->i_total_time = totalTime;
			emit p_vlc_mediaObject->totalTimeChanged( p_vlc_mediaObject->i_total_time );
		}
	}

	if( p_event->type == libvlc_MediaMetaChanged )
	{
	}
}

void VLCMediaObject::updateMetaData()
{
	QMultiMap<QString, QString> metaDataMap;

	metaDataMap.insert( QLatin1String( "ARTIST" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Artist, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "ALBUM" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Album, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "TITLE" ),
                        QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Title, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "DATE" ), 
                        QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Date, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "GENRE" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Genre, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "TRACKNUMBER" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_TrackNumber, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "DESCRIPTION" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Description, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "COPYRIGHT" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_TrackNumber, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "URL" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_URL, p_vlc_exception )));
	checkException();
	metaDataMap.insert( QLatin1String( "ENCODEDBY" ),
	                    QString::fromUtf8( p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_EncodedBy, p_vlc_exception )));

	qDebug() << "updateMetaData(): artist:"
	         << p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Artist, p_vlc_exception );
    checkException();
    qDebug() << "updateMetaData(): title:"
	         << p_libvlc_media_get_meta( p_vlc_media, libvlc_meta_Title, p_vlc_exception );
	checkException();

	emit metaDataChanged( metaDataMap );
}

qint64 VLCMediaObject::totalTime() const
{
	return i_total_time;
}

qint64 VLCMediaObject::currentTimeInternal() const
{
	libvlc_time_t time = p_libvlc_media_player_get_time( p_vlc_media_player, p_vlc_exception );
	checkException();

	return time;
}

}}	//Namespace Phonon::VLC
