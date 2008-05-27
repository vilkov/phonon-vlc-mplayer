# - Use Module for QT4
# Sets up C and C++ to use Qt 4.  It is assumed that FindQt.cmake
# has already been loaded.  See FindQt.cmake for information on
# how to load Qt 4 into your CMake project.


ADD_DEFINITIONS(${QT_DEFINITIONS})
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG QT_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_DEBUG)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

SET(QT_LIBRARIES "")

IF (QT_USE_QTMAIN)
  IF (WIN32)
    SET(QT_LIBRARIES ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
  ENDIF (WIN32)
ENDIF (QT_USE_QTMAIN)

IF(QT_DONT_USE_QTGUI)
  SET(QT_USE_QTGUI 0)
ELSE(QT_DONT_USE_QTGUI)
  SET(QT_USE_QTGUI 1)
ENDIF(QT_DONT_USE_QTGUI)

IF(QT_DONT_USE_QTCORE)
  SET(QT_USE_QTCORE 0)
ELSE(QT_DONT_USE_QTCORE)
  SET(QT_USE_QTCORE 1)
ENDIF(QT_DONT_USE_QTCORE)

IF (QT_USE_QT3SUPPORT)
  ADD_DEFINITIONS(-DQT3_SUPPORT)
ENDIF (QT_USE_QT3SUPPORT)

# list dependent modules, so their modules are automatically on
SET(QT_QT3SUPPORT_MODULE_DEPENDS QTGUI QTSQL QTXML QTNETWORK QTCORE)
SET(QT_QTSVG_MODULE_DEPENDS QTGUI QTXML QTCORE)
SET(QT_QTUITOOLS_MODULE_DEPENDS QTGUI QTXML QTCORE)
SET(QT_QTHELP_MODULE_DEPENDS QTGUI QTSQL QTXML QTCORE)
SET(QT_PHONON_MODULE_DEPENDS QTGUI QTDBUS QTCORE)
SET(QT_QTDBUS_MODULE_DEPENDS QTXML QTCORE)
SET(QT_QTXMLPATTERNS_MODULE_DEPENDS QTNETWORK QTCORE)

# Qt modules  (in order of dependence)
FOREACH(module QT3SUPPORT QTOPENGL QTASSISTANT QTDESIGNER QTMOTIF QTNSPLUGIN
               QTSCRIPT QTSVG QTUITOOLS QTHELP QTWEBKIT PHONON QTGUI QTTEST 
               QTDBUS QTXML QTSQL QTXMLPATTERNS QTNETWORK QTCORE)

  IF (QT_USE_${module})
    IF (QT_${module}_FOUND)
      IF(QT_USE_${module})
        STRING(REPLACE "QT" "" qt_module_def "${module}")
        ADD_DEFINITIONS(-DQT_${qt_module_def}_LIB)
        INCLUDE_DIRECTORIES(${QT_${module}_INCLUDE_DIR})
      ENDIF(QT_USE_${module})
      SET(QT_LIBRARIES ${QT_LIBRARIES} ${QT_${module}_LIBRARY} ${QT_${module}_LIB_DEPENDENCIES})
      FOREACH(depend_module ${QT_${module}_MODULE_DEPENDS})
        SET(QT_USE_${depend_module} 1)
      ENDFOREACH(depend_module ${QT_${module}_MODULE_DEPENDS})
    ELSE (QT_${module}_FOUND)
      MESSAGE("Qt ${module} library not found.")
    ENDIF (QT_${module}_FOUND)
  ENDIF (QT_USE_${module})
  
ENDFOREACH(module)

