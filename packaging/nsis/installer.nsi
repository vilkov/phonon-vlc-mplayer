;
; NSIS installer script for QuarkPlayer
;
; Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
;

; Global variables, already defined by CMake.
; If you add a global variable, declare it here as commented.
;!define BUILD_DIR "..\build\win32-msvc80-release\"
;!define PRODUCT_NAME "QuarkPlayer"
;!define PRODUCT_VERSION "0.2.5"
;!define INSTALLER_NAME "quarkplayer-0.2.4-rev629-win32-msvc90-minsizerel.exe"
;!define BINARY_NAME "quarkplayer.exe"
;!define PRODUCT_DESCRIPTION "QuarkPlayer, a Phonon media player"
;!define PRODUCT_PUBLISHER "QuarkPlayer Team"
;!define PRODUCT_URL "http://phonon-vlc-mplayer.googlecode.com/"
;!define PRODUCT_COPYRIGHT "Copyright (C) 2008-2009 Tanguy Krotoff"
;!define PRODUCT_LICENSE_FILE "../COPYING"
;!define COMPILER_NAME "MSVC80"
;!define BUILD_TYPE "Debug"

!define PRODUCT_REGKEY "Software\${PRODUCT_NAME}"
!define PRODUCT_UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

; Version information for the installer executable
VIAddVersionKey ProductName "${PRODUCT_NAME}"
VIAddVersionKey ProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey Comments "${PRODUCT_DESCRIPTION}"
VIAddVersionKey CompanyName "${PRODUCT_PUBLISHER}"
VIAddVersionKey LegalCopyright "${PRODUCT_COPYRIGHT}"
VIAddVersionKey FileDescription "${PRODUCT_NAME}"
VIAddVersionKey FileVersion "${PRODUCT_VERSION}"
VIAddVersionKey InternalName "${PRODUCT_NAME}"
VIAddVersionKey OriginalFilename "${INSTALLER_NAME}"
VIProductVersion "${PRODUCT_VERSION}.0"

SetCompressor /SOLID /FINAL lzma

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
BrandingText "$(^Name)"

; Installer name (e.g. quarkplayer-0.2.4-rev629-win32-msvc90-minsizerel.exe)
OutFile "${INSTALLER_NAME}"

InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"

; Gets installation folder from registry if available
InstallDirRegKey HKCU "${PRODUCT_REGKEY}" ""

ShowInstDetails show
ShowUnInstDetails show

; Request application privileges for Windows Vista
; See http://nsis.sourceforge.net/Shortcuts_removal_fails_on_Windows_Vista
; for more explanations
RequestExecutionLevel admin

; LogicLib makes NSIS scripts easier, provides a similar to other programming languages
!include "LogicLib.nsh"

; Modern UI
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC

; Language Selection Dialog Settings
; Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_REGKEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${PRODUCT_LICENSE_FILE}"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\${BINARY_NAME}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language files, first language is the default one
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

; Translations files
!include "translations\english.nsh"
!include "translations\french.nsh"

; Function includes, cannot be inside a Section
!include "IsMsvcrt80Installed.nsh"
!include "IsMsvcrt90Installed.nsh"
;!include "IsUserAdmin.nsh"



Function .onInit
	; Kills any running quarkplayer.exe
	;KillProcDLL::KillProc "${BINARY_NAME}"
	Sleep 100

	; Display a language selection dialog
	;!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section $(Name_SectionInstallFiles) SectionInstallFiles
	SectionIn RO
	SetOverwrite on
	!include "files_install.nsh"
SectionEnd

Section $(Name_SectionStartMenuShortcut) SectionStartMenuShortcut
	WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_URL}"
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${BINARY_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section $(Name_SectionDesktopIcon) SectionDesktopIcon
	CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${BINARY_NAME}"
SectionEnd

Section /o $(Name_SectionQuickLaunchIcon) SectionQuickLaunchIcon
	CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${PRODUCT_NAME}.lnk" "$INSTDIR\${BINARY_NAME}"
SectionEnd

Section $(Name_SectionSetAsDefaultProgram) SectionSetAsDefaultProgram
	ExecWait '"$INSTDIR\${BINARY_NAME}" --windows-install"'
SectionEnd

!macro deletePreferences
	ExecWait '"$INSTDIR\${BINARY_NAME}" --delete-preferences"'
!macroend

Section /o $(Name_SectionDeletePreferences) SectionDeletePreferences
	!insertmacro deletePreferences
SectionEnd

; Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionInstallFiles} $(Desc_SectionInstallFiles)
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionStartMenuShortcut} $(Desc_SectionStartMenuShortcut)
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionDesktopIcon} $(Desc_SectionDesktopIcon)
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionQuickLaunchIcon} $(Desc_SectionQuickLaunchIcon)
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionSetAsDefaultProgram} $(Desc_SectionSetAsDefaultProgram)
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionDeletePreferences} $(Desc_SectionDeletePreferences)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section -Post
	; Stores installation folder
	WriteRegStr HKCU "${PRODUCT_REGKEY}" "" $INSTDIR

	; Creates uninstaller
	WriteUninstaller "$INSTDIR\uninstall.exe"

	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayIcon" "$INSTDIR\${BINARY_NAME}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "URLInfoAbout" "${PRODUCT_URL}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd



Function un.onInit
	; Kills any running quarkplayer.exe
	;KillProcDLL::KillProc "${BINARY_NAME}"
	Sleep 100

	; Get the stored language preference
	!insertmacro MUI_UNGETLANGUAGE
FunctionEnd

Section un.$(Name_SectionUninstallFiles) SectionUninstallFiles
	SectionIn RO

	ExecWait '"$INSTDIR\${BINARY_NAME}" --windows-uninstall"'

	!include "files_uninstall.nsh"

	Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
	RMDir "$SMPROGRAMS\${PRODUCT_NAME}"

	Delete "$DESKTOP\${PRODUCT_NAME}.lnk"

	DeleteRegKey HKLM "${PRODUCT_UNINSTALL_KEY}"
	DeleteRegKey HKCU "${PRODUCT_REGKEY}"
SectionEnd

Section /o un.$(Name_SectionDeletePreferences) un.SectionDeletePreferences
	!insertmacro deletePreferences
SectionEnd

; Assign language strings to sections
!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${un.SectionDeletePreferences} $(Desc_SectionDeletePreferences)
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionUninstallFiles} $(Desc_SectionUninstallFiles)
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END
