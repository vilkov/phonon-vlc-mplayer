# - add_qt_translations(dir1 ... dirN)
# Adds directories which contain source files (*.cpp, *.h, *.cxx...) to a list of directories to be translated.
# Creates a global variable named SRCS_TRANSLATIONS
# - clear_qt_translations()
# Clears global variable named SRCS_TRANSLATIONS
#
# Example:
# clear_qt_translations()
# set(myapp_SRCS
#     toto.cpp
#     tata.cpp
# )
# add_qt_translations(${CMAKE_CURRENT_SOURCE_DIR})
# qt4_create_translation(myapp_QMS ${DIRS_TRANSLATIONS})
# add_executable(myapp ${myapp_SRCS} ${myapp_QMS})
#
# Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (clear_qt_translations)

	set(DIRS_TRANSLATIONS
		""
		CACHE INTERNAL "Directories which contain source files (*.cpp, *.h, *.cxx...) to be translated" FORCE
	)

endmacro (clear_qt_translations)

macro (add_qt_translations)

	set(_dirList "")
	foreach(dir ${ARGN})
		list(APPEND _dirList
			${dir}
		)
	endforeach(dir ${ARGN})

	set(DIRS_TRANSLATIONS
		${DIRS_TRANSLATIONS}
		${_dirList}
		CACHE INTERNAL "Directories which contain source files (*.cpp, *.h, *.cxx...) to be translated" FORCE
	)

endmacro (add_qt_translations)
