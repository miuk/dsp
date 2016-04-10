#include "CodecChooser.hxx"

#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

#include <iostream>
using namespace std;

CodecChooser::CodecChooser(QWidget* parent, CodecWrapper* codec)
{
    this->parent = parent;
    this->codec = codec;
    QHBoxLayout* h = new QHBoxLayout(parent);
    layout = h;
    QButtonGroup* bg = new QButtonGroup();
    int n = codec->getMaxCodec();
    for (int i = 0; i < n; i++) {
        QRadioButton* b = new QRadioButton(codec->getCodecName(i));
        if (i == 0)
            b->setChecked(true);
        h->addWidget(b);
        bg->addButton(b, i);
    }
    connect(bg, SIGNAL(buttonToggled(int,bool))
            , this, SLOT(onCodecToggled(int,bool)));
}

CodecChooser::~CodecChooser(void)
{
}

void
CodecChooser::onCodecToggled(int id, bool state)
{
    QWidget* setter = NULL;
    if (id < (int)optionSetters.size())
        setter = optionSetters[id];
    if (state == false) {
        if (setter != NULL)
            setter->setVisible(false);
        return;
    }
    codec->setCodec(id);
    chooseCodec(id);
    if (setter != NULL)
        setter->setVisible(true);
}

void
CodecChooser::addCodecOptionSetter(QWidget* setter)
{
    optionSetters.push_back(setter);
}
