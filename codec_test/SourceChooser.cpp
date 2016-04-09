#include "SourceChooser.hxx"
#include "AudioSource.hxx"

#include "NullCodec.hxx"
#include "SpeexCodec.hxx"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>

#include <iostream>
using namespace std;

SourceChooser::SourceChooser(QWidget* parent, Codec* codec)
{
    this->parent = parent;
    this->codec = codec;
    layout = new QHBoxLayout(parent);
    QLabel* l = new QLabel(parent);
    connect(this, SIGNAL(setInfo(const QString&)), l, SLOT(setText(const QString&)));
    layout->addWidget(l);
    QPushButton* b = new QPushButton("Open", parent);
    connect(b, SIGNAL(clicked()), this, SLOT(open()));
    layout->addWidget(b);
}

SourceChooser::~SourceChooser(void)
{
}

void
SourceChooser::open()
{
    QString fname = QFileDialog::getOpenFileName(parent);
    if (fname.isEmpty())
        return;

    AudioSource* src = new AudioSource();
    src->setFileName(fname);
    src->setCodec(codec);
    if (src->open(QIODevice::ReadOnly) == false) {
        cout << "open failed" << endl;
        return;
    }

    QAudioFormat fmt;
    fmt.setSampleRate(src->getHz());
    fmt.setChannelCount(1);
    fmt.setSampleSize(16);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::SignedInt);

    int pos = fname.lastIndexOf("/");
    if (pos >= 0) {
        fname.remove(0, pos+1);
    }
    chooseSource(fname, fmt, src);
    cout << "file opened" << endl;

    char timestr[64];
    sprintf(timestr, "%02d:%02d"
            , src->getLengthSec()/60, src->getLengthSec()%60);
    QString info = fname;
    info.append(", ")
        .append(src->getTypeStr())
        .append(", ")
        .append(QString().setNum(src->getHz()/1000))
        .append("kHz")
        .append(", ")
        .append(timestr);
    setInfo(info);
}
