//
//  Created by Stasel
//

#include "Message.h"

Message::Message(const UUID& senderClientID, UInt32 messageID, MessageType type, const Bytes& content)
    : senderClientID(senderClientID), messageID(messageID), type(type), content(content) { }

const UUID& Message::getSenderClientID() const {
    return senderClientID;
}
const UInt32 Message::getMessageID() const {
    return  messageID;
}
const MessageType Message:: getType() const {
    return type;
}
const Bytes& Message::getContent() const {
    return content;
}
