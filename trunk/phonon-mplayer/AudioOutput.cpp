/*
 * MPlayer backend for the Phonon library
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

#include "AudioOutput.h"

#include "Backend.h"

#include "smplayer/core.h"

namespace Phonon
{
namespace MPlayer
{

AudioOutput::AudioOutput(QObject * parent)
	: QObject(parent) {

	_volume = 0;

	connect(Backend::getSMPlayerCore(), SIGNAL(volumeChanged(int)),
		SLOT(volumeChangedSlotInternal(int)));
}

AudioOutput::~AudioOutput() {
}

qreal AudioOutput::volume() const {
	return _volume;
}

void AudioOutput::setVolume(qreal volume) {
	//Core::setVolume() varies from 0 to 100
	Backend::getSMPlayerCore()->setVolume(volume * 100);
}

int AudioOutput::outputDevice() const {
	return 0;
}

bool AudioOutput::setOutputDevice(int device) {
	Backend::getSMPlayerCore()->changeAudio(device);
	return true;
}

void AudioOutput::volumeChangedSlotInternal(int volume) {
	//volume varies from 0 to 100
	_volume = volume / 100.0;
	emit volumeChanged(_volume);
}

}}	//Namespace Phonon::MPlayer
