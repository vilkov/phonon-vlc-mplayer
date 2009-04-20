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

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

class PlaylistModel;

/**
 * Parse command line arguments.
 *
 * @author Tanguy Krotoff
 */
class CommandLineParser {
public:

	CommandLineParser(PlaylistModel * playlistModel);

	~CommandLineParser();

private:

	void start();

	PlaylistModel * _playlistModel;
};

#endif	//COMMANDLINEPARSER_H