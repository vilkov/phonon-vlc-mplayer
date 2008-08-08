project(phonon-vlc)

include(ConfigureChecks.cmake)

set(VLC_FOUND FALSE)
find_package(VLC)
if (VLC_FOUND)
	include_directories(${VLC_INCLUDE_DIRS})
endif (VLC_FOUND)


#Uses Phonon from this repository instead
#CMake declarations order matters!!!
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_PHONON_VLC)
   set(phonon_vlc_SRCS
      AudioOutput.cpp
      Backend.cpp
      DeviceManager.cpp
      Effect.cpp
      EffectManager.cpp
      MediaController.cpp
      MediaObject.cpp
      SeekStack.cpp
      SinkNode.cpp
      VideoWidget.cpp
      vlc_loader.cpp
      vlc_symbols.cpp
      VLCMediaController.cpp
      VLCMediaObject.cpp
      VLCVideoWidget.cpp
      WidgetNoPaintEvent.cpp
      )

    automoc4(phonon_vlc phonon_vlc_SRCS)
    add_library(phonon_vlc SHARED ${phonon_vlc_SRCS})
    set_target_properties(phonon_vlc PROPERTIES DEFINE_SYMBOL MAKE_PHONON_VLC_LIB)
    target_link_libraries(phonon_vlc
      ${PHONON_LIBS}
      ${VLC_LIBRARIES}
    )

    install(TARGETS phonon_vlc DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
    install(FILES vlc.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
endif (BUILD_PHONON_VLC)

