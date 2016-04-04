#include "Player.hxx"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QSlider>
#include <QComboBox>

#include "iostream"
using namespace std;

Player::Player(QWidget* parent)
{
    src = NULL;
    audio = NULL;
    volume = 10;
    layout = new QVBoxLayout(parent);
    QHBoxLayout* h = new QHBoxLayout(parent);
    layout->addLayout(h);

    // source name label
    QLabel* l = new QLabel(parent);
    connect(this, SIGNAL(setName(const QString&))
            , l, SLOT(setText(const QString&)));
    h->addWidget(l);

    // Play/Pause button
    QIcon ico;
    QStyle* style = QApplication::style();
    QIcon ic = style->standardIcon(QStyle::SP_MediaPlay);
    QPixmap px = ic.pixmap(16);
    ico.addPixmap(px, QIcon::Normal, QIcon::On);
    ic = style->standardIcon(QStyle::SP_MediaPause);
    px = ic.pixmap(16);
    ico.addPixmap(px, QIcon::Normal, QIcon::Off);
    QPushButton* b = new QPushButton(parent);
    b->setCheckable(true);
    b->setIcon(ico);
    connect(b, SIGNAL(toggled(bool)), this, SLOT(onPlayToggled(bool)));
    connect(this, SIGNAL(setEnabled(bool)), b, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(setPaused(bool)), b, SLOT(setChecked(bool)));
    h->addWidget(b);

    // Stop button
    ico = style->standardIcon(QStyle::SP_MediaStop);
    b = new QPushButton(parent);
    b->setIcon(ico);
    connect(b, SIGNAL(clicked()), this, SLOT(onStopClicked()));
    connect(this, SIGNAL(setEnabled(bool)), b, SLOT(setEnabled(bool)));
    h->addWidget(b);

    // Volume slider
    ico = style->standardIcon(QStyle::SP_MediaVolume);
    l = new QLabel(parent);
    l->setPixmap(ico.pixmap(16));
    h->addWidget(l);
    QSlider* slider = new QSlider(Qt::Horizontal);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
    h->addWidget(slider);
    //QStyle::SP_MediaVolumeMuted

    QList<QAudioDeviceInfo> al = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if (al.size() > 1) {
        QComboBox* cb = new QComboBox(parent);
        for (QList<QAudioDeviceInfo>::const_iterator i = al.begin();
             i != al.end(); i++) {
            const QAudioDeviceInfo& info = *i;
            cb->addItem(info.deviceName());
        }
        connect(cb, SIGNAL(currentIndexChanged(const QString&))
                , this, SLOT(onDeviceChanged(const QString&)));
        h->addWidget(cb);
    }
    setEnabled(false);
}

Player::~Player(void)
{
}

static QAudioDeviceInfo
getAudioDeviceInfo(const QString& name)
{
    if (name.isEmpty())
        return QAudioDeviceInfo::defaultOutputDevice();
    QList<QAudioDeviceInfo> al = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (QList<QAudioDeviceInfo>::const_iterator i = al.begin();
         i != al.end(); i++) {
        const QAudioDeviceInfo& info = *i;
        if (info.deviceName() == name)
            return info;
    }
    return QAudioDeviceInfo::defaultOutputDevice();
}

bool
Player::setSource(const QString& name
                  , const QAudioFormat& fmt
                  , QIODevice* src)
{
    info = getAudioDeviceInfo(devName);
    if (!info.isFormatSupported(fmt)) {
        cerr << "unsupported format" << endl;
        return false;
    }
    if (audio != NULL) {
        audio->stop();
        delete audio;
        audio = NULL;
    }
    if (this->src != NULL) {
        this->src->close();
        delete this->src;
        this->src = NULL;
    }
    this->fmt = fmt;
    this->name = name;
    this->src = src;
    setName(name);
    startAudio();
    return true;
}

void
Player::startAudio(void)
{
    if (src == NULL)
        return;
    if (audio != NULL) {
        audio->stop();
        delete audio;
        audio = NULL;
    }
    audio = new QAudioOutput(info, fmt, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State))
            , this, SLOT(onAudioStateChanged(QAudio::State)));
    audio->setVolume(volume/100.0);
    setEnabled(true);
    bPaused = false;
    setPaused(bPaused);
    src->reset();
    audio->start(src);
}

void
Player::onVolumeChanged(int value)
{
    cout << "onVolumeChanged " << value << endl;
    volume = value;
    if (audio != NULL)
        audio->setVolume(volume/100.0);
}

void
Player::onPlayToggled(bool checked)
{
    cout << "onPlayToggled " << checked << endl;
    bPaused = checked;
    if (audio != NULL) {
        if (bPaused)
            audio->suspend();
        else
            audio->resume();
    } else {
        if (bPaused == false)
            startAudio();
    }
}

void
Player::stopAudio(void)
{
    if (audio != NULL) {
        audio->stop();
        delete audio;
        audio = NULL;
    }
    bPaused = true;
    setPaused(bPaused);
}

void
Player::onStopClicked()
{
    cout << "onStopClicked" << endl;
    stopAudio();
}

void
Player::onDeviceChanged(const QString& name)
{
    cout << "onDeviceChanged " << name.toStdString() << endl;
    devName = name;
    info = getAudioDeviceInfo(devName);
    if (!info.isFormatSupported(fmt)) {
        cerr << "unsupported format" << endl;
        return;
    }
    if (audio != NULL) {
        startAudio();
    }
}

void
Player::onAudioStateChanged(QAudio::State state)
{
    if (audio == NULL)
        return;
    cout << "onAudioStateChanged ,state=" << state << endl;
    switch (state) {
    case QAudio::IdleState:
        stopAudio();
        cout << "onAudioStateChanged finished" << endl;
        break;
    case QAudio::StoppedState:
    default:
        if (audio->error() != QAudio::NoError) {
            cout << "audio error" << endl;
        }
        break;
    }
}
