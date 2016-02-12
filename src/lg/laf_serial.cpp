#include "laf_serial.h"


using namespace OpenPST::LG;

LafSerial::LafSerial(std::string port, int baudrate, serial::Timeout timeout) :
    serial::Serial(port, baudrate, timeout),
    hdlcEncoder()
{

}

size_t LafSerial::write(uint8_t *data, size_t size)
{
    size_t bytesWritten = Serial::write(data, size);
    hexdump_tx(data, bytesWritten);
    return bytesWritten;
}

size_t LafSerial::read(uint8_t *buf, size_t size)
{
    size_t bytesRead = Serial::read(buf, size);
    hexdump_rx(buf, bytesRead);
    return bytesRead;
}

size_t LafSerial::write(std::vector<uint8_t> &data)
{
    size_t bytesWritten = Serial::write(data);
    hexdump_tx(&data[0], bytesWritten);
    return bytesWritten;
}

size_t LafSerial::read(std::vector<uint8_t> &buffer, size_t size)
{
    size_t bytesRead = Serial::read(buffer, size);
    hexdump_rx(&buffer[0], bytesRead);
    return bytesRead;
}

void LafSerial::initialize()
{
    std::vector<uint8_t> response;
    uint8_t initData[] = { 0x3A, 0xA1, 0x6E, 0x7E };
    write(&initData, sizeof(initData));
    read(response, LAF_MAX_DATA_SIZE);
    hexdump_rx(response, response.size());
}

std::string LafSerial::exec(std::string command)
{
    size_t rxSize;
    std::vector<uint8_t> response;
    std::string ret;
    LafCommand* packet;

    if (!command.size()) {
        throw LafInvalidArgument("Invalid command length");
    }

    packet = (LafCommand*) new uint8_t[sizeof() + command.size()] = {};

    packet->header.command = LAF_CMD_EXECUTE;
    packet->header.magic   = LAF_EXEC_MAGIC;

    std::memcpy(packet->data, command.c_str(), command.size());

    packet->header.crc = hdlcEncoder.crc16((char*)packet, (sizeof(packet->header) + command.size() + 1));

    try {
        write((uint8_t*)packet, (sizeof(packet->header) + command.size() + 1));

        rxSize = read(response, LAF_MAX_DATA_SIZE);
    } catch (...) {
        delete packet;
        throw;
    }

    if (((LafCommand*)&response)->header.command == LAF_CMD_FAIL) {
        throw LafResponseError("Device returned a failure response");
    }

    for (auto c : response) {
        ret.append(c);
    }

    return ret;
}
