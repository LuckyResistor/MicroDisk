#
# Lucky Resistor Micro Disk
# --------------------------------------------------------------------------
# (c)2016 by Lucky Resistor
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# Always compress solid with LZMA
SetCompressor /SOLID lzma

# Location of the config.h file.
!define CONFIG_H "config.h"

# Read application informations from config.h
!searchparse /file ${CONFIG_H} `#define APPLICATION_VERSION "` APP_VERSION `"`
!searchparse /file ${CONFIG_H} `#define APPLICATION_NAME "` APP_NAME `"`
!searchparse /file ${CONFIG_H} `#define APPLICATION_COMPANY_ID "` COMPANY_ID `"`
!searchparse /file ${CONFIG_H} `#define APPLICATION_COMPANY_NAME "` COMPANY_TITLE `"`
!searchparse /file ${CONFIG_H} `#define APPLICATION_COPYRIGHT "` APP_COPYRIGHT `"`
!searchparse /file ${CONFIG_H} `#define APPLICATION_COMPANY_URL "` COMPANY_URL `"`
!searchparse /file ${CONFIG_H} `#define APPLICATION_ID "` APP_ID `"`

# Compose additional fields
!define TITLE "${COMPANY_TITLE} ${APP_NAME}"
!define REGKEY "SOFTWARE\${COMPANY_ID}\${APP_ID}"
!define UNINSTALLKEY "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_ID}"

Name "${APP_NAME} V${APP_VERSION}"

# MUI defines
!define MUI_ICON "LuckyResistor.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNICON "LuckyResistor.ico"
#!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN "$INSTDIR\${APP_ID}.exe"

# Included files
!include Sections.nsh
!include MUI2.nsh
!include FileFunc.nsh
!include LogicLib.nsh

# Used macros
!insertmacro GetParameters
!insertmacro GetOptions

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Request admin execute level for this installer.
RequestExecutionLevel admin

# Installer attributes
OutFile "${COMPANY_ID} ${APP_NAME} ${APP_VERSION} Setup.exe"

InstallDir $PROGRAMFILES\${COMPANY_ID}\${APP_ID}

CRCCheck on
XPStyle on
ShowInstDetails hide
ShowUninstDetails hide
VIProductVersion ${APP_VERSION}.0
VIAddVersionKey ProductName "${TITLE}"
VIAddVersionKey ProductVersion "${APP_VERSION}"
VIAddVersionKey CompanyName "${COMPANY_TITLE}"
VIAddVersionKey CompanyWebsite "${COMPANY_URL}"
VIAddVersionKey FileVersion "${APP_VERSION}"
VIAddVersionKey FileDescription "Installer for ${TITLE}"
VIAddVersionKey LegalCopyright "${APP_COPYRIGHT}"
VIAddVersionKey Comments "Setup built at ${__TIMESTAMP__}"
InstallDirRegKey HKLM "${REGKEY}" Path

# Installer sections
# ---------------------------------------------------------------------------

Section -Main SEC_MAIN
	SetOutPath "$INSTDIR"
	SetOverwrite on

        File "Setup\*.*"
        SetOutPath "$INSTDIR\platforms"
        File "Setup\platforms\*.*"
SectionEnd

Section "Create Shortcut" SEC_SHORTCUT
        SetShellVarContext all
        CreateDirectory "$SMPROGRAMS\${COMPANY_ID}"
        CreateShortCut "$SMPROGRAMS\${COMPANY_ID}\${APP_NAME}.lnk" "$INSTDIR\${APP_ID}.exe"
SectionEnd

Section -post SEC_LAST
	WriteRegStr HKLM "${REGKEY}" Path "$INSTDIR"
	WriteRegStr HKLM "${REGKEY}" Version "${APP_VERSION}"
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "${UNINSTALLKEY}" DisplayName "${COMPANY_TITLE} - $(^Name)"
	WriteRegStr HKLM "${UNINSTALLKEY}" DisplayVersion "${APP_VERSION}"
	WriteRegStr HKLM "${UNINSTALLKEY}" Publisher "${COMPANY_TITLE}"
	WriteRegStr HKLM "${UNINSTALLKEY}" URLInfoAbout "${COMPANY_URL}"
	WriteRegStr HKLM "${UNINSTALLKEY}" DisplayIcon "$INSTDIR\${APP_ID}.exe"
	WriteRegStr HKLM "${UNINSTALLKEY}" UninstallString "$INSTDIR\Uninstall.exe"
	WriteRegDWORD HKLM "${UNINSTALLKEY}" NoModify 1
	WriteRegDWORD HKLM "${UNINSTALLKEY}" NoRepair 1
SectionEnd

Section un.Main UNSEC_MAIN
        Delete /REBOOTOK "$INSTDIR\platforms\*.*"
        Delete /REBOOTOK "$INSTDIR\*.*"
        Delete /REBOOTOK "$SMPROGRAMS\${COMPANY_ID}\${APP_NAME}.lnk"
SectionEnd

Section un.post UNSEC_LAST
	DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_ID}"
	Delete /REBOOTOK "$INSTDIR\Uninstall.exe"
	DeleteRegValue HKLM "${REGKEY}" Path
	DeleteRegValue HKLM "${REGKEY}" Version
	DeleteRegKey HKLM "${REGKEY}"
	RMDir /r /REBOOTOK "$INSTDIR"
SectionEnd

# Installer functions
Function .onInit
	InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
	ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
FunctionEnd

# Language strings
LangString DESC_SHORTCUT ${LANG_ENGLISH} "Creates a shortcut in the start menu."

# Add a description for the module.
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
        !insertmacro MUI_DESCRIPTION_TEXT ${SEC_SHORTCUT} $(DESC_SHORTCUT)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

