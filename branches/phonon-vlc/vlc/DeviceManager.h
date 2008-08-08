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

#ifndef PHONON_VLC_DEVICEMANAGER_H
#define PHONON_VLC_DEVICEMANAGER_H

#include <phonon/audiooutputinterface.h>

#include <QtCore/QObject>


QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace VLC
{

class Backend;
class DeviceManager;
class AbstractRenderer;
class VideoWidget;

#define DEFAULT_ID  "Default"
#define ALSA_ID     "ALSA"
#define OSS_ID      "OSS"
#define SDL_ID      "Simple DirectMedia layer"
#define DIRECTX_ID  "DirectX"

class AudioDevice
{
public :
    AudioDevice( DeviceManager *p_manager, const QByteArray &name_id, const QByteArray &hw_id = "" );
    int id;
    QByteArray nameId;
    QByteArray hwId;
    QByteArray description;
};

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    DeviceManager( Backend *p_parent );
    virtual ~DeviceManager();
    const QList<AudioDevice> audioOutputDevices() const;
    int deviceId( const QByteArray &nameId ) const;
    QByteArray deviceDescription( int i_id ) const;

signals:
    void deviceAdded( int );
    void deviceRemoved( int );

public slots:
    void updateDeviceList();

private:
    bool canOpenDevice() const;
    Backend *p_backend;
    QList <AudioDevice> audioDeviceList;
};
}
} // namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_DEVICEMANAGER_H
