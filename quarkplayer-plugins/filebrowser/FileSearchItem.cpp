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

#include "FileSearchItem.h"

#include <tkutil/TkFile.h>

#include <QtCore/QStringList>
#include <QtCore/QDebug>

FileSearchItem::FileSearchItem(const QString & filename, FileSearchItem * parent) {
	_mediaInfo = MediaInfo(filename);
	_parentItem = parent;
	_populatedChildren = false;
}

FileSearchItem::~FileSearchItem() {
	qDeleteAll(_childItems);
}

void FileSearchItem::setPopulatedChildren(bool populatedChildren) {
	_populatedChildren = populatedChildren;
}

bool FileSearchItem::populatedChildren() const {
	return _populatedChildren;
}

void FileSearchItem::appendChild(FileSearchItem * newItem) {
	static int firstFileItemAdded = -1;

	if (_childItems.isEmpty()) {
		_childItems.append(newItem);
	} else {
		const FileSearchItem * lastItem = _childItems.last();

		if (!lastItem->isDir() && newItem->isDir()) {
			//Insert before the first item of type "file"
			_childItems.insert(firstFileItemAdded, newItem);
		} else {
			//Don't change the order
			_childItems.append(newItem);
		}

		if (!newItem->isDir() && firstFileItemAdded != -1) {
			//Saves the position of the first item of type "file"
			firstFileItemAdded = _childItems.count() - 1;
		}
	}
}

FileSearchItem * FileSearchItem::child(int row) {
	return _childItems.value(row);
}

int FileSearchItem::childCount() const {
	return _childItems.count();
}

FileSearchItem * FileSearchItem::parent() {
	return _parentItem;
}

int FileSearchItem::row() const {
	if (_parentItem) {
		return _parentItem->_childItems.indexOf(const_cast<FileSearchItem *>(this));
	}

	return 0;
}

QString FileSearchItem::fileName() const {
	return _mediaInfo.fileName();
}

const MediaInfo & FileSearchItem::mediaInfo() const {
	return _mediaInfo;
}

void FileSearchItem::setMediaInfo(const MediaInfo & mediaInfo) {
	_mediaInfo = mediaInfo;
}

FileSearchItem::Type FileSearchItem::type() const {
	bool isDir = TkFile::isDir(_mediaInfo.fileName());
	if (isDir) {
		return Dir;
	} else {
		return File;
	}
}

bool FileSearchItem::isDir() const {
	return type() == Dir;
}
