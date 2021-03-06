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

#include "PlaylistParserThread.h"

#include "IPlaylistParser.h"

#include <mediainfofetcher/MediaInfo.h>

#include <QtCore/QDebug>

PlaylistParserThread::PlaylistParserThread(IPlaylistParser * parser) {
	_parser = parser;
	_save = false;
	_load = false;
}

PlaylistParserThread::~PlaylistParserThread() {
	stop();
}

void PlaylistParserThread::load() {
	_load = true;
	stop();
	start();
}

void PlaylistParserThread::save(const QList<MediaInfo> & files) {
	_save = true;
	_files = files;
	stop();
	start();
}

void PlaylistParserThread::run() {
	if (_parser) {
		if (_load) {
			_load = false;
			_parser->load();
		} else if (_save) {
			_save = false;
			_parser->save(_files);
		}
	}
}

void PlaylistParserThread::stop() {
	if (_parser) {
		_parser->stop();
		wait();
	}
}
