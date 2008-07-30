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

#include "EffectManager.h"
#include "vlc/vlc_loader.h"
#include "vlc/vlc_symbols.h"

namespace Phonon
{
namespace VLC_MPlayer
{

EffectManager::EffectManager(QObject * parent)
	: QObject(parent) {
	    
    b_equalizer_enabled = false;

#ifdef PHONON_MPLAYER
	//Audio effects
	_effectList.append(new EffectInfo("(Audio) Karaoke", "karaoke", EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Extra stereo", "extrastereo", EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Volume normalization", "volnorm=2", EffectInfo::AudioEffect));

	//Deinterlace
	_effectList.append(new EffectInfo("(Video) Deinterlace: Lowpass5", "pp=l5", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Deinterlace: Yadif (normal)", "yadif", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Deinterlace: Yadif (double framerate)", "yadif=1", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Deinterlace: Linear Blend", "pp=lb", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Deinterlace: Kerndeint", "kerndeint=5", EffectInfo::VideoEffect));

	//Rotate
	_effectList.append(new EffectInfo("(Video) Rotate: clockwise flip", "rotate=0", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Rotate: clockwise", "rotate=1", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Rotate: counter clockwise", "rotate=2", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Rotate: counter clockwise flip", "rotate=3", EffectInfo::VideoEffect));

	//Denoise
	_effectList.append(new EffectInfo("(Video) Denoise: soft", "hqdn3d=2:1:2", EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Denoise: normal", "hqdn3d", EffectInfo::VideoEffect));

	//Deblock
	_effectList.append(new EffectInfo("(Video) Deblock", "pp=vb/hb", EffectInfo::VideoEffect));

	//Dering
	_effectList.append(new EffectInfo("(Video) Dering", "pp=dr", EffectInfo::VideoEffect));

	//Addnoise
	_effectList.append(new EffectInfo("(Video) Addnoise", "noise=9ah:5ah", EffectInfo::VideoEffect));

	//Postprocessing
	//FIXME does not work
	//_effectList.append(new EffectInfo("(Video) Postprocessing", "pp -autoq 6", EffectInfo::VideoEffect));
#endif	//PHONON_MPLAYER

#ifdef PHONON_VLC
	/* Audio effects - equalizer
	   only one of them can be used => last set
	   it is clever used with combobox */
	_effectList.append(new EffectInfo("(Audio) Flat", FLAT, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Classical", CLASSICAL, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Club", CLUB, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Dance", DANCE, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Fullbass", FULLBASS, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Fullbasstreble", FULLBASSTREBLE, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Fulltreble", FULLTREBLE, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Headphones", HEADPHONES, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Large hall", LARGEHALL, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Live", LIVE, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Party", PARTY, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Pop", POP, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Reggae", REGGAE, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Rock", ROCK, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Ska", SKA, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Soft", SOFT, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Softrock", SOFTROCK, EffectInfo::AudioEffect));
	_effectList.append(new EffectInfo("(Audio) Techno", TECHNO, EffectInfo::AudioEffect));
	
	/* Video effects, more than one can be used simultaneously
	   clever to used with checboxes */
	_effectList.append(new EffectInfo("(Video) Color threshold", COLORTHRESHOLD, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Extract", EXTRACT, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Gradient", GRADIENT, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Invert", INVERT, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Motion blur", MOTIONBLUR, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Motion detect", MOTIONDETECT, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Noise", NOISE, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Postprocess", POSTPROCESS, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Psychedelic", PSYCHEDELIC, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Ripple", RIPPLE, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Rotate", ROTATE, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Sharpen", SHARPEN, EffectInfo::VideoEffect));
	_effectList.append(new EffectInfo("(Video) Wave", WAVE, EffectInfo::VideoEffect));
#endif
}

EffectManager::~EffectManager() {
	_effectList.clear();
}

void EffectManager::enableEqualizerEffects()
{
    if( !b_equalizer_enabled )
    {
        libvlc_audio_equalizer_enable( _vlcInstance, _vlcException );
        checkException();
        b_equalizer_enabled = true;
    }
}

void EffectManager::disableEqualizerEffects()
{
    if( b_equalizer_enabled )
    {
        libvlc_audio_equalizer_disable( _vlcInstance, _vlcException );
        checkException();
        b_equalizer_enabled = false;
    }
}

QList<EffectInfo *> EffectManager::getEffectList() const {
	return _effectList;
}

}}	//Namespace Phonon::VLC_MPlayer
