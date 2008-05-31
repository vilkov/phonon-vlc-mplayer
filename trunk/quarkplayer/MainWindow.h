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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <phonon/phononnamespace.h>
#include <phonon/path.h>

#include <QtGui/QMainWindow>

namespace Ui { class MainWindow; }

class VideoWidget;
class PlayToolBar;

namespace Phonon {
	class MediaObject;
	class MediaController;
	class AudioOutput;
	class MediaSource;
}

class QStackedWidget;

/**
 * Main interface, the main window: QMainWindow.
 *
 * @author Tanguy Krotoff
 */
class MainWindow : public QMainWindow {
	Q_OBJECT
public:

	MainWindow(QWidget * parent);

	~MainWindow();

	PlayToolBar * playToolBar() const;

	VideoWidget * videoWidget() const;

	QStackedWidget * stackedWidget() const;

private slots:

	void addFiles();

	void about();

	void showConfigWindow();
	void showQuickSettingsWindow();

	void playDVD();
	void playURL();

	void addRecentFilesToMenu();
	void playRecentFile(int id);
	void clearRecentFiles();

	void play(const Phonon::MediaSource & mediaSource);

	void aboutToFinish();

	void sourceChanged(const Phonon::MediaSource & source);
	void metaDataChanged();
	void stateChanged(Phonon::State newState, Phonon::State oldState);

private:

	void closeEvent(QCloseEvent * event);

	Ui::MainWindow * _ui;

	Phonon::MediaObject * _mediaObject;

	/** Widget containing the video. */
	VideoWidget * _videoWidget;

	/** Widget containing the logo. */
	QWidget * _backgroundLogoWidget;

	Phonon::AudioOutput * _audioOutput;
	Phonon::Path _audioOutputPath;
	Phonon::MediaSource * _mediaSource;

	PlayToolBar * _playToolBar;
};

#endif	//MAINWINDOW_H