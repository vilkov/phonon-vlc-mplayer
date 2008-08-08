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

#include "VLCMediaController.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

namespace Phonon
{
namespace VLC
{

VLCMediaController::VLCMediaController()
	: MediaController()
{
	p_vlc_media_player = NULL;
}

VLCMediaController::~VLCMediaController()
{
}

void VLCMediaController::clearMediaController()
{
	current_audio_channel = Phonon::AudioChannelDescription();
	available_audio_channels.clear();

	current_subtitle = Phonon::SubtitleDescription();
	available_subtitles.clear();

	i_current_angle = 0;
	i_available_angles = 0;

	//current_chapter = Phonon::ChapterDescription();
	//available_chapters.clear();
	current_chapter = 0;
	available_chapters = 0;
	
	//current_title = Phonon::TitleDescription();
	//available_titles.clear();
	current_title = 0;
	available_titles = 0;

	b_autoplay_titles = false;

    emit availableAudioChannelsChanged();
    emit availableSubtitlesChanged();
    emit availableTitlesChanged();
    emit availableChaptersChanged();
}

/* add audio channel -> in libvlc it is track, it means audio in another
   language
 */
void VLCMediaController::audioChannelAdded( int id, const QString & lang )
{
	QHash<QByteArray, QVariant> properties;
	properties.insert( "name", lang );
	properties.insert( "description", "" );

	available_audio_channels << Phonon::AudioChannelDescription( id, properties );
	emit availableAudioChannelsChanged();
}

/* add subtitle */
void VLCMediaController::subtitleAdded( int id, const QString & lang, const QString & type )
{
	QHash<QByteArray, QVariant> properties;
	properties.insert( "name", lang );
	properties.insert( "description", "" );
	properties.insert( "type", type );

	available_subtitles << Phonon::SubtitleDescription( id, properties );
	emit availableSubtitlesChanged();
}

/* add title */
void VLCMediaController::titleAdded( int id, const QString & name )
{
	//QHash<QByteArray, QVariant> properties;
	//properties.insert( "name", name );
	//properties.insert( "description", "" );

	//available_titles << Phonon::TitleDescription( id, properties );
	available_titles++;
	emit availableTitlesChanged();
}

/* add chapter */
void VLCMediaController::chapterAdded( int titleId, const QString & name )
{
	//QHash<QByteArray, QVariant> properties;
    //properties.insert( "name", name );
	//properties.insert( "description", "" );

	//available_chapters << Phonon::ChapterDescription( titleId, properties );
	available_chapters++;
	emit availableChaptersChanged();
}

//AudioChannel
void VLCMediaController::setCurrentAudioChannel( const Phonon::AudioChannelDescription & audioChannel )
{
	current_audio_channel = audioChannel;
    p_libvlc_audio_set_track( p_vlc_media_player, audioChannel.index(), p_vlc_exception );
    checkException();
}

QList<Phonon::AudioChannelDescription> VLCMediaController::availableAudioChannels() const
{
	return available_audio_channels;
}

Phonon::AudioChannelDescription VLCMediaController::currentAudioChannel() const
{
	return current_audio_channel;
}

void VLCMediaController::refreshAudioChannels()
{
    current_audio_channel = Phonon::AudioChannelDescription();
    available_audio_channels.clear();

	libvlc_track_description_t * p_info = p_libvlc_audio_get_track_description(
	        p_vlc_media_player, p_vlc_exception );
    checkException();
    while( p_info )
    {
        audioChannelAdded( p_info->i_id, p_info->psz_name );
        p_info = p_info->p_next;
    }
    libvlc_track_description_release( p_info );
}

//Subtitle
void VLCMediaController::setCurrentSubtitle( const Phonon::SubtitleDescription & subtitle )
{
	current_subtitle = subtitle;
	int id = current_subtitle.index();
	QString type = current_subtitle.property("type").toString();

	if( type == "file" )
	{
		QString filename = current_subtitle.property("name").toString();
		if( !filename.isEmpty() )
		{
			p_libvlc_video_set_subtitle_file( p_vlc_media_player, filename.toAscii().data(), p_vlc_exception );
			checkException();

			//There is no subtitle event inside libvlc
			//so let's send our own event...
			available_subtitles << current_subtitle;
			emit availableSubtitlesChanged();
		}
	}
	else
	{
        p_libvlc_video_set_spu( p_vlc_media_player, subtitle.index(), p_vlc_exception );
        checkException();
	}
}

QList<Phonon::SubtitleDescription> VLCMediaController::availableSubtitles() const
{
	return available_subtitles;
}

Phonon::SubtitleDescription VLCMediaController::currentSubtitle() const
{
	return current_subtitle;
}

void VLCMediaController::refreshSubtitles()
{
    current_subtitle = Phonon::SubtitleDescription();
    available_subtitles.clear();

    libvlc_track_description_t *p_info = p_libvlc_video_get_spu_description(
        p_vlc_media_player, p_vlc_exception );
    checkException();
    while(p_info)
    {
        subtitleAdded( p_info->i_id, p_info->psz_name, "" );
        p_info = p_info->p_next;
    }
    libvlc_track_description_release( p_info );
}

//Title

//void VLCMediaController::setCurrentTitle( const Phonon::TitleDescription & title )
void VLCMediaController::setCurrentTitle( int title )
{
	current_title = title;
	
	//p_libvlc_media_player_set_title( p_vlc_media_player, title.index(), p_vlc_exception );
	p_libvlc_media_player_set_title( p_vlc_media_player, title, p_vlc_exception );
	checkException();
}

//QList<Phonon::TitleDescription> VLCMediaController::availableTitles() const
int VLCMediaController::availableTitles() const
{
	return available_titles;
}

//Phonon::TitleDescription VLCMediaController::currentTitle() const
int VLCMediaController::currentTitle() const
{
	return current_title;
}

void VLCMediaController::setAutoplayTitles( bool autoplay )
{
	b_autoplay_titles = autoplay;
}

bool VLCMediaController::autoplayTitles() const {
	return b_autoplay_titles;
}

/* Chapter */
//void VLCMediaController::setCurrentChapter( const Phonon::ChapterDescription & chapter )
void VLCMediaController::setCurrentChapter( int chapter )
{
	current_chapter = chapter;
    //p_libvlc_media_player_set_chapter( p_vlc_media_player, chapter.index(), p_vlc_exception );
    p_libvlc_media_player_set_chapter( p_vlc_media_player, chapter, p_vlc_exception );
    checkException();
}

//QList<Phonon::ChapterDescription> VLCMediaController::availableChapters() const
int VLCMediaController::availableChapters() const
{
	return available_chapters;
}

//Phonon::ChapterDescription VLCMediaController::currentChapter() const
int VLCMediaController::currentChapter() const
{
	return current_chapter;
}

/* when title is changed, we need to rebuild available chapters */
void VLCMediaController::refreshChapters( int i_title )
{
    //current_chapter = Phonon::ChapterDescription();
    //available_chapters.clear();
    current_chapter = 0;
    available_chapters = 0;

    // give info about chapters for actual title
    libvlc_track_description_t *p_info = p_libvlc_video_get_chapter_description(
        p_vlc_media_player, i_title, p_vlc_exception );
    checkException();
    while( p_info )
    {
        chapterAdded( p_info->i_id, p_info->psz_name );
        p_info = p_info->p_next;
    }
    libvlc_track_description_release( p_info );
}


/* Angle */
void VLCMediaController::setCurrentAngle( int angleNumber )
{
    i_current_angle = angleNumber;
}

int VLCMediaController::availableAngles() const
{
	return i_available_angles;
}

int VLCMediaController::currentAngle() const
{
	return i_current_angle;
}

}}	//Namespace Phonon::VLC
