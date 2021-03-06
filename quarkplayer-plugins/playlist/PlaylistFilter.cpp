/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "PlaylistFilter.h"

#include "PlaylistModel.h"

#include <tkutil/Random.h>

#include <QtCore/QStringList>
#include <QtCore/QDebug>

static const int POSITION_INVALID = -1;

PlaylistFilter::PlaylistFilter(PlaylistModel * playlistModel)
	: QSortFilterProxyModel(playlistModel) {

	_playlistModel = playlistModel;

	_shuffle = false;
	_repeat = false;
	_nextPosition = POSITION_INVALID;

	setSortCaseSensitivity(Qt::CaseInsensitive);

	setSourceModel(_playlistModel);
}

bool PlaylistFilter::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
	QString artist(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_ARTIST, sourceParent)).toString());
	QString album(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_ALBUM, sourceParent)).toString());
	QString title(_playlistModel->data(_playlistModel->index(sourceRow, PlaylistModel::COLUMN_TITLE, sourceParent)).toString());

	bool keep = artist.contains(filterRegExp()) ||
			album.contains(filterRegExp()) ||
			title.contains(filterRegExp());

	return keep;
}

QModelIndex PlaylistFilter::currentIndex() const {
	return _playlistModel->index(_playlistModel->position(), PlaylistModel::COLUMN_FIRST);
}

int PlaylistFilter::convertCurrentModelToFilterPosition() const {
	QModelIndex index = _playlistModel->index(_playlistModel->position(), PlaylistModel::COLUMN_FIRST);
	int position = mapFromSource(index).row();
	return position;
}

int PlaylistFilter::convertToModelPosition(const QModelIndex & index) const {
	int position = POSITION_INVALID;
	if (index.isValid()) {
		const QAbstractItemModel * model = index.model();
		if (model == this) {
			position = mapToSource(index).row();
		} else if (model == _playlistModel) {
			position = index.row();
		}
	}
	return position;
}

void PlaylistFilter::play(const QModelIndex & index) {
	int position = convertToModelPosition(index);
	_playlistModel->play(position);
}

void PlaylistFilter::playNextTrack(bool repeatPlaylist) {
	play(nextTrack(repeatPlaylist));
}

void PlaylistFilter::playPreviousTrack() {
	play(previousTrack());
}

void PlaylistFilter::enqueueNextTrack() {
	_nextPosition = convertToModelPosition(nextTrack(false));
	_playlistModel->enqueue(_nextPosition);
}

QModelIndex PlaylistFilter::nextTrack(bool repeatPlaylist) const {
	int position = convertCurrentModelToFilterPosition();

	if (_shuffle) {
		position = Random::randomInt(0, rowCount() - 1);
	} else {
		position++;
	}

	if (position < 0 || position >= rowCount()) {
		//Limits of the playlist have been reached

		if (repeatPlaylist || (!repeatPlaylist && _repeat)) {
			//Back to the top of the playlist
			position = 0;
		}
	}

	return mapToSource(index(position, PlaylistModel::COLUMN_FIRST));
}

QModelIndex PlaylistFilter::previousTrack() const {
	int position = convertCurrentModelToFilterPosition();

	if (_shuffle) {
		position = Random::randomInt(0, rowCount() - 1);
	} else {
		position--;
	}

	if (position < 0 || position >= rowCount()) {
		//Back to the bottom of the playlist
		position = rowCount() - 1;
	}

	return mapToSource(index(position, PlaylistModel::COLUMN_FIRST));
}

void PlaylistFilter::setPositionAsNextTrack() {
	if (_nextPosition != POSITION_INVALID) {
		_playlistModel->setPosition(_nextPosition);
		_nextPosition = POSITION_INVALID;
	}
}

void PlaylistFilter::setShuffle(bool shuffle) {
	_shuffle = shuffle;
}

void PlaylistFilter::setRepeat(bool repeat) {
	_repeat = repeat;
}
