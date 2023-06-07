//
//  Created by Stasel
//

#include "ClientDirectory.h"

struct ClientDirectoryRecord {
   UUID id;
   String name;
   Bytes publicKey;
   Bytes symmetricKey;
};

ClientDirectory::~ClientDirectory() {
    for (let &pair : clients) {
        delete pair.second;
    }
}

void ClientDirectory::addClient(const UUID& id, const String& name) {
    if (isClientIDExists(id)) {
        throw std::runtime_error("Try to add duplicate client");
    }
    let newRecord = new ClientDirectoryRecord{id, name};
    clients[id] = newRecord;
    clientsByName[name] = newRecord;
}

bool ClientDirectory::isClientNameExists(const String& name) const {
    return clientsByName.find(name) != clientsByName.end();
}

bool ClientDirectory::isClientIDExists(const UUID& clientID) const {
    return clients.find(clientID) != clients.end();
}

const UUID& ClientDirectory::getClientIDByName(const String& name) const {
    let client = clientsByName.find(name);
    if (client == clientsByName.end()) {
        throw std::runtime_error("Could not find client");
    }
    return (*client).second->id;
}

const String& ClientDirectory::getClientName(const UUID& clientID) const {
    if (!isClientIDExists(clientID)) {
        throw std::runtime_error("ClientID does not exist");
    }
    return clients.find(clientID)->second->name;
}

const Bytes& ClientDirectory::getPublicKey(const UUID& clientID) const {
    if (!isClientIDExists(clientID)) {
        throw std::runtime_error("ClientID does not exist");
    }
    return clients.find(clientID)->second->publicKey;
}

const Bytes& ClientDirectory::getSymmetricKey(const UUID& clientID) const {
    if (!isClientIDExists(clientID)) {
        throw std::runtime_error("ClientID does not exist");
    }
    return clients.find(clientID)->second->symmetricKey;
}
void ClientDirectory::setClientName(const UUID& clientID, const String& newName) {
    if (!isClientIDExists(clientID)) {
        throw std::runtime_error("ClientID does not exist");
    }
    let oldName = clients[clientID]->name;
    if (oldName == newName) {
        return;
    }
    clients[clientID]->name = newName;
    clientsByName[newName] = clients[clientID];
    clientsByName.erase(oldName);
}

void ClientDirectory::setPublicKey(const UUID& clientID, const Bytes& publicKey) {
    if (!isClientIDExists(clientID)) {
        throw std::runtime_error("ClientID does not exist");
    }
    clients[clientID]->publicKey = publicKey;
}

void ClientDirectory::setSymmetricKey(const UUID& clientID, const Bytes& symmetricKey) {
    if (!isClientIDExists(clientID)) {
        throw std::runtime_error("ClientID does not exist");
    }
    clients[clientID]->symmetricKey = symmetricKey;

}
