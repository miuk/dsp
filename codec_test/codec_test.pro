######################################################################
# Automatically generated by qmake (3.0) ? 4? 2 22:04:57 2016
######################################################################

USE_G729 = TRUE

TEMPLATE = app
TARGET = codec_test
INCLUDEPATH += . .. /usr/local/include
LIBPATH += /usr/local/lib
LIBS += -lspeex -lopus

# Input
HEADERS += Player.hxx AudioSource.hxx SourceChooser.hxx SpeexOptionSetting.hxx NullCodec.hxx CodecWrapper.hxx CodecChooser.hxx OpusCodec.hxx OpusOptionSetting.hxx ULawCodec.hxx Slider.hxx PacketLossSetting.hxx PacketLossSimulator.hxx
SOURCES += main.cpp Player.cpp AudioSource.cpp SourceChooser.cpp SpeexOptionSetting.cpp SpeexCodec.cpp NullCodec.cpp CodecWrapper.cpp CodecChooser.cpp ../wav.cpp ../ulaw.c OpusCodec.cpp OpusOptionSetting.cpp ULawCodec.cpp Slider.cpp PacketLossSetting.cpp PacketLossSimulator.cpp
QT += widgets
QT += multimedia

equals(USE_G729,TRUE) {
INCLUDEPATH += ../g729a
LIBPATH += ../g729a
LIBS += -lg729a
HEADERS += G729aCodec.hxx
SOURCES += G729aCodec.cpp
DEFINES += USE_G729
}
