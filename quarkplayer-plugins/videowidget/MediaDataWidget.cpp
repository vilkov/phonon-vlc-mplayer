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

#include "MediaDataWidget.h"

#include "ui_MediaDataWidget.h"

#include <quarkplayer/config/Config.h>
#include <quarkplayer-plugins/mainwindow/MainWindow.h>

#include <mediainfowindow/MediaInfoWindow.h>
#include <mediainfofetcher/MediaInfoFetcher.h>

#include <contentfetcher/AmazonCoverArt.h>

#include <tkutil/MouseEventFilter.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/SqueezeLabel.h>

#include <phonon/mediaobject.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
#include <QtCore/QFile>
#include <QtCore/QCryptographicHash>

//Please don't copy this to another program; keys are free from aws.amazon.com
//Your Access Key ID:
static const char * AMAZON_WEB_SERVICE_ACCESS_KEY_ID = "1BPZGMNT4PWSJS6NHG02";
//Your Secret Access Key:
static const char * AMAZON_WEB_SERVICE_SECRET_KEY = "RfD3RoKwZ+5GpJa/i03jhoiDZM26OAc+TPpXMps0";

MediaDataWidget::MediaDataWidget(QWidget * parent)
	: QWidget(parent) {

	_mediaInfoFetcher = NULL;
	_coverArtFetcher = NULL;
	_coverArtSwitchTimer = NULL;

	_ui = new Ui::MediaDataWidget();
	_ui->setupUi(this);

	_mediaInfoWindow = NULL;
	connect(_ui->coverArtButton, SIGNAL(clicked()), SLOT(showMediaInfoWindow()));

	RETRANSLATE(this);
	retranslate();
}

MediaDataWidget::~MediaDataWidget() {
}

void MediaDataWidget::showMediaInfoWindow() {
	if (!_mediaInfoWindow) {
		//Lazy initialization
		_mediaInfoWindow = new MediaInfoWindow(QApplication::activeWindow());
	}
	_mediaInfoWindow->setMediaInfoFetcher(_mediaInfoFetcher);
	_mediaInfoWindow->setLanguage(Config::instance().language());
	_mediaInfoWindow->show();
}

void MediaDataWidget::startMediaInfoFetcher(Phonon::MediaObject * mediaObject) {
	_mediaInfoFetcher = new MediaInfoFetcher(this);
	connect(_mediaInfoFetcher, SIGNAL(fetched()), SLOT(updateMediaInfo()));
	Phonon::MediaSource mediaSource(mediaObject->currentSource());
	if (mediaSource.type() == Phonon::MediaSource::Url) {
		//Cannot solve meta data from a stream/remote media
		//if we are using a MediaSource
		//Use the given mediaObject to get the meta data
		_mediaInfoFetcher->start(mediaObject);
	} else {
		//MediaSource is not a URL
		//so everything is fine
		_mediaInfoFetcher->start(MediaInfo(mediaSource.fileName()),
			MediaInfoFetcher::ReadStyleAccurate);
	}
	if (_mediaInfoFetcher->mediaInfo().fetched()) {
		updateMediaInfo();
	}
}

