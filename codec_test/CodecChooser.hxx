
#ifndef __CODEC_CHOOSER_HXX__
#define __CODEC_CHOOSER_HXX__

#include "CodecWrapper.hxx"

#include <QWidget>
#include <QLayout>
#include <vector>

class CodecChooser : public QObject {
    Q_OBJECT
public:
    CodecChooser(QWidget* parent, CodecWrapper* codec);
    virtual ~CodecChooser(void);
    QLayout* getLayout(void) { return layout; };
    void addCodecOptionSetter(QWidget* setter);
private slots:
    void onCodecToggled(int id, bool state);
signals:
    void chooseCodec(int value);
private:
    QWidget* parent;
    QLayout* layout;
    CodecWrapper* codec;
    std::vector<QWidget*> optionSetters;
};

#endif // __CODEC_CHOOSER_HXX__
