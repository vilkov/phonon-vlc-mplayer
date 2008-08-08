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

#include "Effect.h"

#include "EffectManager.h"
#include "vlc/vlc_loader.h"
#include "vlc/vlc_symbols.h"

#include "MediaObject.h"

namespace Phonon
{
namespace VLC
{

Effect::Effect( EffectManager *p_em, int i_effectId, QObject *p_parent )
	: SinkNode( p_parent )
{
    p_effectManager = p_em;
	QList<EffectInfo *> effects = p_effectManager->getEffectList();

	if( i_effectId >= 0 && i_effectId < effects.size())
	{
		i_effect_filter = effects[ i_effectId ]->getFilter();
		effect_type = effects[ i_effectId ]->getType();
	}
	else
	{
		//Effect ID out of range
		Q_ASSERT(0);
	}
}

Effect::~Effect()
{
}

void Effect::connectToMediaObject( PrivateMediaObject *p_media_object )
{
	SinkNode::connectToMediaObject( p_media_object );

	switch ( effect_type )
	{
        case EffectInfo::AudioEffect:
            p_effectManager->enableEqualizerEffects();
            p_libvlc_audio_equalizer_set_preset( p_vlc_instance, ( libvlc_audio_preset_names_t ) i_effect_filter,
                                                 p_vlc_exception );
            checkException();
            break;
        case EffectInfo::VideoEffect:
            p_libvlc_video_filter_add( p_vlc_current_media_player, ( libvlc_video_filter_names_t ) i_effect_filter,
                                       p_vlc_exception);
            checkException();
            break;
	}
}

void Effect::disconnectFromMediaObject( PrivateMediaObject *p_media_object )
{
	SinkNode::disconnectFromMediaObject( p_media_object );

	switch ( effect_type )
	{
        case EffectInfo::AudioEffect:
            break;
        case EffectInfo::VideoEffect:
            p_libvlc_video_filter_remove( p_vlc_current_media_player, ( libvlc_video_filter_names_t ) i_effect_filter,
                                          p_vlc_exception );
            checkException();
            break;
	}
}

QList<EffectParameter> Effect::parameters() const
{
	QList<EffectParameter> params;
	return params;
}

QVariant Effect::parameterValue(const EffectParameter & param) const
{
	return new QVariant();
}

void Effect::setParameterValue(const EffectParameter & param, const QVariant & newValue)
{
}

}}	//Namespace Phonon::VLC
