
#include <QWidget>
#include <QLayout>

class SpeexOptionSetting : public QObject {
    Q_OBJECT
public:
    SpeexOptionSetting(QWidget* parent);
    virtual ~SpeexOptionSetting(void);
    QLayout* getLayout(void) { return layout; };
private slots:
    void onXBRToggled(int id, bool state);
    void onQualityChanged(int value);
signals:
    void setQuality(int value);
private:
    QWidget* parent;
    QLayout* layout;
    int xBR; // 0=CBR, 1=ABR, 2=VBR;
    int quality;
};
