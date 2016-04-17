
#ifndef __SPEEX_OPTION_SETTING_HXX__
#define __SPEEX_OPTION_SETTING_HXX__

#include <QWidget>
#include <QLayout>

#include "SpeexCodec.hxx"

class SpeexOptionSetting : public QObject {
    Q_OBJECT
public:
    SpeexOptionSetting(QWidget* parent, SpeexCodec* codec);
    virtual ~SpeexOptionSetting(void);
    QLayout* getLayout(void) { return layout; };
private slots:
    void onXBRToggled(int id, bool state);
    void onQualityChanged(int value);
    void onPLCChanged(int value);
    void onVADChanged(int);
    void onDTXChanged(int);
    void onENHChanged(int);
signals:
    void setQuality(int value);
    void setPLC(int value);
private:
    QWidget* parent;
    QLayout* layout;
    SpeexCodec* codec;
    int xBR; // 0=CBR, 1=ABR, 2=VBR;
    int quality;
};

#endif // __SPEEX_OPTION_SETTING_HXX__
