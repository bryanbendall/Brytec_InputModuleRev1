#include "Boards/BrytecBoard.h"

#include "Can.h"
#include "InputModuleRev1Defines.h"
#include <Deserializer/BinaryProgmemDeserializer.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "Program.h"

namespace Brytec {

// Progmem
Brytec::BinaryProgmemDeserializer des(progmem_data, sizeof(progmem_data));

BinaryDeserializer* BrytecBoard::getDeserializer()
{
    des = Brytec::BinaryProgmemDeserializer(progmem_data, sizeof(progmem_data));
    return &des;
}

void BrytecBoard::error(EBrytecErrors error)
{
}

void BrytecBoard::setupBrytecCan(uint32_t mask, uint32_t filter)
{
    Can::Init(B0, CAN500kBPS, CAN_ID_EXT, mask, filter);
}

void BrytecBoard::setupPin(uint16_t index, IOTypes::Types type)
{
    // Done in main file
}

void BrytecBoard::shutdownAllPins()
{
    for (uint16_t i = BT_PIN_Black_2; i < BT_PIN_Blue_13; i++)
        setPinValue(i, IOTypes::Types::Undefined, 0.0f);
}

float AnalogVoltage(int pin)
{
    ADMUX &= ~(1 << MUX0 | 1 << MUX1 | 1 << MUX2 | 1 << MUX3 | 1 << MUX4); // Clear selected channel
    ADCSRB = 0;

    switch (pin) {
    case ADC1D:
        ADMUX |= (1 << MUX0); // ADC1
        break;
    case ADC3D:
        ADMUX |= (1 << MUX1) | (1 << MUX0); // ADC3
        break;
    case ADC7D:
        ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ADC7
        break;
    default:
        return -1;
    }

    ADCSRA |= (1 << ADSC); // Start conversion
    while ((ADCSRA & (1 << ADSC)))
        ; // Wait until adc is done conversion

    uint8_t low = ADCL;
    uint8_t high = ADCH;
    uint16_t reading = ((high << 8) | low);

    return (float)reading / (1023.0f / 5.0f);
}

float BrytecBoard::getPinValue(uint16_t index)
{
    switch (index) {
    case BT_PIN_Black_1:
        return !(PINE & (1 << PIN2)); // Black 1
    case BT_PIN_Black_8:
        return !(PINK & (1 << PIN3)); // Black 8
    case BT_PIN_Black_9:
        return !(PINF & (1 << PIN0)); // Black 9
    case BT_PIN_Black_20:
        return !(PINK & (1 << PIN1)); // Black 20
    case BT_PIN_Black_28:
        return !(PINC & (1 << PIN3)); // Black 28
    case BT_PIN_Black_32:
        return !(PINK & (1 << PIN0)); // Black 32

    case BT_PIN_Black_3:
        return !(PINE & (1 << PIN4));
    case BT_PIN_Black_4:
        return !(PINE & (1 << PIN5));
    case BT_PIN_Black_5:
        return !(PINE & (1 << PIN6));
    case BT_PIN_Black_7:
        return !(PINK & (1 << PIN6));
    case BT_PIN_Black_18:
        return !(PINK & (1 << PIN7));
    case BT_PIN_Black_19:
        return !(PINK & (1 << PIN4));

    case BT_PIN_Black_17:
        return !(PINC & (1 << PIN4));
    case BT_PIN_Black_29:
        return !(PINC & (1 << PIN5));
    case BT_PIN_Black_30:
        return !(PINK & (1 << PIN5));
    case BT_PIN_Black_31:
        return !(PINK & (1 << PIN2));

    case BT_PIN_Blue_5:
        return !(PIND & (1 << PIN4)); // Blue 5
    case BT_PIN_Blue_6:
        return !(PIND & (1 << PIN6)); // Blue 6
    case BT_PIN_Blue_7:
        return !(PINJ & (1 << PIN2)); // Blue 7
    case BT_PIN_Blue_8:
        return !(PINJ & (1 << PIN0)); // Blue 8
    case BT_PIN_Blue_9:
        return !(PINC & (1 << PIN2)); // Blue 9
    case BT_PIN_Blue_18:
        return !(PINJ & (1 << PIN3)); // Blue 18
    case BT_PIN_Blue_19:
        return !(PINJ & (1 << PIN1)); // Blue 19
    case BT_PIN_Blue_21:
        return !(PINC & (1 << PIN1)); // Blue 21
    case BT_PIN_Blue_31:
        return !(PIND & (1 << PIN7)); // Blue 31

    // Analog
    case BT_PIN_Black_10:
        return AnalogVoltage(ADC3D); // Black 10 PF3 ADC3
    case BT_PIN_Black_21:
        return AnalogVoltage(ADC1D); // Black 21 PF1 ADC1
    case BT_PIN_Black_35:
        return AnalogVoltage(ADC7D); // Black 35 PF7 ADC7
    }

    return 0.0f;
}

float BrytecBoard::getPinVoltage(uint16_t index)
{
    // Not supported
    return 0.0f;
}

float BrytecBoard::getPinCurrent(uint16_t index)
{
    // Not supported for this module
    return 0.0f;
}

void BrytecBoard::setPinValue(uint16_t index, IOTypes::Types type, float value)
{
    bool state = value > 0.001f;
    switch (index) {
    case BT_PIN_Black_2:
        state ? PORTE |= (1 << PIN3) : PORTE &= ~(1 << PIN3); // Black 2
        break;
    case BT_PIN_Black_11:
        state ? PORTF |= (1 << PIN4) : PORTF &= ~(1 << PIN4); // Black 11
        break;
    case BT_PIN_Black_13:
        state ? PORTH |= (1 << PIN5) : PORTH &= ~(1 << PIN5); // Black 13
        break;
    case BT_PIN_Black_14:
        state ? PORTH |= (1 << PIN3) : PORTH &= ~(1 << PIN3); // Black 14
        break;
    case BT_PIN_Black_15:
        state ? PORTH |= (1 << PIN1) : PORTH &= ~(1 << PIN1); // Black 15
        break;
    case BT_PIN_Black_22:
        state ? PORTF |= (1 << PIN6) : PORTF &= ~(1 << PIN6); // Black 22
        break;
    case BT_PIN_Black_33:
        state ? PORTF |= (1 << PIN5) : PORTF &= ~(1 << PIN5); // Black 33
        break;
    case BT_PIN_Black_34:
        state ? PORTF |= (1 << PIN2) : PORTF &= ~(1 << PIN2); // Black 34
        break;
    case BT_PIN_Blue_13:
        state ? PORTL |= (1 << PIN3) : PORTL &= ~(1 << PIN3); // Blue 13
        break;
    }
}

void BrytecBoard::sendBrytecCan(CanExtFrame frame)
{
    cli();
    Can::SendCanMsg(frame);
    sei();
}

void BrytecBoard::ReserveConfigSize(uint16_t size)
{
}

void BrytecBoard::updateConfig(uint8_t* data, uint32_t size, uint32_t offset)
{
}

uint32_t BrytecBoard::getTemplateSize()
{
    return sizeof(moduleTemplate);
}

void BrytecBoard::getTemplateData(uint8_t* dest, uint32_t offset, uint32_t length)
{
    if (offset > sizeof(moduleTemplate))
        return;

    memcpy_P(dest, &moduleTemplate[offset], length);
}

uint32_t BrytecBoard::getConfigSize()
{
    return sizeof(progmem_data);
}

void BrytecBoard::getConfigData(uint8_t* dest, uint32_t offset, uint32_t length)
{
    if (offset > sizeof(progmem_data))
        return;

    memcpy_P(dest, &progmem_data[offset], length);
}
}