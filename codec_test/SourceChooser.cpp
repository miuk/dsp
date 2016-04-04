#include "SourceChooser.hxx"
#include "AudioSource.hxx"

#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>

#include <iostream>
using namespace std;

SourceChooser::SourceChooser(QWidget* parent)
{
    this->parent = parent;
    layout = new QHBoxLayout(parent);
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
    if (src->open(QIODevice::ReadOnly) == false) {
        cout << "open failed" << endl;
        return;
    }

    QAudioFormat fmt;
    fmt.setSampleRate(8000);
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
}
