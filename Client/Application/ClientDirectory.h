//
//  Created by Stasel
//

#pragma once

#include "ClientDirectory.h"
#include <map>
#include "../Common.h"
#include "../Utilities/UUID.h"

struct ClientDirectoryRecord;

// Client Directory class keeps track on the client public and symmetric keys
class ClientDirectory {
public:
    ~ClientDirectory();
    void addClient(const UUID& id, const String& name);
    bool isClientIDExists(const UUID& clientID) const;
    bool isClientNameExists(const String& name) const;
    const UUID& getClientIDByName(const String& name) const;
    const String& getClientName(const UUID& clientID) const;
    const Bytes& getPublicKey(const UUID& clientID) const;
    const Bytes& getSymmetricKey(const UUID& clientID) const;
    void setClientName(const UUID& clientID, const String& name);
    void setPublicKey(const UUID& clientID, const Bytes& publicKey);
    void setSymmetricKey(const UUID& clientID, const Bytes& symmetricKey);
private:
    std::map<UUID, ClientDirectoryRecord*> clients;
    std::map<String, ClientDirectoryRecord*> clientsByName;
};
