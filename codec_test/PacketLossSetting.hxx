
#ifndef __PACKET_LOSS_SETTING_HXX__
#define __PACKET_LOSS_SETTING_HXX__

#include "PacketLossSimulator.hxx"

#include <QWidget>
#include <QLayout>

class PacketLossSetting : public QObject {
    Q_OBJECT
public:
    PacketLossSetting(QWidget* parent, PacketLossSimulator* pls);
    virtual ~PacketLossSetting(void);
    QLayout* getLayout(void) { return layout; };
private slots:
    void onLossRateChanged(int value);
    void onLossTypeChanged(int id, bool state);
    void onPtimeChanged(int value);
private:
    QWidget* parent;
    QLayout* layout;
    PacketLossSimulator* pls;
};

#endif // __PACKET_LOSS_SETTING_HXX__
