//
//  Created by Stasel
//

#pragma once

#include "../Common.h"
#include "../Utilities/UUID.h"

enum MessageType {
    MESSAGE_TYPE_SYMMETRIC_KEY_REQUEST = 1,
    MESSAGE_TYPE_SYMMETRIC_KEY = 2,
    MESSAGE_TYPE_TEXT = 3,
    MESSAGE_TYPE_FILE = 4
};

class Message {
public:
    Message(const UUID& senderClientID, UInt32 messageID, MessageType type, const Bytes& content);
    const UUID& getSenderClientID() const;
    const UInt32 getMessageID() const;
    const MessageType getType() const;
    const Bytes& getContent() const;
private:
    const UInt32 messageID;
    const UUID senderClientID;
    const MessageType type;
    const Bytes content;
};
