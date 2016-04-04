#include "Player.hxx"
#include "AudioSource.hxx"
#include "SourceChooser.hxx"
#include "SpeexOptionSetting.hxx"

#include <QApplication>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <QAudioOutput>
#include <QMediaPlayer>

#include <string>
#include <iostream>
using namespace std;

class RadioGroup : public QObject {
    Q_OBJECT
public:
    RadioGroup(const char* label, const char* strs[], int value) {
        w = new QHBoxLayout();
        if (label != NULL) {
            QLabel* l = new QLabel(label);
            w->addWidget(l);
        }
        bg = new QButtonGroup();
        for (int id = 0; strs[id] != NULL; id++) {
            QRadioButton* b = new QRadioButton(strs[id]);
            if (value == id)
                b->setChecked(true);
            w->addWidget(b);
            bg->addButton(b, id);
        }
        QObject::connect(bg, SIGNAL(buttonToggled(int,bool))
                         , this, SLOT(toggled(int,bool)));
        currValue = value;
    };
    QLayout* getLayout(void) { return w; };
    int getCurrentValue(void) const { return currValue; };
private slots:
    void toggled(int id, bool state) {
        if (state) {
            currValue = id;
            valueChanged(id);
        }
    };

signals:
    void valueChanged(int value);
private:
    QButtonGroup* bg;
    QHBoxLayout* w;
    int currValue;
};

class MyApp : public QWidget {
    // SLOT を作る場合必須
    Q_OBJECT
public:
    MyApp(QApplication* app) { this->app = app; };
    virtual ~MyApp(void) {};
    void init(void);
private:

private:
    QApplication* app;
    QVBoxLayout* v;
};


void
MyApp::init(void)
{
    v = new QVBoxLayout();

    QGroupBox* gb = new QGroupBox("player");
    Player* player = new Player(NULL);
    gb->setLayout(player->getLayout());
    v->addWidget(gb);

    gb = new QGroupBox("source");
    SourceChooser* chooser = new SourceChooser(this);
    connect(chooser, SIGNAL(chooseSource(const QString&, const QAudioFormat&, QIODevice*))
            , player, SLOT(setSource(const QString&, const QAudioFormat&, QIODevice*)));
    gb->setLayout(chooser->getLayout());
    v->addWidget(gb);

    gb = new QGroupBox("speex");
    SpeexOptionSetting* spx = new SpeexOptionSetting(this);
    gb->setLayout(spx->getLayout());
    v->addWidget(gb);

    setLayout(v);
}

int
main(int ac, char* av[])
{
    QApplication app(ac, av);
    MyApp* w = new MyApp(&app);
    w->init();
    w->show();
    
    return app.exec();
}

// SLOT を作る場合必須
#include "Hello.moc"
