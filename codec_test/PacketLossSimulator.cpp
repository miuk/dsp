#include "PacketLossSimulator.hxx"

#include <stdlib.h>

bool
PacketLossSimulator::isLost(void)
{
    cnt++;
    if (lossRate <= 0)
        return false;
    switch (lossType) {
    case 0 :
        return ((random() % 100) < lossRate);
    case 1 :
        return ((cnt % (100 / lossRate)) == 0);
    default :
        return false;
    }
    return false;
}
