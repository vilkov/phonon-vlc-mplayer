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

#include "MainWindow.h"

#include "AboutWindow.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>
#include <quarkplayer/config/Config.h>
#include <quarkplayer/version.h>

#include <quarkplayer-plugins/playlist/PlaylistWidget.h>
#include <quarkplayer-plugins/playlist/PlaylistModel.h>

#include <tkutil/ActionCollection.h>
#include <tkutil/TkAction.h>
#include <tkutil/TkIcon.h>
#include <tkutil/TkFileDialog.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <filetypes/FileTypes.h>

#include <qtsingleapplication/QtSingleApplication>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <phonon/audiooutput.h>

#include <QtCore/QSignalMapper>

#include <QtGui/QtGui>

#include <cstdio>

Q_EXPORT_PLUGIN2(mainwindow, MainWindowFactory);

const char * MainWindowFactory::PLUGIN_NAME = "mainwindow";

QStringList MainWindowFactory::dependencies() const {
	QStringList tmp;
	return tmp;
}

PluginInterface * MainWindowFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new MainWindow(quarkPlayer, uuid);
}

MainWindow * MainWindowFactory::mainWindow() {
	MainWindow * mainWindow = dynamic_cast<MainWindow *>(PluginManager::instance().pluginInterface(PLUGIN_NAME));
	Q_ASSERT(mainWindow);
	return mainWindow;
}

MainWindow::MainWindow(QuarkPlayer & quarkPlayer, const QUuid & uuid)
	: TkMainWindow(NULL),
	PluginInterface(quarkPlayer, uuid) {

	QtSingleApplication * app = qobject_cast<QtSingleApplication *>(QCoreApplication::instance());
	app->setActivationWindow(this);
	app->activateWindow();

	populateActionCollection();

	setupUi();

	//Accepts Drag&Drop
	setAcceptDrops(true);

	//DockWidgets tabs are vertical like in Amarok
	setDockOptions(QMainWindow::VerticalTabs);

	_playToolBar = NULL;
	_statusBar = NULL;

	connect(ActionCollection::action("MainWindow.OpenFile"), SIGNAL(triggered()), SLOT(playFile()));
	connect(ActionCollection::action("MainWindow.OpenDVD"), SIGNAL(triggered()), SLOT(playDVD()));
	connect(ActionCollection::action("MainWindow.OpenURL"), SIGNAL(triggered()), SLOT(playURL()));
	connect(ActionCollection::action("MainWindow.OpenVCD"), SIGNAL(triggered()), SLOT(playVCD()));
	connect(ActionCollection::action("MainWindow.NewMediaObject"), SIGNAL(triggered()), &quarkPlayer, SLOT(createNewMediaObject()));
	connect(ActionCollection::action("MainWindow.Quit"), SIGNAL(triggered()), SLOT(close()));
	connect(ActionCollection::action("MainWindow.ReportBug"), SIGNAL(triggered()), SLOT(reportBug()));
	connect(ActionCollection::action("MainWindow.ShowMailingList"), SIGNAL(triggered()), SLOT(showMailingList()));
	connect(ActionCollection::action("MainWindow.About"), SIGNAL(triggered()), SLOT(about()));
	connect(ActionCollection::action("MainWindow.AboutQt"), SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	connect(ActionCollection::action("MainWindow.VolumeMute"), SIGNAL(toggled(bool)), SLOT(mutedToggled(bool)));

	connect(&quarkPlayer, SIGNAL(currentMediaObjectChanged(Phonon::MediaObject *)),
		SLOT(currentMediaObjectChanged(Phonon::MediaObject *)));

	bool ok = restoreGeometry(Config::instance().mainWindowGeometry());
	if (!ok) {
		qCritical() << __FUNCTION__ << "Error: coudn't restore the main window geometry";
	}

	RETRANSLATE(this);
	retranslate();

	show();
}

MainWindow::~MainWindow() {
	//If the MainWindow is destroyed, it's reasonable to say
	//that we can quit the entire application
	QCoreApplication::quit();
}

void MainWindow::setPlayToolBar(QToolBar * playToolBar) {
	_playToolBar = playToolBar;
	addToolBar(Qt::BottomToolBarArea, playToolBar);
	qDebug() << __FUNCTION__;
	emit playToolBarAdded(_playToolBar);
}

QToolBar * MainWindow::playToolBar() const {
	return _playToolBar;
}

void MainWindow::setStatusBar(QStatusBar * statusBar) {
	_statusBar = statusBar;
	if (_statusBar) {
		//If statusBar == NULL then QMainWindow::setStatusBar(NULL)
		//will call statusBar destructor
		//This can lead to an infinite loop since the statusbar plugin
		//does a MainWindow::setStatusBar(NULL) inside its destructor
		QMainWindow::setStatusBar(statusBar);
		emit statusBarAdded(_statusBar);
	}
}

QStatusBar * MainWindow::statusBar() const {
	return _statusBar;
}

void MainWindow::playFile() {
	QStringList filenames = TkFileDialog::getOpenFileNames(
		this, tr("Select Audio/Video File"), Config::instance().lastDirOpened(),
		tr("Multimedia") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video, FileType::Audio)) + ";;" +
		tr("Video") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Video)) +";;" +
		tr("Audio") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Audio)) +";;" +
		tr("Playlist") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Playlist)) + ";;" +
		tr("All Files") + " (*)"
	);

	if (!filenames.isEmpty()) {
		QString fileToPlay(filenames[0]);
		Config::instance().setValue(Config::LAST_DIR_OPENED_KEY, QFileInfo(fileToPlay).absolutePath());

		PlaylistWidget * playlistWidget = PlaylistWidgetFactory::playlistWidget();
		if (playlistWidget) {
			playlistWidget->addFilesToCurrentPlaylist(filenames);
			playlistWidget->playlistModel()->play(0);
		} else {
			play(fileToPlay);
		}
	}
}

