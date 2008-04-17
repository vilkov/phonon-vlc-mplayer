/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MPlayerProcess.h"

#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QtDebug>

#include "MPlayerVersion.h"

#ifdef Q_OS_WIN
	static const char * MPLAYER_EXE = "C:/Program Files/SMPlayer/mplayer/mplayer.exe";
#else
	static const char * MPLAYER_EXE = "mplayer";
#endif

/** MPlayer works using seconds, we prefer to work using milliseconds. */
static const double SECONDS_CONVERTION = 1000.0;

MPlayerProcess::MPlayerProcess(QObject * parent)
	: MyProcess(parent) {

	connect(this, SIGNAL(lineAvailable(const QByteArray &)),
		SLOT(parseLine(const QByteArray &)));

	connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(finished(int, QProcess::ExitStatus)));
}

MPlayerProcess::~MPlayerProcess() {
}

bool MPlayerProcess::start(const QStringList & arguments, const QString & filename, int videoWidgetId, qint64 seek) {
	//Stop MPlayerProcess if it is already running
	if (isRunning()) {
		stop();
	}

	_data.clear();

	//Not found yet
	_mplayerSvnRevision = -1;

	_endOfFileReached = false;


	QStringList args;
	args << arguments;

	//Attach MPlayer video output to our widget
	if (videoWidgetId > 0) {
		_data.videoWidgetId = videoWidgetId;
		args << "-wid";
		args << QString::number(_data.videoWidgetId);
	}

	//If seek < 5 it's better to allow the video to start from the beginning
	if (seek > 5) {
		_data.currentTime = seek;
		args << "-ss";
		args << QString::number(_data.currentTime / SECONDS_CONVERTION);
	}

	//File to play
	_data.filename = filename;
	args << filename;


	MyProcess::start(MPLAYER_EXE, args);
	return waitForStarted();
}

void MPlayerProcess::stop() {
	if (!isRunning()) {
		qWarning() << __FUNCTION__ << "MPlayer not running";
		return;
	}

	writeToStdin("quit");

	qDebug() << __FUNCTION__ << "Finishing MPlayer...";
	if (!waitForFinished(5000)) {
		kill();
	}

	qDebug() << __FUNCTION__ << "MPlayer finished";
}

void MPlayerProcess::writeToStdin(const QString & command) {
	qDebug() << __FUNCTION__ << "Command:" << command;

	if (isRunning()) {
		write(command.toLocal8Bit() + "\n");
	} else {
		qWarning() << __FUNCTION__ << "Error: MPlayer process not running";
	}
}

MediaData MPlayerProcess::getMediaData() const {
	return _data;
}

bool MPlayerProcess::hasVideo() const {
	return _data.hasVideo;
}

bool MPlayerProcess::isSeekable() const {
	return _data.isSeekable;
}

qint64 MPlayerProcess::currentTime() const {
	return _data.currentTime;
}

qint64 MPlayerProcess::totalTime() const {
	return _data.totalTime;
}


static QRegExp rx_av("^[AV]: *([0-9,:.-]+)");
static QRegExp rx_frame("^[AV]:.* (\\d+)\\/.\\d+");// [0-9,.]+");
static QRegExp rx("^(.*)=(.*)");
static QRegExp rx_audio_mat("^ID_AID_(\\d+)_(LANG|NAME)=(.*)");
static QRegExp rx_title("^ID_DVD_TITLE_(\\d+)_(LENGTH|CHAPTERS|ANGLES)=(.*)");
static QRegExp rx_winresolution("^VO: \\[(.*)\\] (\\d+)x(\\d+) => (\\d+)x(\\d+)");
static QRegExp rx_ao("^AO: \\[(.*)\\]");
static QRegExp rx_paused("^ID_PAUSED");
static QRegExp rx_novideo("^Video: no video");
static QRegExp rx_cache("^Cache fill:.*");
static QRegExp rx_create_index("^Generating Index:.*");
static QRegExp rx_play("^Starting playback...");
static QRegExp rx_playing("^Playing");	//"Playing" does not mean the file is actually playing but only loading
static QRegExp rx_connecting("^Connecting to .*");
//Future messages to add:
//Connection timeout
//Failed, exiting
//No stream found to handle url
static QRegExp rx_resolving("^Resolving .*");
static QRegExp rx_screenshot("^\\*\\*\\* screenshot '(.*)'");
static QRegExp rx_endoffile("^Exiting... \\(End of file\\)");
static QRegExp rx_mkvchapters("\\[mkv\\] Chapter (\\d+) from");

