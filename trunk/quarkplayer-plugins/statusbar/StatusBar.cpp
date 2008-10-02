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

#include "StatusBar.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/MainWindow.h>
#include <quarkplayer/config/Config.h>

#include <tkutil/TkTime.h>
#include <tkutil/MouseEventFilter.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

static const char * STATUSBAR_DISPLAY_TIME_MODE_KEY = "statusbar_diplay_time_mode";

Q_EXPORT_PLUGIN2(statusbar, StatusBarFactory);

PluginInterface * StatusBarFactory::create(QuarkPlayer & quarkPlayer) const {
	return new StatusBar(quarkPlayer);
}

StatusBar::StatusBar(QuarkPlayer & quarkPlayer)
	: QStatusBar(NULL),
	PluginInterface(quarkPlayer) {

	_timeLabel = new QLabel(this);
	//Text color is white
	_timeLabel->setStyleSheet("color: rgb(255, 255, 255);");
	_timeLabel->setMargin(2);
	_timeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_timeLabel->installEventFilter(new MousePressEventFilter(this, SLOT(changeDisplayTimeMode())));
	addPermanentWidget(_timeLabel);

	setSizeGripEnabled(false);

	//Background color is black and text color is white
	setStyleSheet("background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);");

	//Add the statusbar to the main window
	quarkPlayer.mainWindow().setStatusBar(this);

	Config::instance().addKey(STATUSBAR_DISPLAY_TIME_MODE_KEY, DisplayTimeModeElapsed);

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));
}

StatusBar::~StatusBar() {
}

void StatusBar::tick(qint64 time) {
	QString timeText;
	if (time != 0) {
		qint64 totalTime = quarkPlayer().currentMediaObject()->totalTime();
		DisplayTimeMode displayTimeMode = static_cast<DisplayTimeMode>(Config::instance().value(STATUSBAR_DISPLAY_TIME_MODE_KEY).toInt());

		switch (displayTimeMode) {
		case DisplayTimeModeElapsed:
			timeText = TkTime::convertMilliseconds(time, totalTime);
			break;
		case DisplayTimeModeRemaining:
			timeText = "- " + TkTime::convertMilliseconds(totalTime - time, totalTime);
			break;
		default:
			qCritical() << __FUNCTION__ << "Error: unknown DisplayTimeMode:" << displayTimeMode;
		}
	}
	_timeLabel->setText(timeText);
}

void StatusBar::changeDisplayTimeMode() {
	qDebug() << __FUNCTION__;

	DisplayTimeMode displayTimeMode = static_cast<DisplayTimeMode>(Config::instance().value(STATUSBAR_DISPLAY_TIME_MODE_KEY).toInt());
	DisplayTimeMode newDisplayTimeMode;
	switch (displayTimeMode) {
	case DisplayTimeModeElapsed:
		newDisplayTimeMode = DisplayTimeModeRemaining;
		break;
	case DisplayTimeModeRemaining:
		newDisplayTimeMode = DisplayTimeModeElapsed;
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unknown DisplayTimeMode:" << displayTimeMode;
	}
	Config::instance().setValue(STATUSBAR_DISPLAY_TIME_MODE_KEY, newDisplayTimeMode);

	tick(quarkPlayer().currentMediaObject()->currentTime());
}

void StatusBar::stateChanged(Phonon::State newState) {
	switch (newState) {
	case Phonon::ErrorState:
		if (quarkPlayer().currentMediaObject()->errorType() == Phonon::FatalError) {
			showMessage("Fatal error: " + quarkPlayer().currentMediaObject()->errorString());
		} else {
			showMessage("Error: " + quarkPlayer().currentMediaObject()->errorString());
		}
		break;

	case Phonon::PlayingState:
		showMessage(tr("Playing"));
		break;

	case Phonon::StoppedState:
		showMessage(tr("Stopped"));
		break;

	case Phonon::PausedState:
		showMessage(tr("Paused"));
		break;

	case Phonon::LoadingState:
		showMessage(tr("Loading..."));
		break;

	case Phonon::BufferingState:
		showMessage(tr("Buffering..."));
		break;

	default:
		qCritical() << "Error: unknown newState:" << newState;
	}
}

void StatusBar::showTitle() {
	QString title = quarkPlayer().currentMediaObjectTitle();
	showMessage(title);
}

void StatusBar::aboutToFinish() {
	qDebug() << __FUNCTION__;
	showMessage(tr("Media finishing..."));
}

void StatusBar::prefinishMarkReached(qint32 msecToEnd) {
	qDebug() << __FUNCTION__ << msecToEnd;
	showMessage(tr("%1 seconds left...").arg(msecToEnd / 1000));
}

void StatusBar::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Update title since we changed from a MediaObject to another
	showTitle();

	//Update current MediaObject state
	stateChanged(mediaObject->state());

	//Resets current and total time display
	tick(mediaObject->currentTime());

	connect(mediaObject, SIGNAL(tick(qint64)), SLOT(tick(qint64)));
	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
		SLOT(stateChanged(Phonon::State)));
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(showTitle()));
	connect(mediaObject, SIGNAL(aboutToFinish()), SLOT(aboutToFinish()));

	//10 seconds before the end
	mediaObject->setPrefinishMark(10000);
	connect(mediaObject, SIGNAL(prefinishMarkReached(qint32)), SLOT(prefinishMarkReached(qint32)));
}