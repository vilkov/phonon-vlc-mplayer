# Detects system compiler and OS used
#
# Operating systems:
# - WIN32
# - UNIX
#   - CYGWIN
#   - APPLE
#   - LINUX
#
# - SYSTEM_NAME Operating system name as a string
#   (Win32, UNIX, Cygwin, MacOS, Linux...)
#
# Compilers:
# - MSVC (Microsoft Visual C++)
#   - MSVC60 (Visual C++ 6.0)             MSVC_VERSION=1200
#   - MSVC60 (Visual C++ 6.0 SP5)         MSVC_VERSION=1200
#   - MSVC60 (Visual C++ 6.0 SP6)         MSVC_VERSION=1200
#   - MSVC70 (Visual C++ .NET 2002)       MSVC_VERSION=1300
#   - MSVC70 (Visual C++ .NET 2002 SP1)   MSVC_VERSION=1300
#   - MSVC71 (Visual C++ .NET 2003)       MSVC_VERSION=1310 (Compiler Version 13.10.3077)
#   - MSVC71 (Visual C++ .NET 2003 SP1)   MSVC_VERSION=1310 (Compiler Version 13.10.6030)
#   - MSVC80 (Visual C++ 2005)            MSVC_VERSION=1400
#   - MSVC80 (Visual C++ 2005 SP1)        MSVC_VERSION=1400 (Compiler Version 14.00.50727.762)
#   - MSVC90 (Visual C++ 2008)            MSVC_VERSION=1500 (Compiler Version 15.00.21022.08)
#   - MSVC90 (Visual C++ 2008 SP1)        MSVC_VERSION=1500 (Compiler Version 15.00.30729.01)
# - GCC (GNU GCC)
#   - MINGW (Native GCC under Windows)
#   - GCC3 (GNU GCC 3.x)
#   - GCC4 (GNU GCC 4.x)
#     - GCC40 (GNU GCC 4.0.x)
#     - GCC41 (GNU GCC 4.1.x)
#     - GCC42 (GNU GCC 4.2.x)
#     - GCC43 (GNU GCC 4.3.x)
#     - GCC44 (GNU GCC 4.4.x)
# - BORLAND (Borland C++)
# - WATCOM (Watcom C/C++ Compiler)
# - ICC (Intel C++ Compiler - not detected yet)
#
# - COMPILER_NAME compiler name as a string
#   (MSVC71, MinGW, GCC42, Borland, Watcom...)
#
# Copyright (C) 2007  Wengo
# Coptright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


# GCC
set(GCC ${CMAKE_COMPILER_IS_GNUCC})
ow_get_gcc_version(version)
if (version MATCHES "3.*")
	set(GCC3 true)
	set(COMPILER_NAME "GCC3")
endif (version MATCHES "3.*")
if (version MATCHES "4.*")
	set(GCC4 true)
endif (version MATCHES "4.*")
if (version MATCHES "4\\.0.*")
	set(GCC40 true)
	set(COMPILER_NAME "GCC40")
endif (version MATCHES "4\\.0.*")
if (version MATCHES "4\\.1.*")
	set(GCC41 true)
	set(COMPILER_NAME "GCC41")
endif (version MATCHES "4\\.1.*")
if (version MATCHES "4\\.2.*")
	set(GCC42 true)
	set(COMPILER_NAME "GCC42")
endif (version MATCHES "4\\.2.*")
if (version MATCHES "4\\.3.*")
	set(GCC43 true)
	set(COMPILER_NAME "GCC43")
endif (version MATCHES "4\\.3.*")
if (version MATCHES "4\\.4.*")
	set(GCC44 true)
	set(COMPILER_NAME "GCC44")
endif (version MATCHES "4\\.4.*")

# MinGW
if (MINGW)
	set(COMPILER_NAME "MinGW")
endif (MINGW)

# MSVC
if (MSVC60)
	set(COMPILER_NAME "MSVC60")
endif (MSVC60)
if (MSVC70)
	set(COMPILER_NAME "MSVC70")
endif (MSVC70)
if (MSVC71)
	set(COMPILER_NAME "MSVC71")
endif (MSVC71)
if (MSVC80)
	set(COMPILER_NAME "MSVC80")
endif (MSVC80)
if (MSVC90)
	set(COMPILER_NAME "MSVC90")
endif (MSVC90)

# Borland
if (BORLAND)
	set(COMPILER_NAME "Borland")
endif (BORLAND)

# Watcom
if (WATCOM)
	set(COMPILER_NAME "Watcom")
endif (WATCOM)

if (NOT COMPILER_NAME)
	# Default value
	set(COMPILER_NAME "CC")
endif (NOT COMPILER_NAME)

# LINUX
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(LINUX true)
	set(APPLE false)
	set(WIN32 false)
	set(SYSTEM_NAME "Linux")
endif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

if (APPLE)
	set(LINUX false)
	set(WIN32 false)
	# "MacOS" name is more commom than "Apple" name
	set(SYSTEM_NAME "MacOS")
endif (APPLE)

if (WIN32)
	set(APPLE false)
	set(LINUX false)
	if (CMAKE_CL_64)
		# Using the 64 bit compiler from Microsoft
		set(WIN64 true)
		set(SYSTEM_NAME "Win64")
	else (CMAKE_CL_64)
		set(SYSTEM_NAME "Win32")
	endif (CMAKE_CL_64)
endif (WIN32)

if (CYGWIN)
	set(SYSTEM_NAME "Cygwin")
endif (CYGWIN)

if (NOT SYSTEM_NAME)
	# Default value
	set(SYSTEM_NAME "UNIX")
endif (NOT SYSTEM_NAME)
