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

#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <tkutil/tkutil_export.h>

#include <QtCore/QString>
#include <QtCore/QHash>

class QAction;

/**
 * The container for all QAction of QuarkPlayer.
 *
 * This is code factorization, idea is to not duplicate QAction.
 * QAction are defined once (with translation, icon...) and used
 * everywhere via this class.
 *
 * Problem with ActionCollection is that every code checking is done at runtime :/
 *
 * Pattern singleton.
 *
 * @author Tanguy Krotoff
 */
class TKUTIL_API ActionCollection {
public:

	static QAction * action(const QString & name);

	static void addAction(const QString & name, QAction * action);

	static QList<QAction *> actions();

private:

	/** Singleton. */
	ActionCollection();

	~ActionCollection();

	/** Associates a name to a QAction. */
	static QHash<QString, QAction *> _actionHash;
};

#endif	//ACTIONCOLLECTION_H
