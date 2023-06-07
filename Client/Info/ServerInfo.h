//
//  Created by Stasel
//

#pragma once
#include "../Common.h"

class ServerInfo {
public:
    ServerInfo(const String& host, UInt16 port);
    ServerInfo(const String& filePath);
    const String& getHost() const;
    const UInt16 getPort() const;
    friend std::ostream& operator<<(std::ostream& os, const ServerInfo& serverInfo);
private:
    String host;
    UInt16 port;
};
