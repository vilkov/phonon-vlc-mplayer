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

#include "Backend.h"

#include "MediaObject.h"
#include "VideoWidget.h"
#include "AudioOutput.h"
#include "EffectManager.h"
#include "Effect.h"
#include "SinkNode.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

#include "VLCMediaObject.h"

#include <QtCore/QByteArray>
#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QFutureWatcher>


Q_EXPORT_PLUGIN2( phonon_vlc, Phonon::VLC::Backend );


namespace Phonon
{
namespace VLC
{

Backend::Backend( QObject * p_parent, const QVariantList & args )
	: QObject( p_parent ),
	  p_effectManager( NULL ),
	  p_deviceManager( NULL )
{
	qDebug() << "Phonon version:" << Phonon::phononVersion();

	setProperty( "identifier", QLatin1String( "phonon_vlc" ) );
	setProperty( "backendName", QLatin1String( "VLC" ) );
	setProperty( "backendComment", QLatin1String( "VLC plugin for Phonon" ) );
	setProperty( "backendVersion", QLatin1String( "0.1") );
	setProperty( "backendWebsite", QLatin1String( "http://multimedia.kde.org/" ) );

	qDebug() << "Loading VLC...";

	//this can take some time
	initLibVLC();

	p_effectManager = new EffectManager( this );
	p_deviceManager = new DeviceManager( this );
}

Backend::~Backend() {
	//releaseLibVLC();
	delete p_effectManager;
}

QObject * Backend::createObject( BackendInterface::Class c, QObject * p_parent, const QList<QVariant> & args )
{
	switch( c )
	{
	    case MediaObjectClass:
		    return new VLCMediaObject( p_parent );
        /*case VolumeFaderEffectClass:
            return new VolumeFaderEffect(parent);
        */
        case AudioOutputClass:
        {
            AudioOutput *p_ao = new AudioOutput( this, p_parent );
            audioOutputs.append( p_ao );
            return p_ao;
        }
        /*case AudioDataOutputClass:
            return new AudioDataOutput(parent);
        case VisualizationClass:
            return new Visualization(parent);
        case VideoDataOutputClass:
            return new VideoDataOutput(parent);*/
        case EffectClass: 
            return new Effect( p_effectManager, args[0].toInt(), p_parent );
        case VideoWidgetClass:
            return new VideoWidget( qobject_cast<QWidget *>( p_parent ) );
	}

	return NULL;
}

bool Backend::supportsVideo() const
{
	return true;
}

bool Backend::supportsOSD() const
{
	return true;
}

bool Backend::supportsFourcc( quint32 fourcc ) const
{
	return true;
}

bool Backend::supportsSubtitles() const
{
	return true;
}

QStringList Backend::availableMimeTypes() const
{
	if( supportedMimeTypes.isEmpty() )
	{
		//Audio mime types
		supportedMimeTypes
			<< "audio/x-m4a"
			<< "audio/x-aiff"
			<< "audio/aiff"
			<< "audio/x-pn-aiff"
			<< "audio/x-realaudio"
			<< "audio/basic"
			<< "audio/x-basic"
			<< "audio/x-pn-au"
			<< "audio/x-8svx"
			<< "audio/8svx"
			<< "audio/x-16sv"
			<< "audio/168sv"
			<< "audio/x-ogg"
			<< "audio/x-speex+ogg"
			<< "audio/vnd.rn-realaudio"
			<< "audio/x-pn-realaudio-plugin"
			<< "audio/x-real-audio"
			<< "audio/x-wav"
			<< "audio/wav"
			<< "audio/x-pn-wav"
			<< "audio/x-pn-windows-acm"
			<< "audio/mpeg2"
			<< "audio/x-mpeg2"
			<< "audio/mpeg3"
			<< "audio/x-mpeg3"
			<< "audio/mpeg"
			<< "audio/x-mpeg"
			<< "audio/x-mpegurl"
			<< "audio/x-mp3"
			<< "audio/mp3"
			<< "audio/mpeg"
			<< "audio/x-ms-wma";

		//Video mime types
		supportedMimeTypes
			<< "video/quicktime"
			<< "video/x-quicktime"
			<< "video/mkv"
			<< "video/msvideo"
			<< "video/x-msvideo"
			<< "video/x-flic"
			<< "video/x-anim"
			<< "video/anim"
			<< "video/mng"
			<< "video/x-mng"
			<< "video/mpeg"
			<< "video/x-mpeg"
			<< "video/x-ms-asf"
			<< "video/x-ms-wmv"
			<< "video/mp4"
			<< "video/mpg"
			<< "video/avi";

		//Application mime types
		supportedMimeTypes
			<< "application/x-annodex"
			<< "application/x-quicktimeplayer"
			<< "application/ogg"
			<< "application/ogg"
			<< "application/vnd.rn-realmedia"
			<< "application/x-flash-video";

		//Image mime types
		supportedMimeTypes
			<< "image/x-ilbm"
			<< "image/ilbm"
			<< "image/png"
			<< "image/x-png";
	}

	return supportedMimeTypes;
}

QList<int> Backend::objectDescriptionIndexes( ObjectDescriptionType type ) const
{
	QList<int> list;

	switch( type )
	{
        case Phonon::AudioOutputDeviceType:
        {
            QList<AudioDevice> deviceList = p_deviceManager->audioOutputDevices();
            for( int dev = 0 ; dev < deviceList.size() ; ++dev )
                list.append(deviceList[dev].id);
            break;
        }
        /*case Phonon::AudioCaptureDeviceType:
            break;
        case Phonon::VideoOutputDeviceType:
            break;
        case Phonon::VideoCaptureDeviceType:
            break;
        case Phonon::VisualizationType:
            break;
        case Phonon::AudioCodecType:
            break;
        case Phonon::VideoCodecType:
            break;
        case Phonon::ContainerFormatType:
            break;
        case Phonon::AudioChannelType:
            break;
        case Phonon::SubtitleType:
            break;
        case Phonon::ChapterType:
            break;
        case Phonon::TitleType:
            break;
        */

        case Phonon::EffectType:
        {
            QList<EffectInfo *> effectList = p_effectManager->getEffectList();
            for ( int i_effect = 0; i_effect < effectList.size(); ++i_effect )
                list.append( i_effect );
            break;
        }
	}

	return list;
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties( ObjectDescriptionType type, int i_index) const
{
	QHash<QByteArray, QVariant> ret;

	switch ( type )
	{
        case Phonon::AudioOutputDeviceType:
        {
            QList<AudioDevice> audioDevices = p_deviceManager->audioOutputDevices();
            if( i_index >= 0 && i_index < audioDevices.size() )
            {
                ret.insert( "name", audioDevices[i_index].nameId );
                ret.insert( "description", audioDevices[i_index].description );
                ret.insert( "icon", QLatin1String( "audio-card" ) );
            }
            break;
        }
        /*case Phonon::AudioCaptureDeviceType:
            break;
        case Phonon::VideoOutputDeviceType:
            break;
        case Phonon::VideoCaptureDeviceType:
            break;
        case Phonon::VisualizationType:
            break;
        case Phonon::AudioCodecType:
            break;
        case Phonon::VideoCodecType:
            break;
        case Phonon::ContainerFormatType:
            break;
        */
        case Phonon::EffectType: 
        {
            QList<EffectInfo *> effectList = p_effectManager->getEffectList();

            if( i_index >= 0 && i_index <= effectList.size() )
            {
                const EffectInfo *effect = effectList[ i_index ];
                ret.insert( "name", effect->getName() );
                ret.insert( "description", effect->getDescription() );
                ret.insert( "author", effect->getAuthor() );
            } 
            else
                Q_ASSERT(1); // Since we use list position as ID, this should not happen

            break;
	    }

	default:
		qCritical() << __FUNCTION__ << "Unknow ObjectDescriptionType:" << type;
	}

	return ret;
}

bool Backend::startConnectionChange( QSet<QObject *> nodes )
{
	qDebug() << __FUNCTION__ << "";
	foreach (QObject * node, nodes)
	{
		qDebug() << "node:" << node->metaObject()->className();
	}

	Q_UNUSED(nodes);
	//There's nothing we can do but hope the connection changes won't take too long so that buffers
	//would underrun. But we should be pretty safe the way xine works by not doing anything here.
	return true;
}

bool Backend::connectNodes( QObject * p_source, QObject * p_sink )
{
	qDebug() << __FUNCTION__ << p_source->metaObject()->className() << p_sink->metaObject()->className();

	//Example:
	//source = Phonon::VLC_MPlayer::MediaObject
	//sink = Phonon::VLC_MPlayer::VideoWidget

	//Example:
	//source = Phonon::VLC_MPlayer::MediaObject
	//sink = Phonon::VLC_MPlayer::AudioOutput

	//Example:
	//source = Phonon::VLC_MPlayer::MediaObject
	//sink = Phonon::VLC_MPlayer::Effect

	//Example:
	//source = Phonon::VLC_MPlayer::Effect
	//sink = Phonon::VLC_MPlayer::AudioOutput

	SinkNode *p_sinkNode = qobject_cast<SinkNode *>( p_sink );
	if( p_sinkNode )
	{
		PrivateMediaObject * p_media_object = qobject_cast<PrivateMediaObject *>( p_source );
		if( p_media_object )
		{
			//Connects the SinkNode to a MediaObject
			p_sinkNode->connectToMediaObject( p_media_object );
			return true;
		}
		else
		{
			//FIXME try to find a better way...
			//Effect * p_effect = qobject_cast<Effect * >( p_source );
			return true;
		}
	}

	qWarning() << __FUNCTION__ << "Connection not supported";
	return false;
}

bool Backend::disconnectNodes( QObject * p_source, QObject * p_sink )
{
	qDebug() << __FUNCTION__ << p_source->metaObject()->className() << p_sink->metaObject()->className();

	SinkNode * p_sinkNode = qobject_cast<SinkNode *>( p_sink );
	if( p_sinkNode )
	{
		PrivateMediaObject * p_media_object = qobject_cast<PrivateMediaObject *>( p_source );
		if( p_media_object )
		{
			//Disconnects the SinkNode from a MediaObject
			p_sinkNode->disconnectFromMediaObject( p_media_object );
			return true;
		}
		else
		{
			//FIXME try to find a better way...
			//Effect * p_effect = qobject_cast<Effect * >( p_source );
			return true;
		}

	}

	qWarning() << __FUNCTION__ << "Disconnection not supported";
	return false;
}

bool Backend::endConnectionChange( QSet<QObject *> nodes )
{
	qDebug() << __FUNCTION__;
	foreach ( QObject * p_node, nodes )
	{
		qDebug() << "node:" << p_node->metaObject()->className();
	}

	return true;
}

void Backend::freeSoundcardDevices()
{
}

QString Backend::toString() const
{
	return "VLC Phonon Backend";
		/*"libvlc version=" + QString(p_libvlc_get_version()) + "\n"
		"libvlc changeset=" + QString(p_libvlc_get_changeset()) + "\n"
		"libvlc compiler=" + QString(p_libvlc_get_compiler());*/

}

}}	//Namespace Phonon::VLC
