; Casio CZ-3000 SysEx Librarian & Editor - NSIS Installer Script
Unicode True

!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "x64.nsh"

; ---------------------------------------------------------------------------
; Metadata
; ---------------------------------------------------------------------------
!define APP_NAME        "Casio CZ-3000 SysEx"
!define APP_EXE         "CZ3000SysEx.exe"
!define APP_VERSION     "1.0.0"
!define APP_PUBLISHER   "CZSYSEX"
!define APP_SM_FOLDER   "Casio CZ-3000 SysEx"
!define UNINSTALL_KEY   "Software\Microsoft\Windows\CurrentVersion\Uninstall\CZ3000SysEx"

Name "${APP_NAME} ${APP_VERSION}"
OutFile "..\dist\CZ3000SysEx_Setup_v${APP_VERSION}.exe"
InstallDir "$PROGRAMFILES64\${APP_NAME}"
InstallDirRegKey HKLM "Software\${APP_NAME}" "InstallDir"
RequestExecutionLevel admin
SetCompressor /SOLID lzma

; ---------------------------------------------------------------------------
; Modern UI Configuration
; ---------------------------------------------------------------------------
!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\${APP_EXE}"
!define MUI_FINISHPAGE_RUN_TEXT "Ejecutar ${APP_NAME} ahora"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Spanish"

; ---------------------------------------------------------------------------
; Version Info
; ---------------------------------------------------------------------------
VIProductVersion "${APP_VERSION}.0"
VIAddVersionKey /LANG=${LANG_SPANISH} "ProductName" "${APP_NAME}"
VIAddVersionKey /LANG=${LANG_SPANISH} "CompanyName" "${APP_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_SPANISH} "FileDescription" "Instalador de ${APP_NAME} (Editor y Gestor de Bancos SysEx)"
VIAddVersionKey /LANG=${LANG_SPANISH} "FileVersion" "${APP_VERSION}"
VIAddVersionKey /LANG=${LANG_SPANISH} "ProductVersion" "${APP_VERSION}"

; ---------------------------------------------------------------------------
; Sections
; ---------------------------------------------------------------------------
Section "Aplicación Principal (Requerido)" SecMain
    SectionIn RO

    SetOutPath "$INSTDIR"
    File "..\build\Release\${APP_EXE}"
    File "..\README.md"

    ; Save install path & write uninstaller
    WriteRegStr HKLM "Software\${APP_NAME}" "InstallDir" "$INSTDIR"
    WriteUninstaller "$INSTDIR\uninstall.exe"

    ; Add/Remove Programs metadata
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayName" "${APP_NAME}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayVersion" "${APP_VERSION}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "Publisher" "${APP_PUBLISHER}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "${UNINSTALL_KEY}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayIcon" "$INSTDIR\${APP_EXE}"
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoModify" 1
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoRepair" 1

    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "EstimatedSize" "$0"
SectionEnd

Section "Accesos directos en el Menú Inicio" SecStartMenu
    CreateDirectory "$SMPROGRAMS\${APP_SM_FOLDER}"
    CreateShortcut "$SMPROGRAMS\${APP_SM_FOLDER}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}" "" "$INSTDIR\${APP_EXE}" 0
    CreateShortcut "$SMPROGRAMS\${APP_SM_FOLDER}\Manual (README).lnk" "$INSTDIR\README.md"
    CreateShortcut "$SMPROGRAMS\${APP_SM_FOLDER}\Desinstalar ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Acceso directo en el Escritorio" SecDesktop
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}" "" "$INSTDIR\${APP_EXE}" 0
SectionEnd

; Section Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "Instala el ejecutable nativo de Casio CZ-3000 SysEx y la documentación."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} "Crea accesos directos en el Menú Inicio de Windows."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Crea un acceso directo en el Escritorio."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; ---------------------------------------------------------------------------
; Uninstaller
; ---------------------------------------------------------------------------
Section "Uninstall"
    Delete "$INSTDIR\${APP_EXE}"
    Delete "$INSTDIR\README.md"
    Delete "$INSTDIR\imgui.ini"
    Delete "$INSTDIR\uninstall.exe"
    RMDir "$INSTDIR"

    Delete "$DESKTOP\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_SM_FOLDER}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_SM_FOLDER}\Manual (README).lnk"
    Delete "$SMPROGRAMS\${APP_SM_FOLDER}\Desinstalar ${APP_NAME}.lnk"
    RMDir "$SMPROGRAMS\${APP_SM_FOLDER}"

    DeleteRegKey HKLM "${UNINSTALL_KEY}"
    DeleteRegKey HKLM "Software\${APP_NAME}"
SectionEnd

