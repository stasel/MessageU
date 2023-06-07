//
//  Created by Stasel
//

#pragma once
#include "../Common.h"
#include <vector>

#define UUID_SIZE 16

// Helper class to reprecent a 16 byte random ID
class UUID {
public:
    UUID();
    UUID(const Bytes& bytes);
    UUID(const std::string& string);
    UUID(const UUID& uuid);
    const std::string hex() const;
    const Bytes& getBytes() const;
    UUID& operator=(const UUID &other);
    friend std::ostream& operator<<(std::ostream& os, const UUID& uuid);
    friend bool operator==(const UUID& lhs, const UUID& rhs);
    friend bool operator<(const UUID& lhs, const UUID& rhs);
private:
    Bytes bytes;
};
