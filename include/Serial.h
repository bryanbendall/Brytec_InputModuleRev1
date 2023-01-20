#pragma once

#include <avr/io.h>
#include <stdlib.h>

enum baudRate {
    BAUD9600,
    BAUD115200
};

class Serial {

public:
    static void Init(uint8_t channel, baudRate baud);
    static bool Available();
    static void SendString(const char b[], int length, bool endline);
    static void Send(char data, bool endline);
    static void SendInt(int data, bool endline);
    static char Recieve();

private:
    static void Transmit(uint8_t data);
    static void TransmitNewLine();
};