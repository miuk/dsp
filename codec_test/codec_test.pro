######################################################################
# Automatically generated by qmake (3.0) ? 4? 2 22:04:57 2016
######################################################################

TEMPLATE = app
TARGET = codec_test
INCLUDEPATH += . /usr/local/include
LIBPATH += /usr/local/lib
LIBS += -lspeex

# Input
HEADERS += Player.hxx AudioSource.hxx SourceChooser.hxx SpeexOptionSetting.hxx NullCodec.hxx CodecWrapper.hxx CodecChooser.hxx
SOURCES += Hello.cpp Player.cpp AudioSource.cpp SourceChooser.cpp SpeexOptionSetting.cpp SpeexCodec.cpp NullCodec.cpp CodecWrapper.cpp CodecChooser.cpp
QT += widgets
QT += multimedia
