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

#ifndef PHONON_VLC_SEEKSTACK_H
#define PHONON_VLC_SEEKSTACK_H

#include "MediaObject.h"

#include <QtCore/QObject>
#include <QtCore/QStack>

class QTimer;

namespace Phonon
{
namespace VLC
{

/**
 * A queue of seek commands.
 */
class SeekStack : public QObject
{
	Q_OBJECT
public:

	SeekStack( MediaObject *mediaObject );
	~SeekStack();

	void pushSeek( qint64 milliseconds );

signals:

private slots:

	void popSeek();

	void reconnectTickSignal();

private:

	MediaObject *p_media_object;

	QTimer *p_timer;

	QStack<qint64> stack;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_SEEKSTACK_H
