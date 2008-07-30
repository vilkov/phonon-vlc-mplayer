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

#include "Effect.h"

#include "EffectManager.h"
#include "vlc/vlc_loader.h"
#include "vlc/vlc_symbols.h"

#include "MediaObject.h"

#ifdef PHONON_MPLAYER
	#include "MPlayerMediaObject.h"

	#include <mplayer/MPlayerLoader.h>
	#include <mplayer/MediaSettings.h>
	#include <mplayer/MPlayerProcess.h>
#endif	//PHONON_MPLAYER

namespace Phonon
{
namespace VLC_MPlayer
{

Effect::Effect(EffectManager * effectManager, int effectId, QObject * parent)
	: SinkNode(parent) {

    effectManager->enableEqualizerEffects();
	QList<EffectInfo *> effects = effectManager->getEffectList();
	if (effectId >= 0 && effectId < effects.size()) {
		_effectFilter = effects[effectId]->getFilter();
		_effectType = effects[effectId]->getType();
	} else {
		//Effect ID out of range
		Q_ASSERT(0);
	}
}

Effect::~Effect() {
}

void Effect::connectToMediaObject(PrivateMediaObject * mediaObject) {
	SinkNode::connectToMediaObject(mediaObject);
#ifdef PHONON_MPLAYER
	switch (_effectType) {
	case EffectInfo::AudioEffect:
		MPlayerLoader::settings.audioFilters.append(_effectCommand);
		break;
	case EffectInfo::VideoEffect:
		MPlayerLoader::settings.videoFilters.append(_effectCommand);
		break;
	}

	MPlayerProcess * process = _mediaObject->getMPlayerProcess();
	MPlayerLoader::restart(process);
#endif	//PHONON_MPLAYER

#ifdef PHONON_VLC
	switch (_effectType) {
	case EffectInfo::AudioEffect:
		p_libvlc_audio_equalizer_set_preset( _vlcInstance, ( libvlc_audio_preset_names_t )_effectFilter, _vlcException);
		checkException();
		break;
	case EffectInfo::VideoEffect:
		p_libvlc_video_filter_add( _vlcCurrentMediaPlayer, ( libvlc_video_filter_names_t ) _effectFilter, _vlcException);
		checkException();
		break;
	}
#endif
}

void Effect::disconnectFromMediaObject(PrivateMediaObject * mediaObject) {
	SinkNode::disconnectFromMediaObject(mediaObject);

#ifdef PHONON_MPLAYER
	switch (_effectType) {
	case EffectInfo::AudioEffect:
		MPlayerLoader::settings.audioFilters.removeAll(_effectCommand);
		break;
	case EffectInfo::VideoEffect:
		MPlayerLoader::settings.videoFilters.removeAll(_effectCommand);
		break;
	}

	MPlayerProcess * process = _mediaObject->getMPlayerProcess();
	MPlayerLoader::restart(process);
#endif	//PHONON_MPLAYER

#ifdef PHONON_VLC
    printf("deleting: \n");
	switch (_effectType) {
	case EffectInfo::AudioEffect:
		break;
	case EffectInfo::VideoEffect:
		p_libvlc_video_filter_remove( _vlcCurrentMediaPlayer, ( libvlc_video_filter_names_t ) _effectFilter, _vlcException);
		checkException();
		break;
	}
#endif
}

QList<EffectParameter> Effect::parameters() const {
	QList<EffectParameter> params;
	return params;
}

QVariant Effect::parameterValue(const EffectParameter & param) const {
	return new QVariant();
}

void Effect::setParameterValue(const EffectParameter & param, const QVariant & newValue) {
}

}}	//Namespace Phonon::VLC_MPlayer
