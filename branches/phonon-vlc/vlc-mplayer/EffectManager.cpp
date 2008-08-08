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

#include "EffectManager.h"
#include "vlc/vlc_loader.h"
#include "vlc/vlc_symbols.h"

namespace Phonon
{
namespace VLC
{

EffectManager::EffectManager( QObject *p_parent )
	: QObject( p_parent )
{
	    
    b_equalizer_enabled = false;

	/* Audio effects - equalizer
	   only one of them can be used => last set
	   it is clever used with combobox */
	effectList.append( new EffectInfo( "(Audio) Flat", FLAT, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Classical", CLASSICAL, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Club", CLUB, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Dance", DANCE, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Fullbass", FULLBASS, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Fullbasstreble", FULLBASSTREBLE, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Fulltreble", FULLTREBLE, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Headphones", HEADPHONES, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Large hall", LARGEHALL, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Live", LIVE, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Party", PARTY, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Pop", POP, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Reggae", REGGAE, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Rock", ROCK, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Ska", SKA, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Soft", SOFT, EffectInfo::AudioEffect ) );
	effectList.append( new EffectInfo( "(Audio) Softrock", SOFTROCK, EffectInfo::AudioEffect)  );
	effectList.append( new EffectInfo( "(Audio) Techno", TECHNO, EffectInfo::AudioEffect ) );
	
	/* Video effects, more than one can be used simultaneously
	   clever to used with checboxes */
	effectList.append( new EffectInfo( "(Video) Color threshold", COLORTHRESHOLD, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Extract", EXTRACT, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Gradient", GRADIENT, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Invert", INVERT, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Motion blur", MOTIONBLUR, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Motion detect", MOTIONDETECT, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Noise", NOISE, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Postprocess", POSTPROCESS, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Psychedelic", PSYCHEDELIC, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Ripple", RIPPLE, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Rotate", ROTATE, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Sharpen", SHARPEN, EffectInfo::VideoEffect ) );
	effectList.append( new EffectInfo( "(Video) Wave", WAVE, EffectInfo::VideoEffect ) );
}

EffectManager::~EffectManager()
{
	effectList.clear();
}

void EffectManager::enableEqualizerEffects()
{
    if( !b_equalizer_enabled )
    {
        b_equalizer_enabled = libvlc_audio_equalizer_enable( p_vlc_instance, p_vlc_exception );
        checkException();
    }
}

void EffectManager::disableEqualizerEffects()
{
    if( b_equalizer_enabled )
    {
        if( libvlc_audio_equalizer_disable( p_vlc_instance, p_vlc_exception ) )
            b_equalizer_enabled = false;
        checkException();
    }
}

QList<EffectInfo *> EffectManager::getEffectList() const
{
	return effectList;
}

}}	//Namespace Phonon::VLC
