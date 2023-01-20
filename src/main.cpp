#include <avr/io.h>

#include "Serial.h"
#include "Can.h"
#include <EBrytecApp.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

EBrytecApp app;

// Variables
volatile uint64_t milli = 0;
bool timerState = false;
static uint64_t lastMillis = 0;

// Timer0 overflow interrupt
ISR(TIMER0_COMPA_vect)
{
    milli++;
}

void DataDirections()
{
    // Data direction
    DDRA = 0b00001111;
    DDRB |= (1 << PINB4) | (1 << PINB5);
    DDRC = 0b00010000;
    DDRD = 0b00000000;
    DDRE = 0b01111000;
    DDRF = 0b01110100;
    DDRH = 0b11111111;
    DDRJ = 0b00000000;
    DDRK = 0b00000000;
    DDRL = 0b11111111;
}

void OutputStateAndPullups()
{
    // Outputs state low and pullup resistors on
    PORTA = 0b11110000;
    PORTC = 0b11101111;
    PORTD = 0b11111111;
    PORTE = 0b10100100;
    PORTF |= (1 << PINF0) | (1 << PINF7);
    PORTH = 0b00000000;
    PORTJ = 0b11111111;
    PORTK = 0b11111111;
    PORTL = 0b00000000;
}

void SetUpTimer0()
{
    timerState = true;
    // TCCR0A = 0;
    // TCCR0B |= (1 << CS02) | (1 << CS00); // Clock divided by 1024
    // OCR0A = 15; // Output compare 0A at 15 aprox every 1ms

    OCR0A = 249;
    TCCR0A = 2;
    TCCR0B = 3;

    TIMSK0 |= (1 << OCIE0A); // Enable output compare 0A interrupt
    sei(); // Enable interrupts
}

void Timer0IntOff()
{
    timerState = false;
    TIMSK0 &= ~(1 << OCIE0A);
    milli = 0;
}

void Timer0IntOn()
{
    timerState = true;
    TIMSK0 |= (1 << OCIE0A);
}

void SetUpADC()
{
    // Setup ADC
    ADMUX |= (1 << REFS0); // AVCC reference voltage
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC
}

void checkCan()
{
    if (Can::Available()) {
        EBrytecCan::CanExtFrame frame = Can::GetCanMsg();
        app.brytecCanReceived(frame);
    }
}

int main()
{
    DataDirections();
    OutputStateAndPullups();
    SetUpTimer0();
    SetUpADC();

    // Serial::Init(0, BAUD9600);

    BinaryDeserializer des;
    app.deserializeModule(des);

    // if (EBrytecApp::isDeserializeOk())
    //     Serial::SendString("Des succ", 8, true);
    // else
    //     Serial::SendString("Des fail", 8, true);

    app.setupModule();
    app.setupPins();

    while (1) {

        uint64_t difference = milli - lastMillis;
        if (difference > 1) {
            float timestep = ((float)difference * 0.001f);
            lastMillis = milli;

            app.update(timestep);
            checkCan();

        }
    }
}