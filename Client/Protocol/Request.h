//
//  Created by Stasel
//

#pragma once
#include "../Common.h"
#include "../Utilities/UUID.h"
#include "Message.h"

enum RequestCode {
    REQUEST_REGISTER = 1000,
    REQUEST_CLIENT_LIST = 1001,
    REQUEST_GET_PUBLIC_KEY = 1002,
    REQUEST_SEND_MESSAGE = 1003,
    REQUEST_GET_MESSAGES = 1004
};

class Request {
public:
    Request(const UUID& clientID, UInt8 version, RequestCode code);
    virtual Bytes pack() const;
    friend std::ostream& operator<<(std::ostream& os, const Request& request);
protected:
    Bytes pack(const Bytes& payload) const;
    const UUID clientID;
    const UInt8 version;
    const RequestCode code;
};

class RegisterRequest: public Request {
public:
    RegisterRequest(UInt8 version, const String& name, const Bytes& publicKey);
    virtual Bytes pack() const;
private:
    const String name;
    const Bytes publicKey;
};

class ClientListRequest: public Request {
public:
    ClientListRequest(const UUID& clientID, UInt8 version);
};

class GetPublicKeyRequest: public Request {
public:
    GetPublicKeyRequest(const UUID& clientID, UInt8 version, const UUID& otherClientID);
    virtual Bytes pack() const;
private:
    const UUID otherClientID;
};

class SendMessageRequest: public Request {
public:
    SendMessageRequest(const UUID& clientID, UInt8 version, const UUID& toClientID, MessageType messageType, const Bytes& messageContent);
    virtual Bytes pack() const;
private:
    const UUID toClientID;
    const MessageType messageType;
    const Bytes messageContent;
};

class GetMessagesRequest: public Request {
public:
    GetMessagesRequest(const UUID& clientID, UInt8 version);
};
