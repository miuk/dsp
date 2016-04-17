#ifndef __PACKET_LOSS_SIMULATOR_HXX__
#define __PACKET_LOSS_SIMULATOR_HXX__

class PacketLossSimulator {
public:
    PacketLossSimulator(void) {
        ptime = 20;
        lossType = 0;
        lossRate = 0;
        cnt = 0;
    };
    int getPtime(void) const { return ptime; };
    int getLossType(void) const { return lossType; };
    int getLossRate(void) const { return lossRate; };
    void setPtime(int value) { ptime = value; };
    void setLossType(int value) { lossType = value; };
    void setLossRate(int value) { lossRate = value; };
    bool isLost(void);
private:
    int ptime;
    int lossType;
    int lossRate;
    int cnt;
};

#endif // __PACKET_LOSS_SIMULATOR_HXX__
