#include <QWidget>
#include <QBoxLayout>
#include <QIODevice>
#include <QAudioFormat>

#include "Codec.hxx"

class SourceChooser : public QObject {
    Q_OBJECT
public:
    SourceChooser(QWidget* parent, Codec* codec);
    virtual ~SourceChooser(void);
    QLayout* getLayout(void) { return layout; };

public slots:

signals:
    void chooseSource(const QString& name
                      , const QAudioFormat& fmt
                      , QIODevice* src);
private slots:
    void open();

private:
    QWidget* parent;
    QBoxLayout* layout;
    Codec* codec;
};

