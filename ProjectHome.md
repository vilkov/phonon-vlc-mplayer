# News #

### 2011/01/14 ###
Both VLC and MPlayer backends are now hosted on https://projects.kde.org/projects/kdesupport/phonon

### 2010/04/09 ###
Both VLC and MPlayer backends are now hosted on gitorious: http://www.gitorious.org/phonon

### 2009/07/17 ###
The MPlayer backend has been moved to http://websvn.kde.org/trunk/playground/multimedia/phonon-backends/mplayer/
http://phonon-vlc-mplayer.googlecode.com is dead, long life to the MPlayer and VLC backends!

### 2009/07/12 ###
QuarkPlayer has been moved to http://quarkplayer.googlecode.com
The VLC backend is now hosted at http://websvn.kde.org/trunk/playground/multimedia/phonon-backends/vlc/

### 2008/11/28 ###
Lukas Durfina is now maintaining the VLC backend.
Here is the address for the source code: http://git.videolan.org/?p=vlc-lukas.git;a=tree;f=projects/phonon/vlc;hb=HEAD
The MPlayer backend is still hosted here. I have fixed a lot of bugs recently and now I can use it daily under Windows for my music and videos. Still MPlayer suffers from severe limitations :/ http://tkrotoff.blogspot.com/2008/11/mplayer-is-far-from-being-perfect.html

### 2008/05/29 ###
I applied for a KDE svn account (tkrotoff), so expect to see VLC and MPlayer backends on http://websvn.kde.org/trunk/kdesupport/phonon/ (or somewhere else) soon.
This repo will be kept for experimentation (QuarkPlayer) and documentation.

### 2008/05/16 ###
0.1.0 released!

Get the right package, install latest MPlayer & VLC-0.9 then run the examples.
I recommend mediaplayer in general and multiplevideoplayer for DVD/MKV chapters/titles/subtitles (MPlayer backend only).

For the Windows setup, you need to install the [Microsoft Visual C++ 2005 SP1 Redistributable Package](http://www.microsoft.com/downloads/details.aspx?familyid=200b2fd9-ae1a-4a14-984d-389c36f85647).

By default it is the MPlayer backend that is used, if you want to use the VLC backend, just delete phonon\_backend/phonon\_mplayer.(dll|so)

For the source package, do a svn checkout of [revision 152](https://code.google.com/p/phonon-vlc-mplayer/source/detail?r=152).

The Phonon library given in the package is a patched version with better titles/chapters handling. phonon-vlc-mplayer is of course compatible with latest Phonon SVN with a simple #define

Any comment are more than welcome!

### 2008/03/23 ###
phonon-mplayer starts to look very good, I've added subtiles/chapters/angles/titles/audio stream support.

### 2008/03/09 ###
Current status:
- libvlc is unfortunalty not fully tested and debugged.
- libsmplayer from the SMPlayer project is to much "SMPlayer" oriented and was not design to be a library.
So my current focus is the MPlayer backend and to create a small libmplayer.