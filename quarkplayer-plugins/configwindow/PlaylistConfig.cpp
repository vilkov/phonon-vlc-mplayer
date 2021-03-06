/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PlaylistConfig.h"

#include <QtCore/QDebug>

PlaylistConfig::PlaylistConfig() {
}

PlaylistConfig::~PlaylistConfig() {
}

void PlaylistConfig::setActivePlaylist(const QUuid & uuid) {
	_uuid = uuid;
	emit activePlaylistChanged(_uuid);
}

QUuid PlaylistConfig::activePlaylist() const {
	return _uuid;
}
