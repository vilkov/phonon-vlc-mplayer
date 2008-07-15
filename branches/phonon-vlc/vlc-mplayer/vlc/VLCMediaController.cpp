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

#include "VLCMediaController.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

namespace Phonon
{
namespace VLC_MPlayer
{

VLCMediaController::VLCMediaController()
	: MediaController() {

	_vlcMediaPlayer = NULL;
}

VLCMediaController::~VLCMediaController() {
}

void VLCMediaController::clearMediaController() {
	_currentAudioChannel = Phonon::AudioChannelDescription();
	_availableAudioChannels.clear();

	_currentSubtitle = Phonon::SubtitleDescription();
	_availableSubtitles.clear();

	_currentAngle = 0;
	_availableAngles = 0;

	_currentChapter = Phonon::ChapterDescription();
	_availableChapters.clear();

	_currentTitle = Phonon::TitleDescription();
	_availableTitles.clear();

	_autoplayTitles = false;

    emit availableAudioChannelsChanged();
    emit availableSubtitlesChanged();
    emit availableTitlesChanged();
    emit availableChaptersChanged();
}

/* add audio channel -> in libvlc it is track, it means audio in another
   language
 */
void VLCMediaController::audioChannelAdded(int id, const QString & lang)
{
    qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", "");

	_availableAudioChannels << Phonon::AudioChannelDescription(id, properties);    
	emit availableAudioChannelsChanged();
}

/* add subtitle */
void VLCMediaController::subtitleAdded(int id, const QString & lang, const QString & type)
{
	qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", lang);
	properties.insert("description", "");
	properties.insert("type", "");

	_availableSubtitles << Phonon::SubtitleDescription(id, properties);
	emit availableSubtitlesChanged();
}

/* add title */
void VLCMediaController::titleAdded(int id, const QString & name)
{
    qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
	properties.insert("name", name);
	properties.insert("description", "");

	_availableTitles << Phonon::TitleDescription(id, properties);
	emit availableTitlesChanged();
}

/* add chapter */
void VLCMediaController::chapterAdded(int titleId, const QString & name)
{
    qDebug() << __FUNCTION__;

	QHash<QByteArray, QVariant> properties;
    properties.insert("name", name);
	properties.insert("description", "");

	_availableChapters << Phonon::ChapterDescription(titleId, properties);
	emit availableChaptersChanged();
}

//AudioChannel
void VLCMediaController::setCurrentAudioChannel(const Phonon::AudioChannelDescription & audioChannel) {
	qDebug() << __FUNCTION__;

	_currentAudioChannel = audioChannel;
    p_libvlc_audio_set_track(_vlcMediaPlayer, audioChannel.index(), _vlcException);
    checkException();
}

QList<Phonon::AudioChannelDescription> VLCMediaController::availableAudioChannels() const {
	return _availableAudioChannels;
}

Phonon::AudioChannelDescription VLCMediaController::currentAudioChannel() const {
	return _currentAudioChannel;
}

void VLCMediaController::refreshAudioChannels()
{
    _currentAudioChannel = Phonon::AudioChannelDescription();
    _availableAudioChannels.clear();

	libvlc_track_description_t * p_info = p_libvlc_audio_get_track_description(
	        _vlcMediaPlayer, _vlcException);
    checkException();
    while (p_info)
    {
        audioChannelAdded(p_info->i_id, p_info->psz_name);
        p_info = p_info->p_next;
    }
    libvlc_track_description_release( p_info );
}

//Subtitle
void VLCMediaController::setCurrentSubtitle(const Phonon::SubtitleDescription & subtitle) {
	qDebug() << __FUNCTION__;

	_currentSubtitle = subtitle;
	int id = _currentSubtitle.index();
	QString type = _currentSubtitle.property("type").toString();

	if (type == "file") {
		QString filename = _currentSubtitle.property("name").toString();
		if (!filename.isEmpty()) {
			p_libvlc_video_set_subtitle_file(_vlcMediaPlayer, filename.toAscii().data(), _vlcException);

			//There is no subtitle event inside libvlc
			//so let's send our own event...
			_availableSubtitles << _currentSubtitle;
			emit availableSubtitlesChanged();
		}
	}
	else
	{
        p_libvlc_video_set_spu(_vlcMediaPlayer, subtitle.index(), _vlcException);
        checkException();
	}
}

QList<Phonon::SubtitleDescription> VLCMediaController::availableSubtitles() const {
	return _availableSubtitles;
}

Phonon::SubtitleDescription VLCMediaController::currentSubtitle() const {
	return _currentSubtitle;
}

void VLCMediaController::refreshSubtitles()
{
    _currentSubtitle = Phonon::SubtitleDescription();
    _availableSubtitles.clear();

    libvlc_track_description_t *p_info = p_libvlc_video_get_spu_description(
        _vlcMediaPlayer, _vlcException);
    checkException();
    while (p_info)
    {
        subtitleAdded(p_info->i_id, p_info->psz_name, "");
        p_info = p_info->p_next;
    }
    libvlc_track_description_release( p_info );
}

//Title

void VLCMediaController::setCurrentTitle(const Phonon::TitleDescription & title) {
	_currentTitle = title;
	
	p_libvlc_media_player_set_title(_vlcMediaPlayer, title.index(), _vlcException);
	checkException();
}

QList<Phonon::TitleDescription> VLCMediaController::availableTitles() const {
	return _availableTitles;
}

Phonon::TitleDescription VLCMediaController::currentTitle() const {
	return _currentTitle;
}

void VLCMediaController::setAutoplayTitles(bool autoplay) {
	_autoplayTitles = autoplay;
}

bool VLCMediaController::autoplayTitles() const {
	return _autoplayTitles;
}

//Chapter

void VLCMediaController::setCurrentChapter(const Phonon::ChapterDescription & chapter) {
	_currentChapter = chapter;
    p_libvlc_media_player_set_chapter(_vlcMediaPlayer, chapter.index(), _vlcException);
    checkException();
}

QList<Phonon::ChapterDescription> VLCMediaController::availableChapters() const {
	return _availableChapters;
}

Phonon::ChapterDescription VLCMediaController::currentChapter() const {
	return _currentChapter;
}

void VLCMediaController::refreshChapters(int i_title)
{
    _currentChapter = Phonon::ChapterDescription();
    _availableChapters.clear();

    // give info about chapters for actual title
    libvlc_track_description_t *p_info = p_libvlc_video_get_chapter_description(
        _vlcMediaPlayer, i_title, _vlcException);
    checkException();
    while (p_info)
    {
        chapterAdded(p_info->i_id, p_info->psz_name);
        p_info = p_info->p_next;
    }
    libvlc_track_description_release( p_info );
}


//Angle
void VLCMediaController::setCurrentAngle(int angleNumber) {
}

int VLCMediaController::availableAngles() const {
	return _availableAngles;
}

int VLCMediaController::currentAngle() const {
	return _currentAngle;
}

}}	//Namespace Phonon::VLC_MPlayer