void MainWindow::playDVD() {
	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select DVD folder"),
			Config::instance().dvdDir());

	if (!dir.isEmpty()) {
		Config::instance().setValue(Config::DVD_DIR_KEY, dir);
		play(Phonon::MediaSource(Phonon::Dvd, dir));
	}
}

void MainWindow::playURL() {
	QString url = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"));

	if (!url.isEmpty()) {
		play(url);
	}
}

void MainWindow::playVCD() {
	QString dir = TkFileDialog::getExistingDirectory(this, tr("Select VCD folder"),
			Config::instance().cdromDir());

	if (!dir.isEmpty()) {
		Config::instance().setValue(Config::CDROM_DIR_KEY, dir);
		play(Phonon::MediaSource(Phonon::Vcd, dir));
	}
}

void MainWindow::play(const Phonon::MediaSource & mediaSource) {
	quarkPlayer().play(mediaSource);
	//_statusBar->showMessage(tr("Processing") + " " + filename + "...");
}

void MainWindow::updateWindowTitle() {
	QString title = quarkPlayer().currentMediaObjectTitle();

	if (title.isEmpty()) {
		setWindowTitle(QCoreApplication::applicationName() + "-" + QString(QUARKPLAYER_VERSION));
	} else {
		setWindowTitle(title + " - " + QCoreApplication::applicationName());
	}
}

void MainWindow::reportBug() {
	QDesktopServices::openUrl(QUrl("http://code.google.com/p/phonon-vlc-mplayer/issues/list"));
}

void MainWindow::showMailingList() {
	QDesktopServices::openUrl(QUrl("http://groups.google.com/group/phonon-vlc-mplayer"));
}

void MainWindow::about() {
	static AboutWindow * aboutWindow = new AboutWindow(this);
	aboutWindow->show();
}

