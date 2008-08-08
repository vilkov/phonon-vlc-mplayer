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

#include "AudioOutput.h"
#include "DeviceManager.h"
#include "Backend.h"

#include "MediaObject.h"
#include "VLCMediaObject.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

namespace Phonon
{
namespace VLC
{

AudioOutput::AudioOutput( Backend *p_back, QObject * p_parent )
	: SinkNode( p_parent ),
      f_volume( 1.0 ),
	  p_backend( p_back )
{
    p_media_object = NULL;
}

AudioOutput::~AudioOutput() 
{
}

qreal AudioOutput::volume() const
{
	return f_volume;
}

void AudioOutput::setVolume( qreal volume ) 
{
    if( p_vlc_instance )
    {
		p_libvlc_audio_set_volume( p_vlc_instance, (int) ( f_volume * 100 ), p_vlc_exception );
		checkException();
		f_volume = volume;
		emit volumeChanged( f_volume );
	}
}

int AudioOutput::outputDevice() const
{
	return i_device;
}

bool AudioOutput::setOutputDevice( int device )
{
    if( i_device == device )
        return true;

    const QList<AudioDevice> deviceList = p_backend->getDeviceManager()->audioOutputDevices();
    if( device >= 0 && device < deviceList.size() )
    {
        i_device = device;
        const QByteArray deviceName = deviceList.at( device ).nameId;
        if( deviceName == DEFAULT_ID )
        {
            libvlc_audio_device_set( p_vlc_instance, DEFAULT, p_vlc_exception );
            checkException();
        }
        else if( deviceName.startsWith( ALSA_ID ) )
        {
            printf("setting ALSA %s\n", deviceList.at( device ).hwId.data() );
            libvlc_audio_device_set( p_vlc_instance, ALSA, p_vlc_exception );
            checkException();
            libvlc_audio_alsa_device_set( p_vlc_instance, deviceList.at( device ).hwId, p_vlc_exception );
            checkException();
        }
        else if( deviceName == OSS_ID )
        {
            libvlc_audio_device_set( p_vlc_instance, OSS, p_vlc_exception );
            checkException();
        }
        else if( deviceName == SDL_ID )
        {
            libvlc_audio_device_set( p_vlc_instance, SDL, p_vlc_exception );
            checkException();
        }
        else if( deviceName == DIRECTX_ID )
        {
            libvlc_audio_device_set( p_vlc_instance, DIRECTX, p_vlc_exception );
            checkException();
        }
        else
            return false;
    }
	return true;
}

#if (PHONON_VERSION >= PHONON_VERSION_CHECK(4, 2, 0))
bool AudioOutput::setOutputDevice( const Phonon::AudioOutputDevice & device )
{
	return true;
}
#endif

}}	//Namespace Phonon::VLC
