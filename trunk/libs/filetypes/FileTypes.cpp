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

#include "FileTypes.h"

#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QDebug>

static const int FILETYPELIST_SIZE = 52;

//See http://en.wikipedia.org/wiki/List_of_file_formats
static const FileType FILETYPELIST[FILETYPELIST_SIZE] = {
	//Video
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Video
	{ FileType::Video, FileType::_3GP, "3GP", "3gp,3g2", "video/3gpp,audio/3gpp" },
	{ FileType::Video, FileType::ASF, "Advanced Systems Format (ASF)", "asf", "video/x-ms-asf,application/vnd.ms-asf" },
	{ FileType::Video, FileType::AVI, "Audio Video Interleave (AVI)", "avi", "video/avi,video/msvideo,video/x-msvideo" },
	{ FileType::Video, FileType::MPEG1, "MPEG-1", "mpg,mpeg,mp1,m1v,m1a,m2a,mpa,mpv", "audio/mpeg,video/mpeg" },
	{ FileType::Video, FileType::FLV, "Flash Video (FLV)", "flv,f4v,f4p,f4a,f4b", "video/x-flv,video/mp4,video/x-m4v,audio/mp4a-latm,video/3gpp,video/quicktime,audio/mp4" },
	{ FileType::Video, FileType::SWF, "Adobe Flash (SWF)", "swf", "application/x-shockwave-flash" },
	{ FileType::Video, FileType::MKV, "Matroska (MKV)", "mkv,mka,mks", "video/x-matroska,audio/x-matroska" },
	{ FileType::Video, FileType::MOV, "QuickTime Movie (MOV)", "mov,qt", "video/quicktime" },
	{ FileType::Video, FileType::MP4, "MPEG-4 Part 14 (MP4)", "mp4", "video/mp4" },
	{ FileType::Video, FileType::NSV, "Nullsoft Streaming Video (NSV)", "nsv", "" },
	{ FileType::Video, FileType::Theora, "Theora", "ogv", "video/ogg,video/x-ogg" },
	{ FileType::Video, FileType::WMV, "Windows Media Video (WMV)", "wmv", "video/x-ms-wmv" },
	{ FileType::Video, FileType::DivX, "DivX Media Format (DMF)", "divx", "video/divx" },
	{ FileType::Video, FileType::NUT, "NUT", "nut", "" },
	{ FileType::Video, FileType::OGM, "Ogg Media (OGM)", "ogm", "" },

	//Audio
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Sound_and_music

	//Lossless audio
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Lossless_audio
	{ FileType::Audio, FileType::AIFF, "Audio Interchange File Format (AIFF)", "aiff,aif,aifc", "audio/x-aiff,audio/aiff" },
	{ FileType::Audio, FileType::AU, "Au", "au,snd", "audio/basic" },
	{ FileType::Audio, FileType::WAV, "Waveform (WAV)", "wav", "audio/wav,audio/wave,audio/x-wav" },
	{ FileType::Audio, FileType::FLAC, "Free Lossless Audio Codec (FLAC)", "flac", "audio/x-flac,audio/x-oggflac" },
	{ FileType::Audio, FileType::AppleLossless, "Apple Lossless (M4A)", "m4a", "" },
	{ FileType::Audio, FileType::APE, "Monkey's Audio (APE)", "ape", "" },
	{ FileType::Audio, FileType::TTA, "True Audio (TTA)", "tta", "" },
	{ FileType::Audio, FileType::WavPack, "WavPack (WV)", "wv", "" },
	{ FileType::Audio, FileType::WMA, "Windows Media Audio (WMA)", "wma", "audio/x-ms-wma" },

	//Lossy audio
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Lossy_audio
	{ FileType::Audio, FileType::MP2, "MPEG-1 Audio Layer 2 (MP2)", "mp2", "audio/mpeg" },
	{ FileType::Audio, FileType::MP3, "MPEG-1 Audio Layer 3 (MP3)", "mp3", "audio/mpeg" },
	{ FileType::Audio, FileType::Speex, "Speex", "spx", "audio/speex" },
	{ FileType::Audio, FileType::Vorbis, "Vorbis", "ogg,oga", "audio/ogg,audio/x-ogg,audio/vorbis" },
	{ FileType::Audio, FileType::AAC, "Advanced Audio Coding (AAC)", "m4a,m4b,m4p,m4v,m4r,3gp,mp4,aac", "audio/aac,audio/aacp" },
	{ FileType::Audio, FileType::MPC, "Musepack (MPC)", "mpc,mp+,mpp", "audio/x-musepack,audio/musepack" },
	{ FileType::Audio, FileType::VQF, "TwinVQ (VQF)", "vqf", "" },
	{ FileType::Audio, FileType::RealMedia, "RealMedia", "ra,ram,rm,rv,rmvb", "audio/vnd.rn-realaudio,audio/x-pn-realaudio,audio/x-pn-realaudioplugin,audio/vnd.rn-realvideo" },
	{ FileType::Audio, FileType::MID, "MIDI File (SMF)", "mid,midi", "application/x-midi,audio/midi,audio/x-mid,audio/x-midi" },
	{ FileType::Audio, FileType::MOD, "MOD", "mod", "audio/mod,audio/x-mod" },

	//Playlist
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Playlist_formats
	{ FileType::Playlist, FileType::ASX, "Advanced Stream Redirector (ASX)", "asx", "video/x-ms-asf" },
	{ FileType::Playlist, FileType::M3U, "M3U", "m3u", "audio/x-mpegurl" },
	{ FileType::Playlist, FileType::PLS, "PLS", "pls", "audio/x-scpls" },
	{ FileType::Playlist, FileType::XSPF, "XML Shareable Playlist Format (XSPF)", "xspf", "application/xspf+xml" },
	{ FileType::Playlist, FileType::WPL, "Windows Media Player Playlist (WPL)", "wpl", "application/vnd.ms-wpl" },

	//Subtitle
	//See http://en.wikipedia.org/wiki/Subtitle_(captioning)#Subtitle_formats
	{ FileType::Subtitle, FileType::AQTitle, "AQTitle (AQT)", "aqt", "" },
	{ FileType::Subtitle, FileType::MicroDVD, "MicroDVD", "sub", "" },
	{ FileType::Subtitle, FileType::MPsub, "MPsub", "sub", "" },
	{ FileType::Subtitle, FileType::SSA, "SubStation Alpha (SSA)", "ssa,ass", "" },
	{ FileType::Subtitle, FileType::SubViewer, "SubViewer", "sub", "" },
	{ FileType::Subtitle, FileType::RealText, "RealText", "rt", "" },
	{ FileType::Subtitle, FileType::SAMI, "Synchronized Accessible Media Interchange (SAMI)", "smi,sami", "" },
	{ FileType::Subtitle, FileType::SSF, "Structured Subtitle Format (SSF)", "ssf", "" },
	{ FileType::Subtitle, FileType::SubRip, "SubRip", "srt", "" },
	{ FileType::Subtitle, FileType::USF, "Universal Subtitle Format (USF)", "usf", "" },
	{ FileType::Subtitle, FileType::VOBsub, "VOBsub", "sub,idx", "" },
	{ FileType::Subtitle, FileType::NameUnknown, "", "utf", "" },
	{ FileType::Subtitle, FileType::NameUnknown, "", "txt", "" }

	//{ "", "", "", "" },
};


