QT += core gui widgets charts

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mainwindow_kopie.cpp \
    mainwindow_working.cpp

HEADERS += \
    mainwindow.h

# Optional: Fügen Sie dies hinzu, wenn Sie UI-Dateien verwenden
FORMS += \
    mainwindow.ui

# Sicherstellen, dass die QtCharts-Bibliothek korrekt verlinkt wird
# (Stellen Sie sicher, dass dies für Ihre Qt-Version korrekt ist)
QT += charts

# Optional: Deployment rules for different platforms
# Uncomment and adjust paths if needed
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target
