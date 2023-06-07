//
//  Created by Stasel
//

#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#define DEBUG 1
#define let const auto
#define var auto
#define PROMPT "> "
#define CLIENT_VERSION 2
#define USERNAME_MAX_LENGTH 255
#define VERSION_SIZE_BYTES 1
#define RESPONSE_CODE_SIZE_BYTES 2
#define PAYLOAD_SIZE_BYTES 4
#define PUBLIC_KEY_SIZE 160
#define MESSAGE_ID_SIZE 4

// typedefs (sorry)
typedef int8_t Int8;
typedef uint8_t UInt8;
typedef int16_t Int16;
typedef uint16_t UInt16;
typedef int32_t Int32;
typedef uint32_t UInt32;
typedef std::string String;
typedef std::vector<UInt8> Bytes;

// Helper functions to join and split 16 and 32 bit integers to/from little endian byte array
UInt16 joinBytes16(UInt8 bytes[]);
UInt32 joinBytes32(UInt8 bytes[]);
Bytes splitBytes16(UInt16 n);
Bytes splitBytes32(UInt32 n);

String randomString(uint32_t length);
String bytesToString(const Bytes& bytes);
Bytes stringToBytes(const String& string);
bool isAsciiOnly(const String& s);
const String tempFilePath();
Bytes readFile(const String& path);
void writeFile(const String& path, const Bytes& data);