QStringList FileTypes::extensions(FileType::Category category) {
	static QMap<FileType::Category, QStringList> map;

	if (!map.contains(category)) {
		//We use QSet then there is no duplicated element
		QSet<QString> set;
		for (int i = 0; i < FILETYPELIST_SIZE; i++) {
			FileType fileType = FILETYPELIST[i];
			if (fileType.category == category) {
				set += QSet<QString>::fromList(fileType.extensions.split(",", QString::SkipEmptyParts));
			}
		}
		map[category] = QStringList::fromSet(set);
	}

	return map.value(category);
}

QStringList FileTypes::extensions(FileType::Category category1, FileType::Category category2) {
	int superCategory = (int) category1 + (int) category2;
	static QMap<int, QStringList> map;

	if (!map.contains(superCategory)) {
		//We use QSet then there is no duplicated element
		QSet<QString> set;
		set += QSet<QString>::fromList(extensions(category1));
		set += QSet<QString>::fromList(extensions(category2));
		map[superCategory] = QStringList::fromSet(set);
	}

	return map.value(superCategory);
}

FileType FileTypes::fileType(const QString & extension) {
	static QMap<QString, FileType> map;

	if (!map.contains(extension)) {
		for (int i = 0; i < FILETYPELIST_SIZE; i++) {
			FileType fileType = FILETYPELIST[i];
			if (fileType.extensions.contains(extension)) {
				//The first one that we found
				map[extension] = fileType;
				break;
			}
		}

		//The extension string was not found :/
		if (!map.contains(extension)) {
			FileType unknownFileType;
			unknownFileType.category = FileType::CategoryUnknown;
			unknownFileType.name = FileType::NameUnknown;
			unknownFileType.fullName = extension;
			unknownFileType.extensions += extension;
			map[extension] = unknownFileType;
		}
	}

	return map.value(extension);
}

FileType FileTypes::fileType(FileType::Name name) {
	static QMap<FileType::Name, FileType> map;

	if (!map.contains(name)) {
		for (int i = 0; i < FILETYPELIST_SIZE; i++) {
			FileType fileType = FILETYPELIST[i];
			if (fileType.name == name) {
				map[name] = fileType;
				break;
			}
		}

		//The extension string was not found :/
		if (!map.contains(name)) {
			FileType unknownFileType;
			unknownFileType.category = FileType::CategoryUnknown;
			unknownFileType.name = FileType::NameUnknown;
			map[name] = unknownFileType;
		}
	}

	return map.value(name);
}

QString FileTypes::toFilterFormat(const QStringList & extensions) {
	QString tmp;
	foreach (QString ext, extensions) {
		tmp += "*." + ext + " ";
	}
	tmp.trimmed();
	tmp.prepend(" (");
	tmp.append(")");
	return tmp;
}