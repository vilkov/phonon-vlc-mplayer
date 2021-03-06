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

#ifndef CONFIG_H
#define CONFIG_H

#include <quarkplayer/quarkplayer_export.h>

#include <tkutil/TkConfig.h>
#include <tkutil/Singleton.h>

//Yes QUuid is a struct instead of a class inside src/corelib/plugin/quuid.h
//This is strange...
struct QUuid;

/**
 * Stores QuarkPlayer configuration.
 *
 * Pattern singleton.
 * Transform it to CRTP pattern?
 * So one can inherit from this class
 * See http://en.wikipedia.org/wiki/Curiously_Recurring_Template_Pattern
 * See PluginConfig for an example of how to extend Config
 *
 * <pre>
 * //Example for key PLUGINS_DISABLED_KEY
 * QStringList pluginsDisabled = Config::instance().pluginsDisabled();
 * //Not recommended, can be written:
 * //QStringList pluginsDisabled = Config::instance().value(PLUGINS_DISABLED_KEY).toStringList();
 * pluginsDisabled += "pluginFilenameToBlackList";
 * Config::instance().setValue(Config::PLUGINS_DISABLED_KEY, pluginsDisabled);
 * </pre>
 *
 * @see TkConfig
 * @see QSettings
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API Config : public TkConfig, public Singleton<Config> {
	friend class Singleton<Config>;
public:

	/** Deletes all configuration files from the computer. */
	void deleteConfig();

	/** List of available backends + selected backend. */
	static const char * BACKEND_KEY;
	QStringList backendList() const;
	QString backend() const;

	/** Language of the application (i.e en, fr, sp...). */
	static const char * LANGUAGE_KEY;
	QString language() const;

	/** Qt style name to be used. */
	static const char * STYLE_KEY;
	QString style() const;

	/** Icon theme to be used (silk, oxygen, tango...). */
	static const char * ICON_THEME_KEY;
	QStringList iconThemeList() const;
	QString iconTheme() const;

	/** The most recent files played. */
	static const char * RECENT_FILES_KEY;
	QStringList recentFiles() const;

	/** Last directory used to open a media. */
	static const char * LAST_DIR_OPENED_KEY;
	QString lastDirOpened() const;

	/** DVD directory: DVD device. */
	static const char * DVD_DIR_KEY;
	QString dvdDir() const;

	/** CDROM directory: CDROM device. */
	static const char * CDROM_DIR_KEY;
	QString cdromDir() const;

	/** Standard music location (i.e C:/blabla/My Music). */
	static const char * MUSIC_DIR_KEY;
	QString musicDir(const QUuid & uuid) /*const*/;
	void addMusicDir(const QString & musicDir, const QUuid & uuid);

	/** Directories where the plugins *.(dll|so|dylib) are located. */
	static const char * PLUGIN_DIR_KEY;
	QStringList pluginDirList() const;

	/** Directoctory where the resources are located. */
	static const char * RESOURCE_DIR_KEY;
	QString resourceDir() const;

	/** Last media volume used. */
	static const char * LAST_VOLUME_USED_KEY;
	qreal lastVolumeUsed() const;

	/** Volume muted?. */
	static const char * VOLUME_MUTED_KEY;
	bool volumeMuted() const;

	/** @see http://doc.trolltech.com/main-snapshot/qwidget.html#restoreGeometry */
	static const char * MAINWINDOW_GEOMETRY_KEY;
	QByteArray mainWindowGeometry() const;

	/**
	 * Multiple QuarkPlayer instances.
	 *
	 * @see QtSingleApplication
	 */
	static const char * ALLOW_MULTIPLE_INSTANCES_KEY;
	bool allowMultipleInstances() const;

private:

	Config();

	~Config();
};

#endif	//CONFIG_H
