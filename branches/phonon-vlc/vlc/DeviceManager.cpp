/*  This file is part of the KDE project.

    Copyright (C) 2007 Trolltech ASA. All rights reserved.

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 or 3 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DeviceManager.h"
#include "Backend.h"
//#include "videowidget.h"
//#include "widgetrenderer.h"
#include "vlc_loader.h"

/*
 * This class manages the list of currently
 * active output devices
 */

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace VLC
{

AudioDevice::AudioDevice( DeviceManager *p_manager, const QByteArray &name_id, const QByteArray &hw_id )
{
    //get an id
    static int counter = 0;
    id = counter++;
    //get name from device
    if( nameId == "default" )
        description = "Default audio device";
    else
    {
        nameId = name_id;
        description = "";
    }
    hwId = hw_id;
}

DeviceManager::DeviceManager( Backend *p_parent )
        : QObject( p_parent )
        , p_backend( p_parent )
{
    updateDeviceList();
}

DeviceManager::~DeviceManager()
{
    audioDeviceList.clear();
}

bool DeviceManager::canOpenDevice( ) const
{
    return true;
}


/*
 * Returns a positive device id or -1 if device
 * does not exist
 */
int DeviceManager::deviceId( const QByteArray &nameId ) const
{
    for( int i = 0 ; i < audioDeviceList.size() ; ++i)
    {
        if( audioDeviceList[i].nameId == nameId )
            return audioDeviceList[i].id;
    }
    return -1;
}

/**
 * Get a human-readable description from a device id
 */
QByteArray DeviceManager::deviceDescription( int i_id ) const
{
    for( int i = 0 ; i < audioDeviceList.size() ; ++i )
    {
        if( audioDeviceList[i].id == i_id )
            return audioDeviceList[i].description;
    }
    return QByteArray();
}

/**
 * Updates the current list of active devices
 */
void DeviceManager::updateDeviceList()
{
    QList<QByteArray> list, list_hw;
    list.append( DEFAULT_ID );
    list_hw.append( "" );

    // get devices from VLC to list
    bool b_alsa = libvlc_audio_device_available( p_vlc_instance, ALSA );
    if( b_alsa )
    {
        int i_devices = libvlc_audio_alsa_device_count( p_vlc_instance, p_vlc_exception );
        checkException();
        if( i_devices )
        {
            for( int i = 0; i < i_devices; i++ )
            {
                char *psz_name = libvlc_audio_alsa_device_longname( p_vlc_instance, i, p_vlc_exception );
                checkException();
                QString item = QString( ALSA_ID ) + " - " + QString( psz_name );
                free( psz_name );
                list.append( item.toUtf8() );
                char *psz_device_id = libvlc_audio_alsa_device_id( p_vlc_instance, i, p_vlc_exception );
                checkException();
                list_hw.append( psz_device_id );
                free( psz_device_id );
            }
        }
    }

    bool b_oss = libvlc_audio_device_available( p_vlc_instance, OSS );
    if( b_oss )
    {
        list.append( OSS_ID );
        list_hw.append( "" ); // later there can be add path like /dev/dsp
    }

    bool b_sdl = libvlc_audio_device_available( p_vlc_instance, SDL );
    if( b_sdl )
    {
        list.append( SDL_ID );
        list_hw.append( "" );
        
    }

    bool b_directx = libvlc_audio_device_available( p_vlc_instance, DIRECTX );
    if( b_directx )
    {
        list.append( DIRECTX_ID );
    }

    for (int i = 0 ; i < list.size() ; ++i)
    {
        QByteArray nameId = list.at(i);
        QByteArray hwId = list_hw.at(i);
        if( deviceId( nameId ) == -1 )
        {
            // This is a new device, add it
            audioDeviceList.append( AudioDevice( this, nameId, hwId ) );
            emit deviceAdded( deviceId( nameId ) );
        }
    }
    if( list.size() < audioDeviceList.size() )
    {
        //a device was removed
        for( int i = audioDeviceList.size() -1 ; i >= 0 ; --i )
        {
            QByteArray currId = audioDeviceList[i].nameId;
            bool b_found = false;
            for( int k = list.size() -1  ; k >= 0 ; --k )
            {
                if( currId == list[k] )
                {
                    b_found = true;
                    break;
                }
            }
            if( !b_found )
            {
                emit deviceRemoved( deviceId( currId ) );
                audioDeviceList.removeAt( i );
            }
        }
    }
}

/**
  * Returns a list of hardware id
  */
const QList<AudioDevice> DeviceManager::audioOutputDevices() const
{
    return audioDeviceList;
}

}
}

QT_END_NAMESPACE
