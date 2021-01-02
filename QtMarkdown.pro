QT += core gui
QT += webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    Document.cpp \
    main.cpp \
    mainwindow.cpp \
    previewpage.cpp \
    texthighlighter.cpp

HEADERS += \
    Document.h \
    mainwindow.h \
    previewpage.h \
    texthighlighter.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/markdowneditor.qrc

DISTFILES += \
    resources/default.md \
    resources/index.html