void MediaDataWidget::updateMediaInfo() {
	if (!_mediaInfoFetcher) {
		return;
	}

	MediaInfo mediaInfo = _mediaInfoFetcher->mediaInfo();

	static const QString font("<font><b>");
	static const QString endfont("</b></font>");
	static const QString href("<a href=\"");
	static const QString endhref1("\">");
	static const QString endhref2("</a>");
	static const QString br("<br>");

	QString filename(mediaInfo.fileName());
	QString title(mediaInfo.metadataValue(MediaInfo::Title));
	QString artist(mediaInfo.metadataValue(MediaInfo::Artist));
	QString album(mediaInfo.metadataValue(MediaInfo::Album));
	QString albumArtist(mediaInfo.metadataValue(MediaInfo::AlbumArtist));
	QString amazonASIN(mediaInfo.metadataValue(MediaInfo::AmazonASIN));
	QString streamName(mediaInfo.networkStreamValue(MediaInfo::StreamName));
	QString streamGenre(mediaInfo.networkStreamValue(MediaInfo::StreamGenre));
	QString streamWebsite(mediaInfo.networkStreamValue(MediaInfo::StreamWebsite));
	QString streamUrl(mediaInfo.networkStreamValue(MediaInfo::StreamURL));
	QString bitrate(mediaInfo.bitrate());
	QString bitrateMode(mediaInfo.audioStreamValue(0, MediaInfo::AudioBitrateMode));

	_currentCoverArtIndex = 0;
	_coverArtList.clear();
	_currentAlbum = album;

	//It's better to try with the album artist first instead of the artist only,
	//more accurate, more chances to get a result
	if (albumArtist.isEmpty()) {
		loadCoverArt(album, artist, amazonASIN);
	} else {
		loadCoverArt(album, albumArtist, amazonASIN);
	}

	//Clean previous _ui->formLayout, remove all the rows previously added
	//This code seems to be slow and makes the widget "flash" :/
	for (int i = 0; i < _ui->formLayout->rowCount(); i++) {
		QLayoutItem * item = _ui->formLayout->itemAt(i, QFormLayout::LabelRole);
		if (item) {
			_ui->formLayout->removeItem(item);
			QWidget * widget = item->widget();
			if (widget) {
				delete widget;
			}
			delete item;
		}

		item = _ui->formLayout->itemAt(i, QFormLayout::FieldRole);
		if (item) {
			_ui->formLayout->removeItem(item);
			QWidget * widget = item->widget();
			if (widget) {
				delete widget;
			}
			delete item;
		}
	}
	///

	if (!title.isEmpty()) {
		_ui->formLayout->addRow(tr("Title:"), new SqueezeLabel(font + title + endfont));
	} else if (!filename.isEmpty()) {
		if (mediaInfo.isUrl()) {
			_ui->formLayout->addRow(tr("URL:"), new SqueezeLabel(font + filename + endfont));
		} else {
			//filename + parent directory name, e.g:
			// /home/tanguy/Music/DJ Vadim/Bluebird.mp3
			// --> DJ Vadim/Bluebird.mp3
			filename = QDir::toNativeSeparators(filename);
			int lastSlashPos = filename.lastIndexOf(QDir::separator()) - 1;
			QString tmp(filename.mid(filename.lastIndexOf(QDir::separator(), lastSlashPos) + 1));

			_ui->formLayout->addRow(tr("File:"), new SqueezeLabel(font + tmp + endfont));
		}
	}

	if (!artist.isEmpty()) {
		_ui->formLayout->addRow(tr("Artist:"), new SqueezeLabel(font + artist + endfont));
	}

	if (!album.isEmpty()) {
		_ui->formLayout->addRow(tr("Album:"), new SqueezeLabel(font + album + endfont));
	}

	if (!streamName.isEmpty()) {
		_ui->formLayout->addRow(tr("Stream Name:"), new SqueezeLabel(font + streamName + endfont));
	}

	if (!streamGenre.isEmpty()) {
		_ui->formLayout->addRow(tr("Stream Genre:"), new SqueezeLabel(font + streamGenre + endfont));
	}

	if (!streamWebsite.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamWebsite + endhref1 + font + streamWebsite + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_ui->formLayout->addRow(tr("Stream Website:"), label);
	}

	if (!streamUrl.isEmpty()) {
		SqueezeLabel * label = new SqueezeLabel();
		label->setText(href + streamUrl + endhref1 + font + streamUrl + endfont + endhref2);
		label->setOpenExternalLinks(true);
		_ui->formLayout->addRow(tr("URL:"), label);
	}

	if (!bitrate.isEmpty()) {
		_ui->formLayout->addRow(tr("Bitrate:"),
			new SqueezeLabel(font + bitrate + ' ' + tr("kbps") + ' ' + bitrateMode + endfont));
	}
}

void MediaDataWidget::retranslate() {
	updateMediaInfo();
	//Crash inside Ui_MediaDataWidget::retranslateUi(QWidget *)
	//pushTheCoverArtOnTheLeftLabel was deleted before
	//_ui->retranslateUi(this);

	if (_mediaInfoWindow) {
		_mediaInfoWindow->setLanguage(Config::instance().language());
	}
}

