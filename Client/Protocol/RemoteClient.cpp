//
//  Created by Stasel
//

#include "RemoteClient.h"

RemoteClient::RemoteClient(const UUID& id, const String& name): id(id), name(name) {}

const UUID& RemoteClient::getID() const {
    return id;
}

const String& RemoteClient::getName() const {
    return name;
}
