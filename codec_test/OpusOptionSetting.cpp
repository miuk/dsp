#include "OpusOptionSetting.hxx"
#include "Slider.hxx"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QCheckBox>

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
    Slider* s = new Slider(parent, 1, 256, 12);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onKBpsChanged(int)));
    connect(this, SIGNAL(setKBpsRange(int,int)), s, SLOT(setRange(int,int)));
    s->addToLayout(w);
    layout->addRow(new QLabel("kbps"), w);

    w = new QHBoxLayout(parent);
    s = new Slider(parent, 0, 10, 5);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onComplexityChanged(int)));
    s->addToLayout(w);
    layout->addRow(new QLabel("Complexity"), w);

    w = new QHBoxLayout(parent);
    s = new Slider(parent, 8, 24, 24);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onLSBDepthChanged(int)));
    s->addToLayout(w);
    layout->addRow(new QLabel("LSB Depth"), w);

    w = new QHBoxLayout(parent);
    s = new Slider(parent, 0 ,100, 0);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onPLCChanged(int)));
    s->addToLayout(w);
    layout->addRow(new QLabel("PLC tuning"), w);

    w = new QHBoxLayout(parent);
    s = new Slider(parent, -32768, 32767, 0);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onGainChanged(int)));
    s->addToLayout(w);
    layout->addRow(new QLabel("Gain"), w);

    w = new QHBoxLayout(parent);
    QCheckBox* cb = new QCheckBox("DTX", parent);
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onDTXChanged(int)));
    w->addWidget(cb);
    cb = new QCheckBox("Inband FEC", parent);
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onInbandFECChanged(int)));
    w->addWidget(cb);
    cb = new QCheckBox("Prediction Disabled", parent);
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onPredictionDisabledChanged(int)));
    w->addWidget(cb);
    layout->addRow(new QLabel("Options"), w);

    // encoder
    //PREDICTION_DISABLED 0 1
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

void
OpusOptionSetting::onComplexityChanged(int value)
{
    codec->setComplexity(value);
}

void
OpusOptionSetting::onDTXChanged(int value)
{
    bool val = (value == 2);
    codec->setDTX(val);
}

void
OpusOptionSetting::onInbandFECChanged(int value)
{
    bool val = (value == 2);
    codec->setInbandFEC(val);
}

void
OpusOptionSetting::onLSBDepthChanged(int value)
{
    codec->setLSBDepth(value);
}

void
OpusOptionSetting::onPLCChanged(int value)
{
    codec->setPLC(value);
}

void
OpusOptionSetting::onPredictionDisabledChanged(int value)
{
    bool val = (value == 2);
    codec->setPredictionDisabled(val);
}

void
OpusOptionSetting::onGainChanged(int value)
{
    codec->setGain(value);
}
