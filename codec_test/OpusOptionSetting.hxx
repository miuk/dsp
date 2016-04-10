
#ifndef __OPUS_OPTION_SETTING_HXX__
#define __OPUS_OPTION_SETTING_HXX__

#include <QWidget>
#include <QLayout>
#include <QAudioFormat>

#include "OpusCodec.hxx"

class OpusOptionSetting : public QObject {
    Q_OBJECT
public:
    OpusOptionSetting(QWidget* parent, OpusCodec* codec);
    virtual ~OpusOptionSetting(void);
    QLayout* getLayout(void) { return layout; };
private slots:
    void onXBRToggled(int id, bool state);
    void onKBpsChanged(int value);
    void chooseSource(const QString&, const QAudioFormat&, QIODevice*);
signals:
    void setKBps(int value);
    void setKBpsStr(const QString& value);
    void setKBpsRange(int min, int max);
private:
    QWidget* parent;
    QLayout* layout;
    OpusCodec* codec;
    int xBR; // 0=CBR, 1=VBR
    int kbps;
};

#endif // __OPUS_OPTION_SETTING_HXX__
