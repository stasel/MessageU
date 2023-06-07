//
//  Created by Stasel
//

#include "Common.h"
#include <cstdarg>
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <filesystem>

UInt16 joinBytes16(UInt8 bytes[]) {
    return bytes[0] + (bytes[1]<<8);
}
UInt32 joinBytes32(UInt8 bytes[]) {
    return bytes[0] + (bytes[1]<<8) + (bytes[2]<<16) + (bytes[3]<<24);
}
Bytes splitBytes16(UInt16 n) {
    Bytes result;
    result.push_back((UInt8)(n));
    result.push_back((UInt8)(n>>8));
    return result;
}
Bytes splitBytes32(UInt32 n) {
    Bytes result;
    result.push_back((UInt8)(n));
    result.push_back((UInt8)(n>>8));
    result.push_back((UInt8)(n>>16));
    result.push_back((UInt8)(n>>24));
    return result;
}

String randomString(uint32_t length) {
    static bool seedInitialized = false;
    if (!seedInitialized) {
        std::srand((UInt32)std::time(nullptr));
        seedInitialized = true;
    }
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static const uint32_t alphanumLength = 62;
    
    std::string result;
    for (uint32_t i = 0; i < length; i++) {
        result += alphanum[rand()%alphanumLength];
    }
    return result;
}

String bytesToString(const Bytes& bytes) {
    return String((char*)bytes.data(), bytes.size());
}

Bytes stringToBytes(const String& string) {
    let strPtr = (UInt8*)string.c_str();
    return Bytes(strPtr, strPtr+string.length());
}

bool isAsciiOnly(const std::string& s) {
    return std::count_if(s.begin(), s.end(), [](unsigned char c){ return !std::isprint(c); } ) == 0;
}

Bytes readFile(const String& path) {
    std::ifstream inStream(path.c_str(), std::ios::binary | std::ios::ate );
    
    if(!inStream.good()) {
        throw std::runtime_error("Could not read file");
    }
    
    let fileSize = inStream.tellg();
    inStream.seekg(0, std::ios::beg);
    Bytes contents(fileSize, 0);
    inStream.read((char*)contents.data(), fileSize);
    inStream.close();
    return contents;
}

void writeFile(const String& path, const Bytes& data) {
    std::ofstream outStream(path.c_str(), std::ios::binary | std::ios::out);
    if(!outStream.good()) {
        throw std::runtime_error("Could not open file for writing");
    }
    outStream.write((char*)data.data(), data.size());
    outStream.close();
}

const String tempFilePath() {
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    var tempPath = std::filesystem::temp_directory_path();
    tempPath.append(randomString(6));
    return tempPath.string();
#else
    #error "Hello, please compile me in C++ 17. Thanks!"
    return "";
#endif
}