//VCD
static QRegExp rx_vcd("^ID_VCD_TRACK_(\\d+)_MSF=(.*)");

//Audio CD
static QRegExp rx_cdda("^ID_CDDA_TRACK_(\\d+)_MSF=(.*)");

//Subtitles
static QRegExp rx_subtitle("^ID_(SUBTITLE|FILE_SUB|VOBSUB)_ID=(\\d+)");
static QRegExp rx_sid("^ID_(SID|VSID)_(\\d+)_(LANG|NAME)=(.*)");
static QRegExp rx_subtitle_file("^ID_FILE_SUB_FILENAME=(.*)");

//Clip info
static QRegExp rx_clip_name("^(name|title): (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_artist("^artist: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_author("^author: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_album("^album: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_genre("^genre: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_date("^(creation date|year): (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_track("^track: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_copyright("^copyright: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_comment("^comment: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_software("^software: (.*)", Qt::CaseInsensitive);

static QRegExp rx_stream_title("^.* StreamTitle='(.*)';StreamUrl='(.*)';");

void MPlayerProcess::parseLine(const QByteArray & tmp) {
	//qDebug() << __FUNCTION__ << tmp;

	QString line = QString::fromLocal8Bit(tmp);

	//Skip empty lines
	if (line.isEmpty()) {
		return;
	}

	//Parse A: V: line
	if (rx_av.indexIn(line) > -1) {
		_data.currentTime = rx_av.cap(1).toDouble() * SECONDS_CONVERTION;

		if (_state != PlayingState) {
			qDebug() << __FUNCTION__ << "Starting time:" << _data.currentTime;
			setState(PlayingState);

			//OK, now all the media datas should be in clean state
			emit mediaDataChanged();
		}

		//qDebug() << __FUNCTION__ << "Tick:" << _data.currentTime;
		emit tick(_data.currentTime);

		//Check for frame number
		if (rx_frame.indexIn(line) > -1) {
			int frame = rx_frame.cap(1).toInt();
			//qDebug() << __FUNCTION__ << "Frame number:" << frame;
			emit currentFrameNumberReceived(frame);
		}
	}

	//Parse other things
	else {
		qDebug() << "MPlayer:" << line;

		//Loading the file/stream/media
		//Becarefull! "Playing" does not mean the file is playing but only loading
		if (rx_playing.indexIn(line) > -1) {
			setState(LoadingState);
		}

		//Screenshot
		else if (rx_screenshot.indexIn(line) > -1) {
			const QString filename = rx_screenshot.cap(1);
			qDebug() << __FUNCTION__ << "Screenshot:" << filename;
			emit screenshotSaved(filename);
		}

		//End of file
		else if (rx_endoffile.indexIn(line) > -1) {
			qDebug() << __FUNCTION__ << "End of line detected";

			//In case of playing VCDs or DVDs, maybe the first title
			//is not playable, so the GUI doesn't get the info about
			//available titles. So if we received the end of file
			//first let's pretend the file has started so the GUI can have
			//the data.
			if (_state != PlayingState) {
				setState(PlayingState);
			}

			//Sends the stateChanged(EndOfFileState) signal once the process is finished, not now!
			_endOfFileReached = true;
		}

		//Window resolution
		else if (rx_winresolution.indexIn(line) > -1) {
			int width = rx_winresolution.cap(4).toInt();
			int height = rx_winresolution.cap(5).toInt();

			qDebug() << __FUNCTION__ << "Video driver:" << rx_winresolution.cap(1);

			emit videoWidgetSizeChanged(width, height);
			//emit mplayerFullyLoaded();
		}

		//No video
		else if (rx_novideo.indexIn(line) > -1) {
			_data.hasVideo = false;
			qDebug() << __FUNCTION__ << "Video:" << _data.hasVideo;
			emit hasVideoChanged(_data.hasVideo);
			//emit mplayerFullyLoaded();
		}

		//Pause
		else if (rx_paused.indexIn(line) > -1) {
			setState(PausedState);
		}

		//Stream title
		if (rx_stream_title.indexIn(line) > -1) {
			const QString title = rx_stream_title.cap(1);
			const QString url = rx_stream_title.cap(2);
			qDebug() << __FUNCTION__ << "Stream title:" << title;
			qDebug() << __FUNCTION__ << "Stream url:" << url;
			_data.stream_title = title;
			_data.stream_url = url;
			//emit streamTitleAndUrl(title, url);
		}

		//The following things are not sent when the file has started to play
		//(or if sent, GUI will ignore anyway...)
		//So not process anymore, if video is playing to save some time
		if (_state == PlayingState) {
			return;
		}

		if ((_mplayerSvnRevision == -1) && (line.startsWith("MPlayer "))) {
			_mplayerSvnRevision = MPlayerVersion::parse(line);
			if (_mplayerSvnRevision <= 0) {
				emit failedToParseMplayerVersion(line);
			}
		}

		//Subtitles
		/*if (rx_subtitle.indexIn(line) > -1) {
			_data.subs.process(line);
		}
		else
		if (rx_sid.indexIn(line) > -1) {
			_data.subs.process(line);
		}
		else
		if (rx_subtitle_file.indexIn(line) > -1) {
			_data.subs.process(line);
		}*/

		//AO
		if (rx_ao.indexIn(line) > -1) {
			//emit receivedAO(rx_ao.cap(1));
		}

		//DVD and Matroska audio
		else if (rx_audio_mat.indexIn(line) > -1) {
			int id = rx_audio_mat.cap(1).toInt();
			QString lang = rx_audio_mat.cap(3);
			QString type = rx_audio_mat.cap(2);
			qDebug() << __FUNCTION__ << "Audio:" << id << "lang:" << lang << "type:" << type;

			if (type == "NAME") {
				//_data.audios.addName(id, lang);
			} else {
				//_data.audios.addLang(id, lang);
			}
		}

		//Matroshka chapters
		/*if (rx_mkvchapters.indexIn(line)!=-1) {
			int c = rx_mkvchapters.cap(1).toInt();
			qDebug("MPlayerProcess::parseLine: mkv chapters: %d", c);
			if ((c+1) > _data.mkv_chapters) {
				_data.mkv_chapters = c+1;
				qDebug("MPlayerProcess::parseLine: mkv_chapters set to: %d", _data.mkv_chapters);
			}
		}
		else

		//VCD titles
		if (rx_vcd.indexIn(line) > -1) {
			int ID = rx_vcd.cap(1).toInt();
			QString length = rx_vcd.cap(2);
			//_data.titles.addID(ID);
			_data.titles.addName(ID, length);
		}
		else

		//Audio CD titles
		if (rx_cdda.indexIn(line) > -1) {
			int ID = rx_cdda.cap(1).toInt();
			QString length = rx_cdda.cap(2);
			double duration = 0;
			QRegExp r("(\\d+):(\\d+):(\\d+)");
			if (r.indexIn(length) > -1) {
				duration = r.cap(1).toInt() * 60;
				duration += r.cap(2).toInt();
			}
			_data.titles.addID(ID);
			//QString name = QString::number(ID) + " (" + length + ")";
			//_data.titles.addName(ID, name);
			_data.titles.addDuration(ID, duration);
		}
		else*/

		//DVD titles
		else if (rx_title.indexIn(line) > -1) {
			int id = rx_title.cap(1).toInt();
			const QString title = rx_title.cap(2);

			if (title == "LENGTH") {
				double length = rx_title.cap(3).toDouble();
				qDebug() << __FUNCTION__ << "DVD:" << "title:" << id << "length:" << length;
				//_data.titles.addDuration(id, length);
			}

			else if (title == "CHAPTERS") {
				int chapters = rx_title.cap(3).toInt();
				qDebug() << __FUNCTION__ << "DVD:" << "title:" << id << "chapters:" << chapters;
				//_data.titles.addChapters(ID, chapters);
			}

			else if (title == "ANGLES") {
				int angles = rx_title.cap(3).toInt();
				qDebug() << __FUNCTION__ << "DVD:" << "title:" << id << "angles:" << angles;
				//_data.titles.addAngles(ID, angles);
			}
		}

		//Catch cache messages
		else if (rx_cache.indexIn(line) > -1) {
			emit receivedCacheMessage(line);
		}

		//Creating index
		else if (rx_create_index.indexIn(line) > -1) {
			emit receivedCreatingIndex(line);
		}

		//Catch connecting message
		else if (rx_connecting.indexIn(line) > -1) {
			qDebug() << __FUNCTION__ << "Connecting message:" << line;
			emit connectingMessageReceived(line);
			setState(BufferingState);
		}

		//Catch resolving message
		else if (rx_resolving.indexIn(line) > -1) {
			qDebug() << __FUNCTION__ << "Resolving message:" << line;
			emit resolvingMessageReceived(line);
		}

		//Clip info

		//Name
		else if (rx_clip_name.indexIn(line) > -1) {
			const QString s = rx_clip_name.cap(2);
			qDebug() << __FUNCTION__ << "Clip name:" << s;
			_data.clip_name = s;
		}

		//Artist
		else if (rx_clip_artist.indexIn(line) > -1) {
			const QString s = rx_clip_artist.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_artist: '%s'", s.toUtf8().data());
			_data.clip_artist = s;
		}

		//Author
		else if (rx_clip_author.indexIn(line) > -1) {
			const QString s = rx_clip_author.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_author: '%s'", s.toUtf8().data());
			_data.clip_author = s;
		}

		//Album
		else if (rx_clip_album.indexIn(line) > -1) {
			const QString s = rx_clip_album.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_album: '%s'", s.toUtf8().data());
			_data.clip_album = s;
		}

		//Genre
		else if (rx_clip_genre.indexIn(line) > -1) {
			const QString s = rx_clip_genre.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_genre: '%s'", s.toUtf8().data());
			_data.clip_genre = s;
		}

		//Date
		else if (rx_clip_date.indexIn(line) > -1) {
			const QString s = rx_clip_date.cap(2);
			qDebug("MPlayerProcess::parseLine: clip_date: '%s'", s.toUtf8().data());
			_data.clip_date = s;
		}

		//Track
		else if (rx_clip_track.indexIn(line) > -1) {
			const QString s = rx_clip_track.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_track: '%s'", s.toUtf8().data());
			_data.clip_track = s;
		}

		//Copyright
		else if (rx_clip_copyright.indexIn(line) > -1) {
			const QString s = rx_clip_copyright.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_copyright: '%s'", s.toUtf8().data());
			_data.clip_copyright = s;
		}

		//Comment
		else if (rx_clip_comment.indexIn(line) > -1) {
			const QString s = rx_clip_comment.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_comment: '%s'", s.toUtf8().data());
			_data.clip_comment = s;
		}

		//Software
		else if (rx_clip_software.indexIn(line) > -1) {
			const QString s = rx_clip_software.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_software: '%s'", s.toUtf8().data());
			_data.clip_software = s;
		}

		//Catch "Starting playback..." message
		else if (rx_play.indexIn(line) > -1) {
			//OK, now all the media datas should be in clean state
			emit mediaDataChanged();

			setState(PlayingState);
		}

		//Generic things
		if (rx.indexIn(line) > -1) {
			const QString tag = rx.cap(1);
			const QString value = rx.cap(2);

			if (tag == "ID_VIDEO_ID") {
				//First string to tell us that the media contains a video track
				_data.hasVideo = true;
				qDebug() << __FUNCTION__ << "Video:" << _data.hasVideo;
				emit hasVideoChanged(_data.hasVideo);
			}

			else if (tag == "ID_AUDIO_ID") {
				//First string to tell us that the media contains an audio track
				/*int ID = value.toInt();
				qDebug("MplayerProcess::parseLine: ID_AUDIO_ID: %d", ID);
				md.audios.addID( ID );*/
			}

			else if (tag == "ID_LENGTH") {
				_data.totalTime = value.toDouble() * SECONDS_CONVERTION;
				qDebug() << __FUNCTION__ << "Media total time:" << _data.totalTime;
				emit totalTimeChanged(_data.totalTime);
			}

			else if (tag == "ID_SEEKABLE") {
				_data.isSeekable = value.toInt();
				qDebug() << __FUNCTION__ << "Media seekable:" << _data.isSeekable;
				emit seekableChanged(_data.isSeekable);
			}

			else if (tag == "ID_VIDEO_WIDTH") {
				_data.videoWidth = value.toInt();
				qDebug() << __FUNCTION__ << "Video width:" << _data.videoWidth;
			}

			else if (tag == "ID_VIDEO_HEIGHT") {
				_data.videoHeight = value.toInt();
				qDebug() << __FUNCTION__ << "Video height:" << _data.videoHeight;
			}

			else if (tag == "ID_VIDEO_ASPECT") {
				_data.videoAspectRatio = value.toDouble();
				if (_data.videoAspectRatio == 0.0) {
					//I hope width & height are already set
					_data.videoAspectRatio = (double) _data.videoWidth / _data.videoHeight;
				}
				qDebug() << __FUNCTION__ << "Video aspect:" << _data.videoAspectRatio;
			}

			else if (tag == "ID_DVD_DISC_ID") {
				//_data.dvd_id = value;
				qDebug() << __FUNCTION__ << "DVD disc Id:" << value;
			}

			else if (tag == "ID_DEMUXER") {
				_data.demuxer = value;
			}

			else if (tag == "ID_VIDEO_FORMAT") {
				_data.videoFormat = value;
			}

			else if (tag == "ID_AUDIO_FORMAT") {
				_data.audioFormat = value;
			}

			else if (tag == "ID_VIDEO_BITRATE") {
				_data.videoBitrate = value.toInt();
			}

			else if (tag == "ID_VIDEO_FPS") {
				_data.videoFPS = value.toDouble();
				qDebug() << __FUNCTION__ << "Video FPS:" << _data.videoFPS;
			}

			else if (tag == "ID_AUDIO_BITRATE") {
				_data.audioBitrate = value.toInt();
			}

			else if (tag == "ID_AUDIO_RATE") {
				_data.audioRate = value.toInt();
			}

			else if (tag == "ID_AUDIO_NCH") {
				_data.audioNbChannels = value.toInt();
			}

			else if (tag == "ID_VIDEO_CODEC") {
				_data.videoCodec = value;
			}

			else if (tag == "ID_AUDIO_CODEC") {
				_data.audioCodec = value;
			}
		}
	}
}

void MPlayerProcess::finished(int, QProcess::ExitStatus) {
	//Send the finished() signal before the EndOfFileState one, otherwise
	//the playlist will start to play next file before all
	//objects are notified that the process has exited
	if (_endOfFileReached) {
		setState(EndOfFileState);
	}
}

void MPlayerProcess::setState(State state) {
	_state = state;
	emit stateChanged(state);
}