void MainWindow::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	ActionCollection::addAction("MainWindow.OpenFile", new TkAction(app, QKeySequence::Open));
	ActionCollection::addAction("MainWindow.Quit", new TkAction(app, tr("Ctrl+Q"), tr("Alt+X")));
	ActionCollection::addAction("MainWindow.ReportBug", new QAction(app));
	ActionCollection::addAction("MainWindow.ShowMailingList", new QAction(app));
	ActionCollection::addAction("MainWindow.About", new TkAction(app, tr("Ctrl+F1")));
	ActionCollection::addAction("MainWindow.AboutQt", new QAction(app));
	ActionCollection::addAction("MainWindow.OpenDVD", new TkAction(app, tr("Ctrl+D")));
	ActionCollection::addAction("MainWindow.OpenURL", new TkAction(app, tr("Ctrl+U")));
	ActionCollection::addAction("MainWindow.OpenVCD", new QAction(app));
	ActionCollection::addAction("MainWindow.NewMediaObject", new QAction(app));
	ActionCollection::addAction("MainWindow.Equalizer", new TkAction(app, tr("Ctrl+E")));
	ActionCollection::addAction("MainWindow.Configure", new QAction(app));
	ActionCollection::addAction("MainWindow.EmptyMenu", new QAction(app));

	TkAction * action = new TkAction(app, tr("Space"), Qt::Key_MediaPlay, Qt::Key_Pause);
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.PlayPause", action);
	action = new TkAction(app, Qt::Key_MediaStop);
	ActionCollection::addAction("MainWindow.Stop", action);
	action = new TkAction(app, tr("Ctrl+N"), tr(">"), Qt::Key_MediaNext);
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.NextTrack", action);
	action = new TkAction(app, tr("Ctrl+P"), tr("<"), Qt::Key_MediaPrevious);
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.PreviousTrack", action);

	action = new TkAction(app, tr("Shift+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.JumpBackward10s", action);
	action = new TkAction(app, tr("Ctrl+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.JumpBackward1min", action);
	action = new TkAction(app, tr("Alt+Left"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.JumpBackward10min", action);

	action = new TkAction(app, tr("Shift+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.JumpForward10s", action);
	action = new TkAction(app, tr("Ctrl+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.JumpForward1min", action);
	action = new TkAction(app, tr("Alt+Right"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.JumpForward10min", action);

	action = new TkAction(app, tr("["));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.SpeedDecrease10%", action);
	action = new TkAction(app, tr("]"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.SpeedIncrease10%", action);

	action = new TkAction(app, tr("Ctrl+M"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	ActionCollection::addAction("MainWindow.VolumeMute", action);

	action = new TkAction(app, tr("Ctrl+Down"), tr("-"), tr("Alt+-"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.VolumeDecrease10%", action);
	action = new TkAction(app, tr("Ctrl+Up"), tr("+"), tr("Alt++"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	ActionCollection::addAction("MainWindow.VolumeIncrease10%", action);

	//FIXME See MainWindow.cpp MediaController.cpp FindSubtitles.cpp QuarkPlayer.h
	//Need to implement a full plugin system like Qt Creator has
	//Let's wait for Qt Creator source code to be released...
	//This way MainWindow would be also a real plugin!
	ActionCollection::addAction("MainWindow.FindSubtitles", new QAction(app));
	ActionCollection::addAction("MainWindow.UploadSubtitles", new QAction(app));
	///

	action = new TkAction(app, tr("Ctrl+F"), tr("Alt+Return"));
	action->setShortcutContext(Qt::ApplicationShortcut);
	action->setCheckable(true);
	ActionCollection::addAction("MainWindow.FullScreen", action);

	action = new TkAction(app, tr("Esc"));
	ActionCollection::addAction("MainWindow.FullScreenLeave", action);
}

void MainWindow::setupUi() {
	//No central widget, only QDockWidget
	setCentralWidget(NULL);

	_menuFile = new QMenu();
	menuBar()->addMenu(_menuFile);
	_menuFile->addAction(ActionCollection::action("MainWindow.OpenFile"));
	_menuFile->addAction(ActionCollection::action("MainWindow.OpenDVD"));
	_menuFile->addAction(ActionCollection::action("MainWindow.OpenURL"));
	_menuFile->addAction(ActionCollection::action("MainWindow.OpenVCD"));
	_menuFile->addSeparator();
	_menuFile->addAction(ActionCollection::action("MainWindow.Quit"));

	_menuPlay = new QMenu();
	menuBar()->addMenu(_menuPlay);
	_menuPlay->addAction(ActionCollection::action("MainWindow.PreviousTrack"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.PlayPause"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.Stop"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.NextTrack"));
	_menuPlay->addSeparator();
	_menuPlay->addAction(ActionCollection::action("MainWindow.JumpBackward10s"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.JumpBackward1min"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.JumpBackward10min"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.JumpForward10s"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.JumpForward1min"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.JumpForward10min"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.SpeedDecrease10%"));
	_menuPlay->addAction(ActionCollection::action("MainWindow.SpeedIncrease10%"));
	_menuPlay->addSeparator();
	_menuPlay->addAction(ActionCollection::action("MainWindow.FullScreen"));
	_menuPlay->addSeparator();
	_menuPlay->addAction(ActionCollection::action("MainWindow.NewMediaObject"));

	_menuAudio = new QMenu();
	menuBar()->addMenu(_menuAudio);
	_menuAudio->addAction(ActionCollection::action("MainWindow.VolumeMute"));
	_menuAudio->addAction(ActionCollection::action("MainWindow.VolumeDecrease10%"));
	_menuAudio->addAction(ActionCollection::action("MainWindow.VolumeIncrease10%"));

	_menuSettings = new QMenu();
	menuBar()->addMenu(_menuSettings);
	_menuSettings->addAction(ActionCollection::action("MainWindow.Equalizer"));
	_menuSettings->addAction(ActionCollection::action("MainWindow.Configure"));

	_menuHelp = new QMenu();
	menuBar()->addMenu(_menuHelp);
	_menuHelp->addAction(ActionCollection::action("MainWindow.ReportBug"));
	_menuHelp->addAction(ActionCollection::action("MainWindow.ShowMailingList"));
	_menuHelp->addAction(ActionCollection::action("MainWindow.About"));
	_menuHelp->addAction(ActionCollection::action("MainWindow.AboutQt"));

	//Main ToolBar
	_mainToolBar = new TkToolBar(this);
	//_mainToolBar->setIconSize(QSize(16, 16));

	_mainToolBar->addAction(ActionCollection::action("MainWindow.OpenFile"));
	_mainToolBar->addAction(ActionCollection::action("MainWindow.OpenDVD"));
	_mainToolBar->addAction(ActionCollection::action("MainWindow.OpenURL"));
	_mainToolBar->addSeparator();
	_mainToolBar->addAction(ActionCollection::action("MainWindow.Equalizer"));
	_mainToolBar->addAction(ActionCollection::action("MainWindow.Configure"));
	addToolBar(_mainToolBar);
}

void MainWindow::retranslate() {
	updateWindowTitle();
	setWindowIcon(QIcon(":/icons/quarkplayer-16x16.png"));

	ActionCollection::action("MainWindow.OpenFile")->setText(tr("Play &File..."));
	ActionCollection::action("MainWindow.OpenFile")->setIcon(TkIcon("document-open"));

	ActionCollection::action("MainWindow.Quit")->setText(tr("&Quit"));
	ActionCollection::action("MainWindow.Quit")->setIcon(TkIcon("application-exit"));

	ActionCollection::action("MainWindow.ReportBug")->setText(tr("&Report a bug..."));
	ActionCollection::action("MainWindow.ReportBug")->setIcon(TkIcon("tools-report-bug"));

	ActionCollection::action("MainWindow.ShowMailingList")->setText(tr("&Discuss about QuarkPlayer..."));
	ActionCollection::action("MainWindow.ShowMailingList")->setIcon(TkIcon("mail-mark-unread"));

	ActionCollection::action("MainWindow.About")->setText(tr("&About"));
	ActionCollection::action("MainWindow.About")->setIcon(TkIcon("help-about"));

	ActionCollection::action("MainWindow.AboutQt")->setText(tr("About &Qt"));
	ActionCollection::action("MainWindow.AboutQt")->setIcon(TkIcon("help-about"));

	ActionCollection::action("MainWindow.OpenDVD")->setText(tr("Play &DVD..."));
	ActionCollection::action("MainWindow.OpenDVD")->setIcon(TkIcon("media-optical"));

	ActionCollection::action("MainWindow.OpenURL")->setText(tr("Play &URL..."));
	ActionCollection::action("MainWindow.OpenURL")->setIcon(TkIcon("document-open-remote"));

	ActionCollection::action("MainWindow.OpenVCD")->setText(tr("Play &VCD..."));
	//ActionCollection::action("MainWindow.OpenVCD")->setIcon(TkIcon("media-optical"));

	ActionCollection::action("MainWindow.NewMediaObject")->setText(tr("New Media Window"));
	ActionCollection::action("MainWindow.NewMediaObject")->setIcon(TkIcon("window-new"));

	ActionCollection::action("MainWindow.Equalizer")->setText(tr("&Equalizer..."));
	ActionCollection::action("MainWindow.Equalizer")->setIcon(TkIcon("view-media-equalizer"));

	ActionCollection::action("MainWindow.Configure")->setText(tr("&Configure QuarkPlayer..."));
	ActionCollection::action("MainWindow.Configure")->setIcon(TkIcon("preferences-system"));

	ActionCollection::action("MainWindow.EmptyMenu")->setText(tr("<empty>"));
	ActionCollection::action("MainWindow.EmptyMenu")->setEnabled(false);

	_mainToolBar->setWindowTitle(tr("Main ToolBar"));
	_mainToolBar->setMinimumSize(_mainToolBar->sizeHint());

	_menuFile->setTitle(tr("&File"));
	_menuPlay->setTitle(tr("&Play"));
	_menuAudio->setTitle(tr("&Audio"));
	_menuSettings->setTitle(tr("&Settings"));
	_menuHelp->setTitle(tr("&Help"));

	ActionCollection::action("MainWindow.PreviousTrack")->setText(tr("P&revious Track"));
	ActionCollection::action("MainWindow.PreviousTrack")->setIcon(TkIcon("media-skip-backward"));

	ActionCollection::action("MainWindow.PlayPause")->setText(tr("&Play/Pause"));
	ActionCollection::action("MainWindow.PlayPause")->setIcon(TkIcon("media-playback-start"));

	ActionCollection::action("MainWindow.Stop")->setText(tr("&Stop"));
	ActionCollection::action("MainWindow.Stop")->setIcon(TkIcon("media-playback-stop"));

	ActionCollection::action("MainWindow.NextTrack")->setText(tr("&Next Track"));
	ActionCollection::action("MainWindow.NextTrack")->setIcon(TkIcon("media-skip-forward"));

	ActionCollection::action("MainWindow.JumpBackward10s")->setText(tr("Jump &Backward 10s"));
	ActionCollection::action("MainWindow.JumpBackward10s")->setIcon(TkIcon("media-seek-backward"));
	ActionCollection::action("MainWindow.JumpBackward1min")->setText(tr("Jump &Backward 1min"));
	ActionCollection::action("MainWindow.JumpBackward1min")->setIcon(TkIcon("media-seek-backward"));
	ActionCollection::action("MainWindow.JumpBackward10min")->setText(tr("Jump &Backward 10min"));
	ActionCollection::action("MainWindow.JumpBackward10min")->setIcon(TkIcon("media-seek-backward"));
	ActionCollection::action("MainWindow.JumpForward10s")->setText(tr("Jump &Forward 10s"));
	ActionCollection::action("MainWindow.JumpForward10s")->setIcon(TkIcon("media-seek-forward"));
	ActionCollection::action("MainWindow.JumpForward1min")->setText(tr("Jump &Forward 1min"));
	ActionCollection::action("MainWindow.JumpForward1min")->setIcon(TkIcon("media-seek-forward"));
	ActionCollection::action("MainWindow.JumpForward10min")->setText(tr("Jump &Forward 10min"));
	ActionCollection::action("MainWindow.JumpForward10min")->setIcon(TkIcon("media-seek-forward"));
	ActionCollection::action("MainWindow.SpeedDecrease10%")->setText(tr("Decrease Speed"));
	ActionCollection::action("MainWindow.SpeedDecrease10%")->setIcon(TkIcon("media-seek-backward"));
	ActionCollection::action("MainWindow.SpeedIncrease10%")->setText(tr("Increase Speed"));
	ActionCollection::action("MainWindow.SpeedIncrease10%")->setIcon(TkIcon("media-seek-forward"));

	ActionCollection::action("MainWindow.VolumeMute")->setText(tr("Mute"));
	ActionCollection::action("MainWindow.VolumeMute")->setIcon(TkIcon("audio-volume-muted"));
	ActionCollection::action("MainWindow.VolumeDecrease10%")->setText(tr("Decrease Volume"));
	ActionCollection::action("MainWindow.VolumeDecrease10%")->setIcon(TkIcon("audio-volume-low"));
	ActionCollection::action("MainWindow.VolumeIncrease10%")->setText(tr("Increase Volume"));
	ActionCollection::action("MainWindow.VolumeIncrease10%")->setIcon(TkIcon("audio-volume-high"));

	ActionCollection::action("MainWindow.FullScreen")->setText(tr("&Fullscreen"));
	ActionCollection::action("MainWindow.FullScreen")->setIcon(TkIcon("view-fullscreen"));

	ActionCollection::action("MainWindow.FullScreenLeave")->setText(tr("Leave Fullscreen"));
}

QMenu * MainWindow::menuFile() const {
	return _menuFile;
}

QMenu * MainWindow::menuPlay() const {
	return _menuPlay;
}

QMenu * MainWindow::menuAudio() const {
	return _menuAudio;
}

QMenu * MainWindow::menuSettings() const {
	return _menuSettings;
}

QMenu * MainWindow::menuHelp() const {
	return _menuHelp;
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event) {
	qDebug() << __FUNCTION__ << event->mimeData()->formats();

	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * event) {
	QStringList files;

	//Add urls to a list
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urlList = event->mimeData()->urls();
		QString filename;
		foreach (QUrl url, urlList) {
			if (url.isValid()) {
				qDebug() << __FUNCTION__ << "File scheme:" << url.scheme();
				if (url.scheme() == "file") {
					filename = url.toLocalFile();
				} else {
					filename = url.toString();
				}
				qDebug() << __FUNCTION__ << "File dropped:" << filename;
				files << filename;
			}
		}
	}

	if (files.count() > 0) {
		if (files.count() == 1) {
			//1 file
			QString filename = files[0];

			bool isSubtitle = FileTypes::extensions(FileType::Subtitle).contains(QFileInfo(filename).completeSuffix(), Qt::CaseInsensitive);
			if (isSubtitle) {
				qDebug() << __FUNCTION__ << "Loading subtitle:" << filename;
				emit subtitleFileDropped(filename);
			} else if (QFileInfo(filename).isDir()) {
				//TODO open directory
			} else {
				//TODO add to playlist if 'auto-add-playlist' option
				play(filename);
			}
		} else {
			//Several files

			//TODO add files to playlist
			//and play the first one in the list
		}
	}
}

void MainWindow::closeEvent(QCloseEvent * event) {
	//Saves window geometry
	Config::instance().setValue(Config::MAINWINDOW_GEOMETRY_KEY, saveGeometry());

	TkMainWindow::closeEvent(event);

	event->accept();

	//Quits the application
	//QCoreApplication::quit();

	//FIXME we should only use QCoreApplication::quit()
	//exit(EXIT_SUCCESS);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockWidget, QDockWidget * lastDockWidget) {
	if (dockWidget) {
		QMainWindow::addDockWidget(area, dockWidget);
		if (lastDockWidget) {
			tabifyDockWidget(lastDockWidget, dockWidget);
		}
	}
}

void MainWindow::addBrowserDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::LeftDockWidgetArea, dockWidget, lastDockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetBrowserDockWidget() {
	addBrowserDockWidget(NULL);
}

void MainWindow::addVideoDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, dockWidget, lastDockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetVideoDockWidget() {
	addVideoDockWidget(NULL);
}

void MainWindow::addPlaylistDockWidget(QDockWidget * dockWidget) {
	static QDockWidget * lastDockWidget = NULL;
	addDockWidget(Qt::RightDockWidgetArea, dockWidget, lastDockWidget);
	lastDockWidget = dockWidget;
}

void MainWindow::resetPlaylistDockWidget() {
	addPlaylistDockWidget(NULL);
}

void MainWindow::currentMediaObjectChanged(Phonon::MediaObject * mediaObject) {
	foreach (Phonon::MediaObject * tmp, quarkPlayer().mediaObjectList()) {
		tmp->disconnect(this);
	}

	//Resets the main window title
	updateWindowTitle();
	connect(mediaObject, SIGNAL(metaDataChanged()), SLOT(updateWindowTitle()));

	disconnect(ActionCollection::action("MainWindow.Quit"), SIGNAL(triggered()), mediaObject, SLOT(stop()));
	connect(ActionCollection::action("MainWindow.Quit"), SIGNAL(triggered()), mediaObject, SLOT(stop()));

	Phonon::AudioOutput * audioOutput = quarkPlayer().currentAudioOutput();
	if (audioOutput) {
		//Avoid a crash inside Phonon if the backend couldn't be loaded
		ActionCollection::action("MainWindow.VolumeMute")->setChecked(audioOutput->isMuted());
		disconnect(audioOutput, SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)));
		connect(audioOutput, SIGNAL(mutedChanged(bool)), SLOT(mutedChanged(bool)));
	}
}

void MainWindow::mutedChanged(bool muted) {
	ActionCollection::action("MainWindow.VolumeMute")->setChecked(muted);
}

void MainWindow::mutedToggled(bool muted) {
	quarkPlayer().currentAudioOutput()->setMuted(muted);
}
