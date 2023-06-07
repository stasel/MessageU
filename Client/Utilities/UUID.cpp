//
//  Created by Stasel
//

#include "UUID.h"
#include <exception>
#include <assert.h>

UInt8 asciiToByte(UInt8 input) {
    if (input >= '0' && input <= '9') {
        return input - '0';
    }
    if (input >= 'A' && input <= 'F') {
        return input - 'A' + 10;
    }
    if (input >= 'a' && input <= 'f') {
        return input - 'a' + 10;
    }
    throw std::invalid_argument("Invalid hex input");
}
UInt8 byteToAscii(UInt8 byte) {
    if (byte < 10) {
        return byte + '0';
    }
    if (byte < 16) {
        return byte - 10 + 'A';
    }
    throw std::invalid_argument("Invalid byte");
}

UUID::UUID() {
    for (var i = 0; i < UUID_SIZE; i++) {
        bytes.push_back(0);
    }
}

UUID::UUID(const Bytes& bytes) {
    assert(bytes.size() == UUID_SIZE);
    this->bytes = bytes;
}

UUID::UUID(const std::string& string) {
    if (string.size() != UUID_SIZE*2) {
        throw std::invalid_argument("Invalid UUID string size");
    }
    
    for(var i = 0; i < UUID_SIZE*2; i+= 2) {
        std::string byteString = string.substr(i, 2);
        UInt8 byte = (UInt8)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
}

UUID::UUID(const UUID& uuid) {
    bytes = uuid.bytes;
}


const Bytes& UUID::getBytes() const{
    return bytes;
}

UUID& UUID::operator=(const UUID &other) {
    if (this != &other) {
        bytes = other.bytes;
    }
    return *this;
}

const std::string UUID::hex() const {
    std::string result;
    for(let byte: bytes) {
        result += byteToAscii(byte >> 4);
        result += byteToAscii(byte & 0x0f);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
    os << uuid.hex();
    return os;
}

bool operator==(const UUID& lhs, const UUID& rhs) {
    return lhs.bytes == rhs.bytes;
}

bool operator<(const UUID& lhs, const UUID& rhs) {
    return lhs.getBytes() < rhs.getBytes();
}
