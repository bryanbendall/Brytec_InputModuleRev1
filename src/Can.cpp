#include "Can.h"

#include <avr/io.h>
#include <util/delay.h>

Spi spi;

// Public
void Can::Init(int ssPin, int interruptPin, int speed, uint8_t idType, uint16_t filter)
{
    spi.SpiInit(ssPin);
    CanInit(speed, idType, filter);
}

void Can::SendCanMsg(EBrytecCan::CanExtFrame msg)
{
    char temp;
    char status = 1;

    while (status) { // Wait until buffer is empty
        temp = ReadStatus();
        status = (temp & 0x04);
    }

    spi.SSEnable();
    spi.SpiTransfer(CAN_WRITE);
    spi.SpiTransfer(TXB0SIDH);
    spi.SpiTransfer(msg.id >> 21);
    spi.SpiTransfer((((msg.id >> 18) & 0x07) << 5) | 0x08 | ((msg.id >> 16) & 0x03));
    spi.SpiTransfer((msg.id >> 8) & 0xff);
    spi.SpiTransfer(msg.id & 0xff);
    spi.SpiTransfer(msg.dlc); // Data length code
    spi.SpiTransfer(msg.data[0]);
    spi.SpiTransfer(msg.data[1]);
    spi.SpiTransfer(msg.data[2]);
    spi.SpiTransfer(msg.data[3]);
    spi.SpiTransfer(msg.data[4]);
    spi.SpiTransfer(msg.data[5]);
    spi.SpiTransfer(msg.data[6]);
    spi.SpiTransfer(msg.data[7]);
    spi.SSDisable();
    // Request to send
    spi.SSEnable();
    spi.SpiTransfer(CAN_RTS0);
    spi.SSDisable();
}

bool Can::Available()
{
    char temp = ReadStatus();
    if (temp & 0x03) {
        return true;
    } else {
        return false;
    }
}

EBrytecCan::CanExtFrame Can::GetCanMsg()
{
    EBrytecCan::CanExtFrame msg;
    uint32_t id1, id2, id3, id4;
    if (ReadRxStatus() & 0x40) { // Message in RXB0
        spi.SSEnable();
        spi.SpiTransfer(CAN_READ);
        spi.SpiTransfer(CAN_READRX0BUFF);
        id1 = spi.SpiTransfer(0x00);
        id2 = spi.SpiTransfer(0x00);
        id3 = spi.SpiTransfer(0x00);
        id4 = spi.SpiTransfer(0x00);
        msg.dlc = spi.SpiTransfer(0x00);
        msg.data[0] = spi.SpiTransfer(0x00);
        msg.data[1] = spi.SpiTransfer(0x00);
        msg.data[2] = spi.SpiTransfer(0x00);
        msg.data[3] = spi.SpiTransfer(0x00);
        msg.data[4] = spi.SpiTransfer(0x00);
        msg.data[5] = spi.SpiTransfer(0x00);
        msg.data[6] = spi.SpiTransfer(0x00);
        msg.data[7] = spi.SpiTransfer(0x00);
        spi.SSDisable();

        ModifyRegister(CANINTF, 0x01, 0);

    } else if (ReadRxStatus() & 0x80) { // Message in RXB1
        spi.SSEnable();
        spi.SpiTransfer(CAN_READ);
        spi.SpiTransfer(CAN_READRX1BUFF);
        id1 = spi.SpiTransfer(0x00);
        id2 = spi.SpiTransfer(0x00);
        id3 = spi.SpiTransfer(0x00);
        id4 = spi.SpiTransfer(0x00);
        msg.id = spi.SpiTransfer(0x00);
        msg.data[0] = spi.SpiTransfer(0x00);
        msg.data[1] = spi.SpiTransfer(0x00);
        msg.data[2] = spi.SpiTransfer(0x00);
        msg.data[3] = spi.SpiTransfer(0x00);
        msg.data[4] = spi.SpiTransfer(0x00);
        msg.data[5] = spi.SpiTransfer(0x00);
        msg.data[6] = spi.SpiTransfer(0x00);
        msg.data[7] = spi.SpiTransfer(0x00);
        spi.SSDisable();

        ModifyRegister(CANINTF, 0x02, 0);
    }

    uint32_t idA = (id1 << 3) & (id2 >> 5);
    uint32_t idB = (((id2 & 0x03) << 16) & 0x30000) | ((id3 << 8) & 0xff00) | id4;
    msg.id = (idA << 18) | idB;

    return msg;
}

// Private
void Can::Reset()
{
    spi.SSEnable();
    spi.SpiTransfer(CAN_RESET);
    _delay_ms(20);
    spi.SSDisable();
}

char Can::ReadRegister(uint8_t address)
{
    uint8_t data;
    spi.SSEnable();
    spi.SpiTransfer(CAN_READ);
    spi.SpiTransfer(address);
    data = spi.SpiTransfer(0x00);
    spi.SSDisable();
    return data;
}

void Can::SetRegister(uint8_t address, uint8_t data)
{
    spi.SSEnable();
    spi.SpiTransfer(CAN_WRITE);
    spi.SpiTransfer(address);
    spi.SpiTransfer(data);
    spi.SSDisable();
}

