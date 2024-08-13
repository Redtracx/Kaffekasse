; Installer-Abschnitt
Section "Hauptprogramm" SecMain
  SetOutPath "$INSTDIR"

  ; Kopieren der Icon-Datei
  File "appicon.ico"

  ; Kopieren der Projektdateien
  File "main.cpp"
  File "mainwindow.cpp"
  File "mainwindow.h"
  File "mainwindow.ui"
  File "Testproject.pro"
  File "Testproject.pro.user"

  ; Kopieren der Ordner
  SetOutPath "$INSTDIR\assets"
  File /r "assets\*.*"

  SetOutPath "$INSTDIR\build"
  File /r "build\*.*"

  SetOutPath "$INSTDIR"

  ; Erstellen von Verknüpfungen
  CreateDirectory "$SMPROGRAMS\Kaffeekasse"
  CreateShortCut "$SMPROGRAMS\Kaffeekasse\Kaffeekasse.lnk" "$INSTDIR\build\Testproject.exe" "" "$INSTDIR\appicon.ico"
  CreateShortCut "$DESKTOP\Kaffeekasse.lnk" "$INSTDIR\build\Testproject.exe" "" "$INSTDIR\appicon.ico"

  ; Schreiben der Uninstall-Informationen in die Registry
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kaffeekasse" "DisplayName" "Kaffeekasse"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kaffeekasse" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kaffeekasse" "DisplayIcon" "$INSTDIR\appicon.ico"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kaffeekasse" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kaffeekasse" "NoRepair" 1
SectionEnd

; Uninstaller-Abschnitt
Section "Uninstall"
  ; Entfernen aller installierten Dateien
  Delete "$INSTDIR\appicon.ico"
  Delete "$INSTDIR\main.cpp"
  Delete "$INSTDIR\mainwindow.cpp"
  Delete "$INSTDIR\mainwindow.h"
  Delete "$INSTDIR\mainwindow.ui"
  Delete "$INSTDIR\Testproject.pro"
  Delete "$INSTDIR\Testproject.pro.user"
  RMDir /r "$INSTDIR\assets"
  RMDir /r "$INSTDIR\build"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"

  ; Entfernen von Verknüpfungen
  Delete "$SMPROGRAMS\Kaffeekasse\Kaffeekasse.lnk"
  Delete "$DESKTOP\Kaffeekasse.lnk"
  RMDir "$SMPROGRAMS\Kaffeekasse"

  ; Entfernen der Registry-Einträge
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kaffeekasse"
SectionEnd
