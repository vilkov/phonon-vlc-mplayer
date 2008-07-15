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

#ifndef PHONON_VLC_MPLAYER_VLCMEDIACONTROLLER_H
#define PHONON_VLC_MPLAYER_VLCMEDIACONTROLLER_H

#include "../MediaController.h"

#include <vlc/vlc.h>

namespace Phonon
{
namespace VLC_MPlayer
{

/**
 * MediaController specific code for VLC.
 *
 * @author Tanguy Krotoff
 */
class VLCMediaController : public MediaController {
public:

	VLCMediaController();
	virtual ~VLCMediaController();

    void audioChannelAdded(int id, const QString & lang);
	void subtitleAdded(int id, const QString & lang, const QString & type);
	void titleAdded(int id, const QString & name);
	void chapterAdded(int titleId, const QString & name);
	

protected:
    virtual void clearMediaController();

	//AudioChannel
	void setCurrentAudioChannel(const Phonon::AudioChannelDescription & audioChannel);
	QList<Phonon::AudioChannelDescription> availableAudioChannels() const;
	Phonon::AudioChannelDescription currentAudioChannel() const;
	void refreshAudioChannels();

	//Subtitle
	void setCurrentSubtitle(const Phonon::SubtitleDescription & subtitle);
	QList<Phonon::SubtitleDescription> availableSubtitles() const;
	Phonon::SubtitleDescription currentSubtitle() const;
	void refreshSubtitles();

	//Angle
	void setCurrentAngle(int angleNumber);
	int availableAngles() const;
	int currentAngle() const;

	//Chapter
	void setCurrentChapter(const Phonon::ChapterDescription & chapter);
	QList<Phonon::ChapterDescription> availableChapters() const;
	Phonon::ChapterDescription currentChapter() const;
	void refreshChapters(int i_title);

	//Title
	void setCurrentTitle(const Phonon::TitleDescription & title);
	QList<Phonon::TitleDescription> availableTitles() const;
	Phonon::TitleDescription currentTitle() const;

	void setAutoplayTitles(bool autoplay);
	bool autoplayTitles() const;

	//MediaPlayer
	libvlc_media_player_t * _vlcMediaPlayer;

private:
};

}}	//Namespace Phonon::VLC_MPlayer

#endif	//PHONON_VLC_MPLAYER_VLCMEDIACONTROLLER_H
