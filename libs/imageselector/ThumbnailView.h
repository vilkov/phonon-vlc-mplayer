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

#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QtGui/QListView>

/**
 * An overloaded QListView, with a custom delegate to draw thumbnails.
 *
 * @author Aur�lien G�teau
 * @author Tanguy Krotoff
 */
class ThumbnailView : public QListView {
public:

	ThumbnailView(QWidget * parent);

	/**
	 * Sets the thumbnail size in pixels.
	 */
	void setThumbnailSize(int size);

	/**
	 * Returns the thumbnail size.
	 */
	int thumbnailSize() const;

	/**
	 * Returns the width of an item.
	 *
	 * This width is proportional to the thumbnail size.
	 */
	int itemWidth() const;

	/**
	 * Returns the height of an item.
	 *
	 * This width is proportional to the thumbnail size.
	 */
	int itemHeight() const;

	/**
	 * Overloaded so that it shows a reasonable amount of thumbnails.
	 */
	QSize sizeHint() const;

private:

	int _thumbnailSize;
};

#endif	//THUMBNAILVIEW_H