void MediaDataWidget::loadCoverArt(const QString & album, const QString & artist, const QString & amazonASIN) {
	QString coverArtDir(QFileInfo(_mediaInfoFetcher->mediaInfo().fileName()).path());

	QStringList imageSuffixList;
	foreach (QByteArray format, QImageReader::supportedImageFormats()) {
		QString suffix(format);
		suffix = suffix.toLower();
		imageSuffixList << "*." + suffix;
	}

	QDir path(coverArtDir);
	if (path.exists()) {
		QFileInfoList fileList = path.entryInfoList(imageSuffixList, QDir::Files);
		foreach (QFileInfo fileInfo, fileList) {
			if (fileInfo.size() > 0) {
				QString filename(fileInfo.absoluteFilePath());
				_coverArtList << filename;
			}
		}
	}

	if (!album.isEmpty() && !artist.isEmpty()) {
		//Download the cover only if album + artist are not empty

		QString amazonCoverArtFilename(artist + " - " + album + ".jpg");
		//Remove characters not allowed inside a filename
		static const QChar space(' ');
		amazonCoverArtFilename.replace('/', space);
		amazonCoverArtFilename.replace("\\", space);
		amazonCoverArtFilename.replace(':', space);
		amazonCoverArtFilename.replace('*', space);
		amazonCoverArtFilename.replace('?', space);
		amazonCoverArtFilename.replace('>', space);
		amazonCoverArtFilename.replace('>', space);
		amazonCoverArtFilename.replace('|', space);

		//Do not use QDir::separator() since _coverArtList contains / separators
		//even under Windows
		_amazonCoverArtPath = coverArtDir + '/' + amazonCoverArtFilename;

		bool amazonCoverArtAlreadyDownloaded = _coverArtList.contains(_amazonCoverArtPath);
		if (!amazonCoverArtAlreadyDownloaded) {
			//Download the cover art
			if (!_coverArtFetcher) {
				//Lazy initialization
				_coverArtFetcher = new AmazonCoverArt(AMAZON_WEB_SERVICE_ACCESS_KEY_ID, AMAZON_WEB_SERVICE_SECRET_KEY, this);
				connect(_coverArtFetcher,
					SIGNAL(contentFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, bool, const ContentFetcherTrack &)),
					SLOT(coverArtFound(QNetworkReply::NetworkError, const QUrl &, const QByteArray &, bool, const ContentFetcherTrack &))
				);
			}
			ContentFetcherTrack track;
			track.artist = artist;
			track.album = album;
			track.amazonASIN = amazonASIN;
			_coverArtFetcher->start(track);
		}
	}

	if (!_coverArtSwitchTimer) {
		//Lazy initialization
		_coverArtSwitchTimer = new QTimer(this);
		_coverArtSwitchTimer->setInterval(4000);
		connect(_coverArtSwitchTimer, SIGNAL(timeout()), SLOT(updateCoverArtPixmap()));
	}
	//Restarts the timer
	_coverArtSwitchTimer->start();
	updateCoverArtPixmap();
}

void MediaDataWidget::coverArtFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & coverArt,
	bool accurate, const ContentFetcherTrack & track) {

	if (error == QNetworkReply::NoError) {
		//Check if the cover art received does match the current album playing
		//network replies can be too long
		if ((_currentAlbum != track.album) || !accurate) {
			return;
		}

		//Saves the downloaded cover art to a file
		QFile coverArtFile(_amazonCoverArtPath);
		if (coverArtFile.open(QIODevice::WriteOnly)) {
			//The file could be opened
			qint64 ret = coverArtFile.write(coverArt);
			if (ret != -1) {
				//The file could be written
				coverArtFile.close();

				showCoverArtStatusMessage(tr("Amazon cover art downloaded: ") + _amazonCoverArtPath + " " + url.toString());

				_coverArtList << _amazonCoverArtPath;
			} else {
				qCritical() << __FUNCTION__ << "Error: cover art file couldn't be written:" << _amazonCoverArtPath;
			}
		} else {
			qCritical() << __FUNCTION__ << "Error: cover art file couldn't be opened:" << _amazonCoverArtPath;
		}
	} else {
		//Check if the cover art received does match the current album playing
		//network replies can be too long
		if ((_currentAlbum != track.album)) {
			return;
		}

		showCoverArtStatusMessage(tr("Amazon cover art error: ") + ContentFetcher::errorString(error) + " " + url.toString());
	}
}

void MediaDataWidget::showCoverArtStatusMessage(const QString & message) const {
	MainWindow * mainWindow = MainWindowFactory::mainWindow();
	if (mainWindow && mainWindow->statusBar()) {
		mainWindow->statusBar()->showMessage(message);
	}
}

void MediaDataWidget::updateCoverArtPixmap() {
	if (!_coverArtList.isEmpty()) {
		if (_currentCoverArtIndex >= _coverArtList.size()) {
			//Restart from the beginning
			_currentCoverArtIndex = 0;
		}
		if (_coverArtList.size() == 1) {
			//Only one cover art, update the cover art once and then stops the timer
			_coverArtSwitchTimer->stop();
		}

		//Update the cover art pixmap
		QString filename(_coverArtList[_currentCoverArtIndex]);
		QPixmap coverArt(filename);
		if (!coverArt.isNull()) {
			_ui->coverArtButton->setIcon(coverArt);
		} else {
			qCritical() << __FUNCTION__ << "Error: cover art image is empty";
		}

		_currentCoverArtIndex++;
	} else {
		_ui->coverArtButton->setIcon(QIcon(":/icons/quarkplayer-128x128.png"));
	}
}
