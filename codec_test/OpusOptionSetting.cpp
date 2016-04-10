#include "OpusOptionSetting.hxx"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>

#include <iostream>
using namespace std;

OpusOptionSetting::OpusOptionSetting(QWidget* parent, OpusCodec* codec)
{
    this->parent = parent;
    this->codec = codec;
    QFormLayout* layout = new QFormLayout(parent);
    QButtonGroup* bg = new QButtonGroup();
    QHBoxLayout* w = new QHBoxLayout(parent);
    QRadioButton* b = new QRadioButton("CBR");
    b->setChecked(true);
    w->addWidget(b);
    bg->addButton(b, 0);
    b = new QRadioButton("VBR");
    w->addWidget(b);
    bg->addButton(b, 1);
    connect(bg, SIGNAL(buttonToggled(int,bool))
            , this, SLOT(onXBRToggled(int,bool)));
    layout->addRow(new QLabel("xBR"), w);

    w = new QHBoxLayout(parent);
    QLabel* l = new QLabel(parent);
    l->setFont(QFont("Courier"));
    w->addWidget(l);
    QSlider* s = new QSlider(Qt::Horizontal, parent);
    s->setMinimum(1);
    s->setMaximum(256);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onKBpsChanged(int)));
    connect(this, SIGNAL(setKBpsStr(const QString&)), l, SLOT(setText(const QString&)));
    connect(this, SIGNAL(setKBps(int)), s, SLOT(setValue(int)));
    connect(this, SIGNAL(setKBpsRange(int,int)), s, SLOT(setRange(int,int)));
    w->addWidget(s);
    setKBps(12);
    layout->addRow(new QLabel("kbps"), w);

    this->layout = layout;
}

OpusOptionSetting::~OpusOptionSetting(void)
{
}

void
OpusOptionSetting::onXBRToggled(int id, bool state)
{
    cout << "onXBRToggled id=" << id << ", state=" << state << endl;
    if (state) {
        xBR = id;
        codec->setXBR(id);
    }
}

void
OpusOptionSetting::onKBpsChanged(int value)
{
    //cout << "onKBpsChanged value=" << value << endl;
    kbps = value;
    codec->setBps(value*1000);
    char work[16];
    sprintf(work, "%3d", kbps);
    setKBpsStr(work);
}

void
OpusOptionSetting::chooseSource(const QString&, const QAudioFormat& fmt, QIODevice*)
{
    //cout << "OpusOptionSetting::chooseSource hz=" << fmt.sampleRate() << endl;
    int hz = fmt.sampleRate();
    switch (hz) {
    case 8000 :
        setKBpsRange(1, 128);
        break;
    case 16000 :
        setKBpsRange(1, 256);
        break;
    default :
        break;
    }
}
