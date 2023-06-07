//
//  Created by Stasel
//

#pragma once
#include "../Common.h"
#include "../Utilities/UUID.h"

class RemoteClient {
public:
    RemoteClient(const UUID& id, const String& name);
    const UUID& getID() const;
    const String& getName() const;
private:
    const UUID id;
    const String name;
};

