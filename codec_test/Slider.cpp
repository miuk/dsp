#include "Slider.hxx"

#include <QLayout>

#include <iostream>
using namespace std;

Slider::Slider(QWidget* parent, int rangeMin, int rangeMax
               , int initValue, int step)
{
    this->parent = parent;
    this->step = step;
    s = new QSlider(Qt::Horizontal, parent);
    s->setMinimum(rangeMin / step);
    s->setMaximum(rangeMax / step);
    s->setValue(initValue / step);
    connect(s, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    l = new QLabel(parent);
    l->setFont(QFont("Courier"));
    connect(this, SIGNAL(setLabel(const QString&)), l, SLOT(setText(const QString&)));
    label_cols = calcLabelCols(rangeMin, rangeMax);
    onValueChanged(initValue / step);
}

Slider::~Slider(void)
{
}

void
Slider::addToLayout(QLayout* layout)
{
    layout->addWidget(s);
    layout->addWidget(l);
}

void
Slider::setValue(int val)
{
    s->setValue(val / step);
}

void
Slider::onValueChanged(int val)
{
    char work[32];
    val *= step;
    sprintf(work, "%-*d", label_cols, val);
    setLabel(work);
    valueChanged(val);
}

void
Slider::setRange(int rangeMin, int rangeMax)
{
    label_cols = calcLabelCols(rangeMin, rangeMax);
    s->setRange(rangeMin/step, rangeMax/step);
}

int
Slider::calcLabelCols(int rangeMin, int rangeMax)
{
    bool sign = false;
    if (rangeMin < 0) {
        sign = true;
        rangeMin = -rangeMin;
    }
    if (rangeMax< 0) {
        sign = true;
        rangeMax = -rangeMax;
    }
    int ret = 1;
    if (sign)
        ret++;
    int val = (rangeMax >= rangeMin) ? rangeMax : rangeMin;
    for (; val >= 10; val /= 10)
        ret++;
    return ret;
}
