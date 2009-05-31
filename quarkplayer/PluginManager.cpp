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

#include "PluginManager.h"

#include "PluginManager_win32.h"

#include "PluginFactory.h"
#include "PluginInterface.h"
#include "PluginConfig.h"
#include "config/Config.h"

#include <tkutil/Random.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

PluginManager::PluginManager() {
	_quarkPlayer = NULL;
	_allPluginsLoaded = false;
}

PluginManager::~PluginManager() {
	//Saves the plugin list
	PluginConfig::instance().setPlugins(availablePlugins());

	deleteAllPlugins();
}

QString PluginManager::findPluginDir() const {
	QStringList pluginDirList(Config::instance().pluginDirList());
	QString appDir(QCoreApplication::applicationDirPath());

	QString tmp;
	foreach (QString pluginDir, pluginDirList) {
		QDir dir(pluginDir);

		//Order matters!!! See Config.cpp
		//Check if we have:
		//applicationDirPath/quarkplayer.exe
		//applicationDirPath/plugins/*.pluginspec
		//Then applicationDirPath/plugins/ is the plugin directory
		//
		//Check if we have:
		//applicationDirPath/quarkplayer.exe
		//usr/lib/quarkplayer/plugins/*.pluginspec

		qDebug() << "Checking for plugins:" << pluginDir;

		if (pluginDir.contains(appDir)) {
			if (dir.exists() && !dir.entryList(QDir::Files).isEmpty()) {
				//We have found the plugin directory
				tmp = pluginDir;
				break;
			}
		} else {
			if (dir.exists() && !dir.entryList(QDir::Files).isEmpty()) {
				//We have found the plugin directory
				tmp = pluginDir;
				break;
			}
		}
	}

	if (tmp.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: couldn't find the plugin directory";
	} else {
		qDebug() << __FUNCTION__ << "Plugin directory:" << tmp;
	}

	return tmp;
}

void PluginManager::loadAllPlugins(QuarkPlayer & quarkPlayer) {
	Q_ASSERT(!_allPluginsLoaded);

	//Windows specific code
	PluginManagerWin32::setErrorMode();
	PluginManagerWin32::setDllDirectory();
	///

	//Stupid hack, see loadPlugin()
	_quarkPlayer = &quarkPlayer;
	///

	//Creates the list of static plugins
	QStringList staticPlugins;
	foreach (QObject * plugin, QPluginLoader::staticInstances()) {
		if (plugin) {
			PluginFactory * factory = qobject_cast<PluginFactory *>(plugin);
			if (factory) {
				staticPlugins += factory->name();
			}
		}
	}
	///

	_pluginDir = findPluginDir();

	//List of all the available plugins
	//Dynamic plugins
	QFileInfoList fileInfoList(QDir(_pluginDir).entryInfoList(QDir::Files));
	foreach (QFileInfo fileInfo, fileInfoList) {
		//Take only the base name, i.e without the extension since this part is platform dependent
		_availablePlugins += fileInfo.baseName();
	}
	//_availablePlugins += QDir(_pluginDir).entryList(QDir::Files);
	//Static plugins
	_availablePlugins += staticPlugins;

	//Randomizes the list of available plugins, this allows
	//to easily detect crashes/bugs due to order processing
	QStringList filenames(Random::randomize(_availablePlugins));

	foreach (QString filename, filenames) {

		qDebug() << __FUNCTION__ << "filename:" << filename;

		//Check if the plugin has been already loaded
		if (!(_loadedPlugins.values(filename).isEmpty())) {
			continue;
		}

		//Compare to the configuration:
		//this will tell us if we can load the plugin or not
		PluginDataList list(PluginConfig::instance().plugins().values(filename));
		foreach (PluginData pluginData, list) {
			if (pluginData.isEnabled()) {
				//This plugin is enabled => let's load it
				loadPlugin(pluginData);
			} else {
				//This plugin is disabled => don't load it
				_disabledPlugins += pluginData;
			}
		}
		///

		if (list.isEmpty()) {
			//No plugin matching the given filename inside the configuration
			//Let's create it for the first time
			PluginData pluginData(filename, QUuid::createUuid(), true);
			loadPlugin(pluginData);
		}
	}

	_allPluginsLoaded = true;
	emit allPluginsLoaded();
}