void Can::ModifyRegister(uint8_t address, uint8_t mask, uint8_t data)
{
    spi.SSEnable();
    spi.SpiTransfer(CAN_BITMODIFY);
    spi.SpiTransfer(address);
    spi.SpiTransfer(mask);
    spi.SpiTransfer(data);
    spi.SSDisable();
}

char Can::ReadStatus()
{
    uint8_t temp;
    spi.SSEnable();
    spi.SpiTransfer(CAN_READSTATUS);
    temp = spi.SpiTransfer(0x00);
    spi.SSDisable();
    return temp;
}

char Can::ReadRxStatus()
{
    uint8_t temp;
    spi.SSEnable();
    spi.SpiTransfer(CAN_READRXSTATUS);
    temp = spi.SpiTransfer(0x00);
    spi.SSDisable();
    return temp;
}

void Can::SetControlMode(uint8_t newmode)
{
    ModifyRegister(CANCTRL, CAN_MODE_MASK, newmode);
}

void Can::ConfigSpeed(uint8_t speed)
{
    uint8_t cfg1, cfg2, cfg3;
    switch (speed) {
    case (CAN5kBPS):
        cfg1 = S5kBPS_CFG1;
        cfg2 = S5kBPS_CFG2;
        cfg3 = S5kBPS_CFG3;
        break;

    case (CAN10kBPS):
        cfg1 = S10kBPS_CFG1;
        cfg2 = S10kBPS_CFG2;
        cfg3 = S10kBPS_CFG3;
        break;

    case (CAN20kBPS):
        cfg1 = S20kBPS_CFG1;
        cfg2 = S20kBPS_CFG2;
        cfg3 = S20kBPS_CFG3;
        break;

    case (CAN40kBPS):
        cfg1 = S40kBPS_CFG1;
        cfg2 = S40kBPS_CFG2;
        cfg3 = S40kBPS_CFG3;
        break;

    case (CAN50kBPS):
        cfg1 = S50kBPS_CFG1;
        cfg2 = S50kBPS_CFG2;
        cfg3 = S50kBPS_CFG3;
        break;

    case (CAN80kBPS):
        cfg1 = S80kBPS_CFG1;
        cfg2 = S80kBPS_CFG2;
        cfg3 = S80kBPS_CFG3;
        break;

    case (CAN100kBPS):
        cfg1 = S100kBPS_CFG1;
        cfg2 = S100kBPS_CFG2;
        cfg3 = S100kBPS_CFG3;
        break;

    case (CAN125kBPS):
        cfg1 = S125kBPS_CFG1;
        cfg2 = S125kBPS_CFG2;
        cfg3 = S125kBPS_CFG3;
        break;

    case (CAN200kBPS):
        cfg1 = S200kBPS_CFG1;
        cfg2 = S200kBPS_CFG2;
        cfg3 = S200kBPS_CFG3;
        break;

    case (CAN250kBPS):
        cfg1 = S250kBPS_CFG1;
        cfg2 = S250kBPS_CFG2;
        cfg3 = S250kBPS_CFG3;
        break;

    case (CAN500kBPS):
        cfg1 = S500kBPS_CFG1;
        cfg2 = S500kBPS_CFG2;
        cfg3 = S500kBPS_CFG3;
        break;
    }

    SetRegister(CNF1, cfg1);
    SetRegister(CNF2, cfg2);
    SetRegister(CNF3, cfg3);
}

void Can::CanInit(uint8_t speed, uint8_t idType, uint16_t filter)
{
    Reset();
    SetControlMode(CAN_MODE_CONFIG);
    ConfigSpeed(speed);
    // interrupts for message received
    SetRegister(CANINTE, (1 << RX1IE & 1 << RX0IE));
    // ids
    ModifyRegister(RXB0CTRL, CAN_RXROLLOVR, CAN_RXROLLOVR); // Enable rollover
    switch (idType) {
    case CAN_ID_STD:
        ModifyRegister(RXB0CTRL, CAN_RXID_MASK, CAN_ID_STD);
        ModifyRegister(RXB1CTRL, CAN_RXID_MASK, CAN_ID_STD);
        break;
    case CAN_ID_EXT:
        ModifyRegister(RXB0CTRL, CAN_RXID_MASK, CAN_ID_EXT);
        ModifyRegister(RXB1CTRL, CAN_RXID_MASK, CAN_ID_EXT);
        break;
    case CAN_ID_ANY:
        ModifyRegister(RXB0CTRL, CAN_RXID_MASK, CAN_ID_ANY);
        ModifyRegister(RXB1CTRL, CAN_RXID_MASK, CAN_ID_ANY);
        break;
    }
    // Masks
    // SetRegister(RXM0SIDH, 0xFF);
    // SetRegister(RXM0SIDL, 0xE0);
    // SetRegister(RXM1SIDH, 0xFF);
    // SetRegister(RXM1SIDL, 0xE0);
    // Filters
    // SetRegister(RXF0SIDH, filter >> 3);
    // SetRegister(RXF0SIDL, filter << 5);
    // SetRegister(RXF1SIDH, filter >> 3);
    // SetRegister(RXF1SIDL, filter << 5);

    SetControlMode(CAN_MODE_NORMAL);
}
