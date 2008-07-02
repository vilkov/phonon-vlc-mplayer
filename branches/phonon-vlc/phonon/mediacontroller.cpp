/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2008 Ian Monroe <ian@monroe.nu>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "mediacontroller.h"
#include "mediaobject.h"
#include "addoninterface.h"
#include <QtCore/QList>
#include <QtCore/QVariant>
#include "frontendinterface_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

class MediaControllerPrivate : public FrontendInterfacePrivate
{
    public:
        MediaControllerPrivate(MediaObject *mp) : FrontendInterfacePrivate(mp) {}

        virtual void backendObjectChanged(QObject *);
        MediaController *q;
};

MediaController::MediaController(MediaObject *mp)
    : QObject(mp)
    , d(new MediaControllerPrivate(mp))
{
    d->q = this;
    d->_backendObjectChanged();
}

void MediaControllerPrivate::backendObjectChanged(QObject *m_backendObject)
{
    QObject::connect(m_backendObject, SIGNAL(availableSubtitlesChanged()), q, SIGNAL(availableSubtitlesChanged()));
    QObject::connect(m_backendObject, SIGNAL(availableAudioChannelsChanged()), q, SIGNAL(availableAudioChannelsChanged()));
    QObject::connect(m_backendObject, SIGNAL(availableChaptersChanged()), q, SIGNAL(availableChaptersChanged()));
    QObject::connect(m_backendObject, SIGNAL(availableTitlesChanged()), q, SIGNAL(availableTitlesChanged()));

    //Deprecated
    QObject::connect(m_backendObject, SIGNAL(titleChanged(int)), q, SIGNAL(titleChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(availableTitlesChanged(int)), q, SIGNAL(availableTitlesChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(chapterChanged(int)), q, SIGNAL(chapterChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(availableChaptersChanged(int)), q, SIGNAL(availableChaptersChanged(int)));
    //!Deprecated

    QObject::connect(m_backendObject, SIGNAL(angleChanged(int)), q, SIGNAL(angleChanged(int)));
    QObject::connect(m_backendObject, SIGNAL(availableAnglesChanged(int)), q, SIGNAL(availableAnglesChanged(int)));
}

MediaController::~MediaController()
{
    delete d;
}

#define IFACE \
    AddonInterface *iface = d->iface(); \
    if (!iface) return

MediaController::Features MediaController::supportedFeatures() const
{
    if (!d || !d->media) {
        return false;
    }
    IFACE false;
    Features ret;
    if (iface->hasInterface(AddonInterface::AngleInterface)) {
        ret |= Angles;
    }
    if (iface->hasInterface(AddonInterface::ChapterInterface)) {
        ret |= Chapters;
    }
    if (iface->hasInterface(AddonInterface::TitleInterface)) {
        ret |= Titles;
    }
    return ret;
}

//Deprecated
int MediaController::availableTitles() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::availableTitles).toInt();
}

//Deprecated
int MediaController::currentTitle() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::title).toInt();
}

//Deprecated
void MediaController::setCurrentTitle(int titleNumber)
{
    IFACE;
    iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::setTitle, QList<QVariant>() << QVariant(titleNumber));
}

bool MediaController::autoplayTitles() const
{
    IFACE true;
    return iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::autoplayTitles).toBool();
}

void MediaController::setAutoplayTitles(bool b)
{
    IFACE;
    iface->interfaceCall(AddonInterface::TitleInterface,
            AddonInterface::setAutoplayTitles, QList<QVariant>() << QVariant(b));
}

void MediaController::nextTitle()
{
    //Deprecated
    setCurrentTitle(currentTitle() + 1);

    //TODO
}

void MediaController::previousTitle()
{
    //Deprecated
    setCurrentTitle(currentTitle() - 1);

    //TODO
}

//Deprecated
int MediaController::availableChapters() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::ChapterInterface,
            AddonInterface::availableChapters).toInt();
}

//Deprecated
int MediaController::currentChapter() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::ChapterInterface,
            AddonInterface::chapter).toInt();
}

//Deprecated
void MediaController::setCurrentChapter(int titleNumber)
{
    IFACE;
    iface->interfaceCall(AddonInterface::ChapterInterface,
            AddonInterface::setChapter, QList<QVariant>() << QVariant(titleNumber));
}