bool PluginManager::loadDisabledPlugin(const PluginData & pluginData) {
	bool loaded = false;

	QString filename(pluginData.fileName());

	PluginDataList list(_disabledPlugins.values(filename));
	foreach (PluginData data, list) {
		//Loads a plugin that has already existed in the past
		//instead of creating a new one
		loaded = loadPlugin(data);
		break;
	}

	if (!loaded) {
		//Plugin does not already exist in the configuration, nor is a static plugin
		//Let's create it for the first time
		PluginData newPluginData(filename, QUuid::createUuid(), true);
		loaded = loadPlugin(newPluginData);
	}

	return loaded;
}

//QPluginLoader needs the file extension unlike QLibrary who does not need it
QString getPluginFileNameWithExtension(const QString & filename) {
	QString tmp(filename);
	Q_ASSERT(!tmp.isEmpty());

	if (!QLibrary::isLibrary(tmp)) {
#ifdef Q_WS_WIN
		tmp += ".dll";
#elif defined(Q_WS_X11)
		tmp += ".so";
#else
		qCritical() << __FUNCTION__ << "Platform not supported";
#endif
	}
	return tmp;
}

bool PluginManager::loadPlugin(PluginData & pluginData) {
	bool loaded = false;

	pluginData.setEnabled(true);

	QString filename(pluginData.fileName());

	qDebug() << __FUNCTION__ << "Load plugin:" << filename << "...";

	//Creates the factory
	//2 cases: dynamic plugin and static plugin
	PluginFactory * factory = NULL;
	bool pluginFound = false;
	QPluginLoader loader(getPluginFileNameWithExtension(_pluginDir + QDir::separator() + filename));
	QObject * plugin = loader.instance();
	if (plugin) {
		//Ok, this is a dynamic plugin
		factory = qobject_cast<PluginFactory *>(plugin);
		if (factory) {
			pluginData.setFactory(factory);
			pluginFound = true;
		} else {
			qCritical() << __FUNCTION__ << "Error: this is not a QuarkPlayer plugin:" << filename;
		}
	} else {
		//QPluginLoader failed, let's check if it is a static plugin
		foreach (QObject * plugin, QPluginLoader::staticInstances()) {
			factory = qobject_cast<PluginFactory *>(plugin);
			if (factory) {
				if (filename == factory->name()) {
					//Ok, this is a static plugin
					pluginData.setFactory(factory);
 					pluginFound = true;
					break;
				}
			}
		}
	}
	if (!pluginFound) {
		qCritical() << __FUNCTION__ << "Error: plugin couldn't be loaded:" << filename << loader.errorString();
	}
	///


	//Check plugin dependencies and load them
	//before to load the actual plugin
	if (factory) {
		QStringList dependsOn(factory->dependencies());

		foreach (QString filenameDepends, dependsOn) {

			bool dependencySolved = false;

			//Check in the list of already loaded plugins
			PluginDataList loadedPlugins = _loadedPlugins.values(filenameDepends);
			foreach (PluginData data, loadedPlugins) {
				//If we are here, it means the dependency plugin is already loaded
				//so no problem
				dependencySolved = true;
				break;
			}

			if (!dependencySolved) {
				//Check in the list of disabled plugins
				PluginDataList disabledPlugins = _disabledPlugins.values(filenameDepends);
				foreach (PluginData data, disabledPlugins) {
					//If we are here, it means the dependency plugin is a disabled plugin
					//FIXME load it?
					loadDisabledPlugin(data);
					dependencySolved = true;
					break;
				}
			}

			if (!dependencySolved) {
				//Still not found the dependency plugin?
				//Check in the list of available plugins
				//these plugins are neither part of the two categories at the moment:
				//loaded or disabled
				foreach (QString availableFilename, _availablePlugins) {
					if (filenameDepends == availableFilename) {
						//Ok, the dependency plugin has been found
						//and is not loaded nor disabled
						PluginData dependencyPluginData(filenameDepends, QUuid::createUuid(), true);
						loadPlugin(dependencyPluginData);
						dependencySolved = true;
						break;
					}
				}
			}

			if (!dependencySolved) {
				//What to do?
				qCritical() << __FUNCTION__ << "Missing dependency:" << filenameDepends;
			}
		}
	}
	///


	//Use the PluginFactory and create the PluginInterface
	if (pluginData.interface()) {
		//Plugin already loaded
		loaded = true;
	} else {
		factory = pluginData.factory();
		if (factory) {
			PluginInterface * interface = factory->create(*_quarkPlayer, pluginData.uuid());
			pluginData.setInterface(interface);

			//Add the loaded plugin to the list of loaded plugins
			//"So the last shall be first, and the first last" Matthew 20:16 ;-)
			//We need to delete the plugins backward: delete first the last loaded plugin
			//This avoid to delete the MainWindow first and then the plugins that depend on it => crash
			//This is why we prepend the loaded plugins to the list of loaded plugins
			_loadedPlugins.prepend(pluginData);

			qDebug() << __FUNCTION__ << "Plugin loaded:" << filename;
			loaded = true;
		} else {
			loaded = false;
		}
	}
	///

	QCoreApplication::processEvents();

	return loaded;
}

