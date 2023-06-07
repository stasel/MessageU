//
//  Created by Stasel
//

#pragma once
#include "../Common.h"
#include "../Utilities/UUID.h"
#include <vector>

class ClientInfo {
public:
    ClientInfo(const String& name, const UUID& id, const Bytes& privateKey);
    ClientInfo(const String& filePath);
    ClientInfo(const ClientInfo& clientInfo);
    void persist(const String& filePath) const;
    const String& getName() const;
    const UUID& getID() const;
    const Bytes& getPrivateKey() const;
    friend std::ostream& operator<<(std::ostream& os, const ClientInfo& clientInfo);
    ClientInfo& operator=(const ClientInfo& other);
private:
    String name;
    UUID id;
    Bytes privateKey;
};
