#ifndef SPI_LIBRARY_CPP
#define SPI_LIBRARY_CPP

#include <avr/io.h>

// SS Pins
#define B0 0
#define B4 1
#define B5 2

class Spi {

private:
    int slavePin;

public:
    // Constructor
    Spi() = default;

    void SpiInit(int ssPin);
    uint8_t SpiTransfer(uint8_t data);
    void SSEnable();
    void SSDisable();
};

#endif