int MediaController::availableAngles() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::AngleInterface,
            AddonInterface::availableAngles).toInt();
}

int MediaController::currentAngle() const
{
    IFACE 0;
    return iface->interfaceCall(AddonInterface::AngleInterface,
            AddonInterface::angle).toInt();
}

void MediaController::setCurrentAngle(int titleNumber)
{
    IFACE;
    iface->interfaceCall(AddonInterface::AngleInterface,
            AddonInterface::setAngle, QList<QVariant>() << QVariant(titleNumber));
}

AudioChannelDescription MediaController::currentAudioChannel() const
{
    IFACE AudioChannelDescription();
    return iface->interfaceCall(AddonInterface::AudioChannelInterface,
        AddonInterface::currentAudioChannel).value<AudioChannelDescription>();
}

SubtitleDescription MediaController::currentSubtitle() const
{
    IFACE SubtitleDescription();
    return iface->interfaceCall(AddonInterface::SubtitleInterface,
        AddonInterface::currentSubtitle).value<SubtitleDescription>();
}

ChapterDescription MediaController::currentChapter2() const
{
    IFACE ChapterDescription();
    return iface->interfaceCall(AddonInterface::ChapterInterface,
        AddonInterface::currentChapter).value<ChapterDescription>();
}

TitleDescription MediaController::currentTitle2() const
{
    IFACE TitleDescription();
    return iface->interfaceCall(AddonInterface::TitleInterface,
        AddonInterface::currentTitle).value<TitleDescription>();
}

QList<AudioChannelDescription> MediaController::availableAudioChannels() const
{
    QList<AudioChannelDescription> retList;
    IFACE retList;
    retList = iface->interfaceCall(AddonInterface::AudioChannelInterface,
        AddonInterface::availableAudioChannels).value< QList<AudioChannelDescription> >();
    return retList;
}

QList<SubtitleDescription> MediaController::availableSubtitles() const
{
    QList<SubtitleDescription> retList;
    IFACE retList;
    retList = iface->interfaceCall(AddonInterface::SubtitleInterface,
        AddonInterface::availableSubtitles)
        .value< QList<SubtitleDescription> >();
    return retList;
}

QList<ChapterDescription> MediaController::availableChapters2() const
{
    QList<ChapterDescription> retList;
    IFACE retList;
    retList = iface->interfaceCall(AddonInterface::ChapterInterface,
        AddonInterface::availableChapters)
        .value< QList<ChapterDescription> >();
    return retList;
}

QList<TitleDescription> MediaController::availableTitles2() const
{
    QList<TitleDescription> retList;
    IFACE retList;
    retList = iface->interfaceCall(AddonInterface::TitleInterface,
        AddonInterface::availableTitles)
        .value< QList<TitleDescription> >();
    return retList;
}

void MediaController::setCurrentAudioChannel(const Phonon::AudioChannelDescription &stream)
{
    IFACE;
    iface->interfaceCall(AddonInterface::AudioChannelInterface,
        AddonInterface::setCurrentAudioChannel, QList<QVariant>() << qVariantFromValue(stream));
}

void MediaController::setCurrentSubtitle(const Phonon::SubtitleDescription &stream)
{
    IFACE;
    iface->interfaceCall(AddonInterface::SubtitleInterface,
        AddonInterface::setCurrentSubtitle, QList<QVariant>() << qVariantFromValue(stream));
}

void MediaController::setCurrentChapter(const Phonon::ChapterDescription &chapter)
{
    IFACE;
    iface->interfaceCall(AddonInterface::ChapterInterface,
        AddonInterface::setCurrentChapter, QList<QVariant>() << qVariantFromValue(chapter));
}

void MediaController::setCurrentTitle(const Phonon::TitleDescription &title)
{
    IFACE;
    iface->interfaceCall(AddonInterface::TitleInterface,
        AddonInterface::setCurrentTitle, QList<QVariant>() << qVariantFromValue(title));
}

#undef IFACE

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_mediacontroller.cpp"

// vim: sw=4 sts=4 et tw=100
