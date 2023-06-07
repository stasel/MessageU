//
//  Created by Stasel
//

#include "ClientInfo.h"
#include <fstream>
#include <exception>
#include "../Utilities/Base64.h"
#include <algorithm>

ClientInfo::ClientInfo(const String& name, const UUID& id, const Bytes& privateKey): name(name), id(id), privateKey(privateKey) { }

ClientInfo::ClientInfo(const String& filePath) {
    std::ifstream inStream;
    inStream.open(filePath);
    if(!inStream.is_open()) {
        throw std::runtime_error("Could not open client info file");
    }
    
    String hexID;
    String privateKeyBase64;
    std::getline(inStream, name);
    std::getline(inStream, hexID);
    std::getline(inStream, privateKeyBase64);
    id = UUID(hexID);
    privateKey = Base64::decodeToBytes(privateKeyBase64);
    
    inStream.close();
}

ClientInfo::ClientInfo(const ClientInfo& clientInfo) {
    name = clientInfo.name;
    id = clientInfo.id;
    privateKey = clientInfo.privateKey;
}

void ClientInfo::persist(const String& filePath) const {
    std::ofstream outStream;
    outStream.open(filePath);
    if(!outStream.is_open()) {
        throw std::runtime_error("Could not open client info file for writing");
    }
    
    var base64String = Base64::encode(privateKey);
    base64String.erase(std::remove(base64String.begin(), base64String.end(), '\n'), base64String.end());
    
    outStream << name << "\n" << id.hex() << "\n" << base64String;
    outStream.close();
}

const String& ClientInfo::getName() const {
    return name;
}

const UUID& ClientInfo::getID() const {
    return id;
}

const Bytes& ClientInfo::getPrivateKey() const {
    return privateKey;
}

ClientInfo& ClientInfo::operator=(const ClientInfo &other) {
    if (this != &other) {
        name = other.name;
        id = other.id;
        privateKey = other.privateKey;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const ClientInfo& clientInfo) {
    os << clientInfo.getName()  << "(" << clientInfo.getID() << ")";
    return os;
}

