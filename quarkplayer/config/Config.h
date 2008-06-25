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

#ifndef CONFIG_H
#define CONFIG_H

#include <quarkplayer/quarkplayer_export.h>

#include <tkutil/TkConfig.h>

/**
 * Stores QuarkPlayer configuration.
 *
 * Pattern singleton.
 *
 * @see TkConfig
 * @see QSettings
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API Config : public TkConfig {
public:

	/** Singleton. */
	static Config & instance();

	~Config();

	static const char * BACKEND_KEY;
	QStringList backendList() const;
	QString backend() const;

	static const char * LANGUAGE_KEY;
	QString language() const;

	static const char * STYLE_KEY;
	QString style() const;

	static const char * ICON_THEME_KEY;
	QStringList iconThemeList() const;
	QString iconTheme() const;

	static const char * RECENT_FILES_KEY;
	QStringList recentFiles() const;

	static const char * LAST_DIRECTORY_USED_KEY;
	QString lastDirectoryUsed() const;

	static const char * LAST_VOLUME_USED_KEY;
	qreal lastVolumeUsed() const;

	static const char * MUSIC_DIR_KEY;
	QString musicDir() const;



	static const char * TEST_INT_KEY;
	int testInt() const;

	static const char * TEST_BOOL_KEY;
	bool testBool() const;

private:

	/** Singleton. */
	Config();

	/** Singleton. */
	static Config * _instance;
};

#endif	//CONFIG_H