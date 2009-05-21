"""
  QuarkPlayer, a Phonon media player
  Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import sys, os
import ftplib

def uploadFile(server, login, password, serverPath, fileToUpload):
	"""
	This method upload a file onto a FTP server in binary mode.
	"""

	print 'Upload {0} to ftp://{1}@{2}:{3}'.format(
		fileToUpload, login, server, serverPath)

	file = open(fileToUpload, 'rb')
	ftp = ftplib.FTP(server)
	ftp.login(login, password)
	ftp.storbinary('STOR ' + os.path.join(serverPath, os.path.basename(fileToUpload)), file)
	ftp.quit()

if __name__ == "__main__":
	file = open('login.txt', 'r')
	login = file.readline().split()[0]
	password = file.readline().split()[0]
	fileToUpload = sys.argv[1]
	uploadFile('192.168.0.12', login, password, '/var/www/snapshots/', fileToUpload)
	os.remove(fileToUpload)
