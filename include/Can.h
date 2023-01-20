
#ifndef CAN_LIBRARY_CPP
#define CAN_LIBRARY_CPP

#include "CanDefs.h"
#include <Can/EBrytecCan.h>
#include "Spi.h"

class Can {

public:
    static void Init(int ssPin, int interruptPin, int speed, uint8_t idType, uint16_t filter);
    static void SendCanMsg(EBrytecCan::CanExtFrame msg);
    static bool Available();
    static EBrytecCan::CanExtFrame GetCanMsg();

private:
    static void Reset();
    static char ReadRegister(uint8_t address);
    static void SetRegister(uint8_t address, uint8_t data);
    static void ModifyRegister(uint8_t address, uint8_t mask, uint8_t data);
    static char ReadStatus();
    static char ReadRxStatus();
    static void SetControlMode(uint8_t newmode);
    static void ConfigSpeed(uint8_t speed);
    static void CanInit(uint8_t speed, uint8_t idType, uint16_t filter);
};

#endif