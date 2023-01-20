#include "Serial.h"

static uint8_t m_channel;

void Serial::Init(uint8_t channel, baudRate baud)
{
    m_channel = channel;
    uint16_t serialBaud = baud;

    switch (baud) {
    case BAUD9600:
        serialBaud = 103;
        break;
    case BAUD115200:
        serialBaud = 8;
        break;
    }
    switch (m_channel) {
    case 0:
        /* Set baud rate */
        UBRR0H = (unsigned char)(serialBaud >> 8);
        UBRR0L = (unsigned char)serialBaud;
        /* Enable receiver and transmitter */
        UCSR0B = (1 << RXEN0) | (1 << TXEN0);
        /* Set frame format: 8data, 2stop bit */
        UCSR0C = (1 << USBS0) | (3 << UCSZ00);
        break;
    case 1:
        /* Set baud rate */
        UBRR1H = (unsigned char)(serialBaud >> 8);
        UBRR1L = (unsigned char)serialBaud;
        /* Enable receiver and transmitter */
        UCSR1B = (1 << RXEN1) | (1 << TXEN1);
        /* Set frame format: 8data, 2stop bit */
        UCSR1C = (1 << USBS1) | (3 << UCSZ10);
        break;
    }
}

bool Serial::Available()
{
    // Check if there is serial data available
    switch (m_channel) {
    case 0:
        return (UCSR0A & (1 << RXC0));
    case 1:
        return (UCSR1A & (1 << RXC1));
    }

    return false;
}

void Serial::SendString(const char b[], int length, bool endline)
{
    for (int i = 0; i <= length; i++) {
        Transmit(b[i]);
    }
    if (endline)
        TransmitNewLine();
}

void Serial::Send(char data, bool endline)
{
    Transmit(data);
    if (endline)
        TransmitNewLine();
}

void Serial::SendInt(int data, bool endline)
{
    char dataString[] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
    itoa(data, dataString, 10);
    SendString(dataString, 9, endline);
}

char Serial::Recieve()
{
    // Get and return received data from buffer
    switch (m_channel) {
    case 0:
        return UDR0;
    case 1:
        return UDR1;
    }

    return ' ';
}

void Serial::Transmit(uint8_t data)
{
    switch (m_channel) {
    case 0:
        /* Wait for empty transmit buffer */
        while (!(UCSR0A & (1 << UDRE0)))
            ;

        /* Put data into buffer, sends the data */
        UDR0 = data;
        break;
    case 1:
        /* Wait for empty transmit buffer */
        while (!(UCSR1A & (1 << UDRE1)))
            ;

        /* Put data into buffer, sends the data */
        UDR1 = data;
        break;
    }
}

void Serial::TransmitNewLine()
{
    Transmit(10);
}
