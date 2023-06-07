//
//  Created by Stasel
//

#include "ServerInfo.h"
#include <fstream>
#include <exception>
#include <boost/algorithm/string.hpp>

ServerInfo::ServerInfo(const String& host, UInt16 port): host(host), port(port) { }

// Read server info from file
ServerInfo::ServerInfo(const String& filePath) {
    std::ifstream inStream;
    inStream.open(filePath);
    if(!inStream.is_open()) {
        throw std::runtime_error("Could not open server info file");
    }
    
    String line;
    std::getline(inStream, line);
    inStream.close();
    
    std::vector<String> components;
    boost::split(components, line, [](char c){return c == ':';});
    this->host = components[0];
    this->port = (UInt16)std::stoi(components[1]);
}

const String& ServerInfo::getHost() const {
    return host;
}
const UInt16 ServerInfo::getPort() const {
    return port;
}

std::ostream& operator<<(std::ostream& os, const ServerInfo& serverInfo) {
    os << serverInfo.getHost() << ":" << serverInfo.getPort();
    return os;
}
