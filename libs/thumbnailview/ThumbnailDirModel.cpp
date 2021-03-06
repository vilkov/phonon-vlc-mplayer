/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2004-2007  Wengo
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

#include "ThumbnailDirModel.h"

#include "ThumbnailManager.h"

#include <QtGui/QImageReader>
#include <QtGui/QPixmap>

ThumbnailDirModel::ThumbnailDirModel(QObject * parent)
	: QAbstractListModel(parent) {

	_dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	_dir.setSorting(QDir::Name | QDir::LocaleAware | QDir::IgnoreCase);

	_thumbnailManager = new ThumbnailManager(this);

	foreach (QByteArray format, QImageReader::supportedImageFormats()) {
		QString suffix(format);
		suffix = suffix.toLower();
		_imageSuffixList << suffix;
	}

	connect(_thumbnailManager, SIGNAL(thumbnailUpdated(const QFileInfo &)),
		SLOT(pixmapUpdated(const QFileInfo &)));
}

void ThumbnailDirModel::setThumbnailSize(int size) {
	_thumbnailManager->setThumbnailSize(size);
}

void ThumbnailDirModel::setDir(const QString & dir) {
	_thumbnailManager->clear();
	_dir.setPath(dir);
	refresh();
}

int ThumbnailDirModel::rowCount(const QModelIndex & /*parent*/) const {
	return _infoList.size();
}

QVariant ThumbnailDirModel::data(const QModelIndex & index, int role) const {
	int row = index.row();
	int column = index.column();
	if (column > 0) {
		return QVariant();
	}

	if (row >= _infoList.size()) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		QString path(_infoList[row].absoluteFilePath());
		return path;
	} else if (role == Qt::DecorationRole) {
		return _thumbnailManager->thumbnail(_infoList[row]);
	}

	return QVariant();
}

void ThumbnailDirModel::refresh() {
	QFileInfoList infoList = _dir.entryInfoList();
	_infoList.clear();

	foreach (QFileInfo fileInfo, infoList) {
		//Only add images
		QString suffix = fileInfo.suffix().toLower();
		if (_imageSuffixList.contains(suffix)) {
			_infoList << fileInfo;
		}
	}
	reset();
}

void ThumbnailDirModel::pixmapUpdated(const QFileInfo & updatedFileInfo) {
	int row = 0;
	foreach (QFileInfo fileInfo, _infoList) {
		if (fileInfo == updatedFileInfo) {
			QModelIndex itemIndex = index(row);
			dataChanged(itemIndex, itemIndex);
			break;
		}
		++row;
	}
}
