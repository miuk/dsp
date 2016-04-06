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
    if (state == false)
        return;
    codec->setCodec(id);
    chooseCodec(id);
}
