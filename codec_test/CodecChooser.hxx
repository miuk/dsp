
#ifndef __CODEC_CHOOSER_HXX__
#define __CODEC_CHOOSER_HXX__

#include "CodecWrapper.hxx"

#include <QWidget>
#include <QLayout>

class CodecChooser : public QObject {
    Q_OBJECT
public:
    CodecChooser(QWidget* parent, CodecWrapper* codec);
    virtual ~CodecChooser(void);
    QLayout* getLayout(void) { return layout; };
private slots:
    void onCodecToggled(int id, bool state);
signals:
    void chooseCodec(int value);
private:
    QWidget* parent;
    QLayout* layout;
    CodecWrapper* codec;
};

#endif // __CODEC_CHOOSER_HXX__
