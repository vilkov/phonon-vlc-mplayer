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

#include "ActionCollection.h"

#include <QtGui/QtGui>

QHash<QString, QAction *> ActionCollection::_actionHash;

ActionCollection::ActionCollection() {
}

ActionCollection::~ActionCollection() {
	_actionHash.clear();
}

void ActionCollection::addAction(const QString & name, QAction * action) {
	if (!action) {
		qCritical() << __FUNCTION__ << "Error: QAction is null";
	}

	QString indexName = name;
	if (indexName.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: QAction index name is empty";
		indexName = action->objectName();
	} else {
		action->setObjectName(indexName);
	}

	if (indexName.isEmpty()) {
		indexName = indexName.sprintf("unnamed-%p", (void *) action);
	}

	if (_actionHash.contains(indexName)) {
		qCritical() << __FUNCTION__ << "Error: QAction index name:" << indexName << "already exist";
	}

	_actionHash[indexName] = action;
}

QAction * ActionCollection::action(const QString & name) {
	if (_actionHash.count(name) != 1) {
		qCritical() << __FUNCTION__ << "Error: invalid QAction name:" << name;
	}

	QAction * action = _actionHash.value(name);
	if (!action) {
		qCritical() << __FUNCTION__ << "Error: QAction is null";
	}

	return action;
}

QList<QAction *> ActionCollection::actions() {
	QList<QAction *> actionList;

	QHashIterator<QString, QAction *> it(_actionHash);
	while (it.hasNext()) {
		it.next();

		QAction * action = it.value();
		actionList += action;
	}

	return actionList;
}
