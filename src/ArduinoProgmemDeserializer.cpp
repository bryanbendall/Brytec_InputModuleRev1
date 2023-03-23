#include "Deserializer/BinaryDeserializer.h"

#include "EBrytecConfig.h"
#include "pgmspace.h"

namespace Brytec {

#include "Program.h"

BinaryDeserializer::~BinaryDeserializer()
{
}

bool BinaryDeserializer::readInternal(uint8_t* data, uint32_t dataSize)
{
    memcpy_P(data, &progmem_data[m_currentOffset], dataSize);
    m_currentOffset += dataSize;
    return true;
}

void setData(uint8_t* data) { }

}