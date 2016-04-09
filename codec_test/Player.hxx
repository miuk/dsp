
#include <QWidget>
#include <QBoxLayout>
#include <QIODevice>
#include <QAudioFormat>
#include <QAudioOutput>

class Player : public QObject {
    Q_OBJECT
public:
    Player(QWidget* parent);
    virtual ~Player(void);
    QLayout* getLayout(void) { return layout; };

public slots:
    bool setSource(const QString& name
                   , const QAudioFormat& fmt
                   , QIODevice* src);

private slots:
    void onVolumeChanged(int value);
    void onPlayToggled(bool checked);
    void onStopClicked();
    void onDeviceChanged(const QString& name);
    void onAudioStateChanged(QAudio::State state);
    void elapsed(void);

signals:
    void showElapsed(const QString& name);
    void setEnabled(bool);
    void setPaused(bool val);

private:
    void startAudio(void);
    void stopAudio(void);
    void resetElapsed(void);

private:
    QBoxLayout* layout;

    // input source
    QIODevice* src;
    QString name;
    QAudioFormat fmt;
    QAudioDeviceInfo info;
    QAudioOutput* audio;

    // audio controls
    int volume;
    QString devName;
    bool bPaused;

    int elapsedSec;
};
