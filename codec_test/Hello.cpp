#include "Player.hxx"
#include "AudioSource.hxx"
#include "SourceChooser.hxx"
#include "SpeexOptionSetting.hxx"
#include "SpeexCodec.hxx"
#include "OpusOptionSetting.hxx"
#include "OpusCodec.hxx"
#include "ULawCodec.hxx"
#include "CodecWrapper.hxx"
#include "CodecChooser.hxx"

#include <QApplication>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <QAudioOutput>
#include <QMediaPlayer>

#include <string>
#include <iostream>
using namespace std;

class MyApp : public QWidget {
    // SLOT を作る場合必須
    Q_OBJECT
public:
    MyApp(QApplication* app) { this->app = app; };
    virtual ~MyApp(void) {};
    void init(void);
private:

private:
    QApplication* app;
    QVBoxLayout* v;
    CodecWrapper* codec;
    ULawCodec* ulawCodec;
    SpeexCodec* spxCodec;
    OpusCodec* opsCodec;
};


void
MyApp::init(void)
{
    v = new QVBoxLayout();

    QGroupBox* gb = new QGroupBox("player");
    Player* player = new Player(NULL);
    gb->setLayout(player->getLayout());
    v->addWidget(gb);

    codec = new CodecWrapper();
    ulawCodec = new ULawCodec();
    codec->addCodec(ulawCodec);
    spxCodec = new SpeexCodec();
    codec->addCodec(spxCodec);
    opsCodec = new OpusCodec();
    codec->addCodec(opsCodec);
    
    gb = new QGroupBox("source");
    SourceChooser* chooser = new SourceChooser(this, codec);
    connect(chooser, SIGNAL(chooseSource(const QString&, const QAudioFormat&, QIODevice*))
            , player, SLOT(setSource(const QString&, const QAudioFormat&, QIODevice*)));
    gb->setLayout(chooser->getLayout());
    v->addWidget(gb);

    gb = new QGroupBox("codec");
    CodecChooser* cc = new CodecChooser(this, codec);
    gb->setLayout(cc->getLayout());
    v->addWidget(gb);
    cc->addCodecOptionSetter(NULL); // for null codec
    cc->addCodecOptionSetter(NULL); // for null ulaw

    gb = new QGroupBox("speex");
    SpeexOptionSetting* spx = new SpeexOptionSetting(this, spxCodec);
    gb->setLayout(spx->getLayout());
    gb->setVisible(false);
    v->addWidget(gb);
    cc->addCodecOptionSetter(gb);

    gb = new QGroupBox("opus");
    OpusOptionSetting* ops = new OpusOptionSetting(this, opsCodec);
    gb->setLayout(ops->getLayout());
    gb->setVisible(false);
    v->addWidget(gb);
    cc->addCodecOptionSetter(gb);
    connect(chooser, SIGNAL(chooseSource(const QString&, const QAudioFormat&, QIODevice*))
            , ops, SLOT(chooseSource(const QString&, const QAudioFormat&, QIODevice*)));

    setLayout(v);
}

int
main(int ac, char* av[])
{
    QApplication app(ac, av);
    MyApp* w = new MyApp(&app);
    w->init();
    w->show();
    
    return app.exec();
}

// SLOT を作る場合必須
#include "Hello.moc"
