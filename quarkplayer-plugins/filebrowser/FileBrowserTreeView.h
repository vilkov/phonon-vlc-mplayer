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

#ifndef FILEBROWSERTREEVIEW_H
#define FILEBROWSERTREEVIEW_H

#include <QtGui/QTreeView>

class FileBrowserWidget;

class QMouseEvent;
class QFileInfo;

/**
 * File browser QTreeView.
 *
 * @author Tanguy Krotoff
 */
class FileBrowserTreeView : public QTreeView {
	Q_OBJECT
public:

	FileBrowserTreeView(FileBrowserWidget * fileBrowserWidget);

	~FileBrowserTreeView();

private slots:

	void addToPlaylist();

	void play();

	void retranslate();

	void activated(const QModelIndex & index);

	void viewMediaInfo();

private:

	void populateActionCollection();

	QFileInfo fileInfo(const QModelIndex & index) const;

	FileBrowserWidget * _fileBrowserWidget;
};

#endif	//FILEBROWSERTREEVIEW_H