void PluginManager::deleteAllPlugins() {
	foreach (PluginData pluginData, _loadedPlugins) {
		qDebug() << __FUNCTION__ << "Delete plugin:" << pluginData.fileName();
		deletePluginWithoutSavingConfig(pluginData);
	}
}

bool PluginManager::allPluginsAlreadyLoaded() const {
	return _allPluginsLoaded;
}

bool PluginManager::deletePlugin(PluginData & pluginData) {
	bool ret = deletePluginWithoutSavingConfig(pluginData);
	if (ret) {
		//Update and saves the plugins configuration
		//Use pluginData.uuid()
		_loadedPlugins.removeAll(pluginData);
		_disabledPlugins.append(pluginData);
		PluginConfig::instance().setPlugins(availablePlugins());
		///
	}
	return ret;
}

bool PluginManager::deletePluginWithoutSavingConfig(PluginData & pluginData) {
	bool ret = true;

	if (!pluginData.interface()) {
		//qCritical() << __FUNCTION__ << "Error: couldn't delete the plugin:" << filename;
		ret = false;
	} else {
		//Unloads the plugin
		pluginData.deleteInterface();
	}

	//FIXME Sometimes QuarkPlayer does not quit
	//some threads are still running
	//I guess this line should do it...
	QCoreApplication::processEvents();

	return ret;
}

PluginData PluginManager::pluginData(const QUuid & uuid) const {
	Q_ASSERT(!uuid.isNull());

	PluginData data;

	foreach (PluginData pluginData, _loadedPlugins) {
		if (pluginData.uuid() == uuid) {
			data = pluginData;
			break;
		}
	}

	//FIXME test if several identical uuid exist.
	//In this case make an assert since
	//there can't be several identical uuid
	//inside the list of loaded plugins

	return data;
}

PluginInterface * PluginManager::pluginInterface(const QString & filename) const {
	PluginInterface * interface = NULL;
	foreach (PluginData pluginData, _loadedPlugins) {
		if (pluginData.fileName() == filename) {
			interface = pluginData.interface();
			break;
		}
	}
	return interface;
}

PluginDataList PluginManager::availablePlugins() const {
	PluginDataList availablePlugins;
	availablePlugins += _loadedPlugins;
	availablePlugins += _disabledPlugins;
	return availablePlugins;
}
