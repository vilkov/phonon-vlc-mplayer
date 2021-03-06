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

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <quarkplayer/PluginInterface.h>

#include <phonon/phononnamespace.h>

#include <QtGui/QStatusBar>

class QuarkPlayer;

namespace Phonon {
	class MediaObject;
}

class QLabel;

/**
 * Main window status bar.
 *
 * @author Tanguy Krotoff
 */
class StatusBar : public QStatusBar, public PluginInterface {
	Q_OBJECT
public:

	StatusBar(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~StatusBar();

private slots:

	void tick(qint64 time);

	void changeTimeDisplayMode();

	void stateChanged(Phonon::State newState);

	void bufferStatus(int percentFilled);

	void finished();

	void showTitle();

	void aboutToFinish();

	void prefinishMarkReached(qint32 msecToEnd);

	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

private:

	enum TimeDisplayMode {
		TimeDisplayModeElapsed,
		TimeDisplayModeRemaining
	};

	QLabel * _timeLabel;
};

#include <quarkplayer/PluginFactory.h>

class StatusBarFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("Status bar that gives informations"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;
};

#endif	//STATUSBAR_H
