#include "SpeexOptionSetting.hxx"
#include "Slider.hxx"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QCheckBox>

#include <iostream>
using namespace std;

SpeexOptionSetting::SpeexOptionSetting(QWidget* parent, SpeexCodec* codec)
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
    b = new QRadioButton("ABR");
    w->addWidget(b);
    bg->addButton(b, 1);
    b = new QRadioButton("VBR");
    w->addWidget(b);
    bg->addButton(b, 2);
    connect(bg, SIGNAL(buttonToggled(int,bool))
            , this, SLOT(onXBRToggled(int,bool)));
    layout->addRow(new QLabel("xBR"), w);

    w = new QHBoxLayout(parent);
    Slider* s = new Slider(parent, 0, 10, 5);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onQualityChanged(int)));
    s->addToLayout(w);
    layout->addRow(new QLabel("Quality"), w);

    w = new QHBoxLayout(parent);
    Slider* s2 = new Slider(parent, 0, 100, 2);
    connect(s2, SIGNAL(valueChanged(int)), this, SLOT(onPLCChanged(int)));
    s2->addToLayout(w);
    layout->addRow(new QLabel("PLC tuning"), w);

    w = new QHBoxLayout(parent);
    QCheckBox* cb = new QCheckBox("VAD");
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onVADChanged(int)));
    w->addWidget(cb);
    cb = new QCheckBox("DTX");
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onDTXChanged(int)));
    w->addWidget(cb);
    cb = new QCheckBox("ENH");
    cb->setCheckState(Qt::Checked);
    connect(cb, SIGNAL(stateChanged(int)), this, SLOT(onENHChanged(int)));
    w->addWidget(cb);
    layout->addRow(new QLabel("Options"), w);

    this->layout = layout;
}

SpeexOptionSetting::~SpeexOptionSetting(void)
{
}

void
SpeexOptionSetting::onXBRToggled(int id, bool state)
{
    cout << "onXBRToggled id=" << id << ", state=" << state << endl;
    if (state) {
        xBR = id;
        codec->setXBR(id);
    }
}

void
SpeexOptionSetting::onQualityChanged(int value)
{
    cout << "onQualityChanged value=" << value << endl;
    quality = value;
    codec->setQuality(value);
}

void
SpeexOptionSetting::onPLCChanged(int value)
{
    codec->setPLC(value);
}

void
SpeexOptionSetting::onVADChanged(int state)
{
    bool val = (state == 2);
    codec->setVAD(val);
}

void
SpeexOptionSetting::onDTXChanged(int state)
{
    bool val = (state == 2);
    codec->setDTX(val);
}

void
SpeexOptionSetting::onENHChanged(int state)
{
    bool val = (state == 2);
    codec->setENH(val);
}
