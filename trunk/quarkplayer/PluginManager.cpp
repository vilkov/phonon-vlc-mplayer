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

#include "PluginManager.h"

#include "PluginFactory.h"
#include "PluginInterface.h"
#include "config/Config.h"

#include <tkutil/Random.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

PluginManager * PluginManager::_pluginManager = NULL;

PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
}

PluginManager & PluginManager::instance() {
	if (!_pluginManager) {
		_pluginManager = new PluginManager();
	}
	return *_pluginManager;
}

void PluginManager::loadPlugins(QuarkPlayer & quarkPlayer) {
	QCoreApplication::processEvents();

	QList<int> randomList;

	QDir pluginsDir = QDir(Config::instance().pluginsDir());
	QStringList before = pluginsDir.entryList(QDir::Files);
	qDebug() << __FUNCTION__ << "Before randomize:" << before;
	QStringList pluginList = Random::randomize(before);
	qDebug() << __FUNCTION__ << "After randomize:" << pluginList;

	foreach (QString filename, pluginList) {
		QString filePath(pluginsDir.absoluteFilePath(filename));

		if (!QLibrary::isLibrary(filePath)) {
			//Don't proced no library file
			continue;
		}

		QPluginLoader * loader = new QPluginLoader(filePath);
		_pluginMap[filename] = loader;

		if (Config::instance().pluginsDisabled().contains(filename)) {
			//This means this plugin must not be loaded
			continue;
		}

		QObject * plugin = loader->instance();
		if (plugin) {
			PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
			if (factory) {
				PluginInterface * interface = factory->create(quarkPlayer);
				qDebug() << __FUNCTION__ << "Plugin loaded:" << filename;
			}
		} else {
			loader->unload();
			qCritical() << __FUNCTION__ << "Error: plugin not loaded:" << filename << loader->errorString();
		}

		QCoreApplication::processEvents();
	}

	emit allPluginsLoaded();
}

PluginManager::PluginMap PluginManager::pluginMap() const {
	return _pluginMap;
}