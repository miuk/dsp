#include "SpeexOptionSetting.hxx"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>

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
    QLabel* l = new QLabel(parent);
    w->addWidget(l);
    QSlider* s = new QSlider(Qt::Horizontal, parent);
    s->setMinimum(0);
    s->setMaximum(10);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onQualityChanged(int)));
    connect(s, SIGNAL(valueChanged(int)), l, SLOT(setNum(int)));
    connect(this, SIGNAL(setQuality(int)), s, SLOT(setValue(int)));
    w->addWidget(s);
    setQuality(5);
    layout->addRow(new QLabel("Quality"), w);

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
