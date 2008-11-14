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

#ifndef PHONON_MPLAYER_SINKNODE_H
#define PHONON_MPLAYER_SINKNODE_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Phonon
{
namespace MPlayer
{

//#include "MPlayerMediaObject.h"
class MPlayerMediaObject;
typedef MPlayerMediaObject PrivateMediaObject;

/**
 *
 *
 * @author Tanguy Krotoff
 */
class SinkNode : public QObject {
	Q_OBJECT
public:

	SinkNode(QObject * parent);
	virtual ~SinkNode();

	virtual void connectToMediaObject(PrivateMediaObject * mediaObject);

	virtual void disconnectFromMediaObject(PrivateMediaObject * mediaObject);

protected:

	void sendMPlayerCommand(const QString & command) const;

	PrivateMediaObject * _mediaObject;

private:

};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_MPLAYER_SINKNODE_H
