#include "PacketLossSetting.hxx"
#include "Slider.hxx"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

PacketLossSetting::PacketLossSetting(QWidget* parent
                                     , PacketLossSimulator* pls)
{
    this->parent = parent;
    this->pls = pls;
    QFormLayout* layout = new QFormLayout(parent);
    this->layout = layout;

    QButtonGroup* bg = new QButtonGroup();
    QHBoxLayout* w = new QHBoxLayout(parent);
    QRadioButton* b = new QRadioButton("Random");
    b->setChecked(true);
    w->addWidget(b);
    bg->addButton(b, 0);
    b = new QRadioButton("Periodic");
    w->addWidget(b);
    bg->addButton(b, 1);
    connect(bg, SIGNAL(buttonToggled(int,bool))
            , this, SLOT(onLossTypeChanged(int,bool)));
    layout->addRow(new QLabel("Loss Type"), w);

    w = new QHBoxLayout(parent);
    Slider* s = new Slider(parent, 0, 100, 0);
    s->addToLayout(w);
    connect(s, SIGNAL(valueChanged(int)), SLOT(onLossRateChanged(int)));
    layout->addRow(new QLabel("Loss Rate"), w);

    w = new QHBoxLayout(parent);
    s = new Slider(parent, 20, 200, 20, 20);
    s->addToLayout(w);
    connect(s, SIGNAL(valueChanged(int)), SLOT(onPtimeChanged(int)));
    layout->addRow(new QLabel("ptime"), w);
}

PacketLossSetting::~PacketLossSetting(void)
{
}

void
PacketLossSetting::onLossRateChanged(int value)
{
    pls->setLossRate(value);
}

void
PacketLossSetting::onLossTypeChanged(int id, bool state)
{
    if (state)
        pls->setLossType(id);
}

void
PacketLossSetting::onPtimeChanged(int value)
{
    pls->setPtime(value);
}
