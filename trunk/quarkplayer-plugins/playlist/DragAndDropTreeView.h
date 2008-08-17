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

#ifndef DRAGANDDROPTREEVIEW_H
#define DRAGANDDROPTREEVIEW_H

#include <QtGui/QTreeView>

class PlaylistModel;

/**
 * Drag and drop capable QTreeView.
 *
 * + other features.
 *
 * @author Tanguy Krotoff
 */
class DragAndDropTreeView : public QTreeView {
	Q_OBJECT
public:

	DragAndDropTreeView(PlaylistModel * playlistModel);

	~DragAndDropTreeView();

private slots:

	void retranslate();

	void playItem();

	void sendTo();

	void deleteItem();

	void rateItem();

	void viewMediaInfo();

private:

	void mousePressEvent(QMouseEvent * event);

	void dragEvent(QDropEvent * event);

	void dropEvent(QDropEvent * event);

	void dragEnterEvent(QDragEnterEvent * event);

	void dragMoveEvent(QDragMoveEvent * event);

	void showMenu(const QPoint & pos);

	void populateActionCollection();

	PlaylistModel * _playlistModel;
};

#endif	//DRAGANDDROPTREEVIEW_H