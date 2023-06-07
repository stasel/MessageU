//
//  Created by Stasel
//

#pragma once
#include "../Common.h"
#include "../Utilities/UUID.h"
#include "Message.h"
#include "RemoteClient.h"

enum ResponseCode {
    RESPONSE_REGISTER_SUCCESS = 2000,
    RESPONSE_CLIENT_LIST = 2001,
    RESPONSE_PUBLIC_KEY = 2002,
    RESPONSE_MESSAGE_SENT = 2003,
    RESPONSE_MESSAGE_LIST = 2004,
    RESPONSE_SERVER_ERROR = 9000
};

class Response {
public:
    Response(UInt8 version, ResponseCode code);
    virtual ~Response() = default;
    UInt8 getVersion() const;
    ResponseCode getCode() const;
private:
    const UInt8 version;
    const ResponseCode code;
};

class RegisterSuccessResponse: public Response {
public:
    RegisterSuccessResponse(UInt8 version, const UUID& clientID);
    const UUID& getClientID() const;
    static RegisterSuccessResponse* fromPayload(UInt8 version, const Bytes& payload);
private:
    const UUID clientID;
};

class ClientListResponse: public Response {
public:
    ClientListResponse(UInt8 version, const std::vector<RemoteClient>& clients);
    const std::vector<RemoteClient>& getClients() const;
    static ClientListResponse* fromPayload(UInt8 version, const Bytes& payload);
private:
    const std::vector<RemoteClient> clients;
};

class PublicKeyResponse: public Response {
public:
    PublicKeyResponse(UInt8 version, const UUID& clientID, const Bytes& publicKey);
    const UUID& getClientID() const;
    const Bytes& getPublicKey() const;
    static PublicKeyResponse* fromPayload(UInt8 version, const Bytes& payload);
private:
    const UUID clientID;
    const Bytes publicKey;
};

class MessageSentResponse: public Response {
public:
    MessageSentResponse(UInt8 version, const UUID& toClient, UInt32 messageID);
    const UUID& getToClient() const;
    const UInt32 getMessageID() const;
    static MessageSentResponse* fromPayload(UInt8 version, const Bytes& payload);
private:
    const UUID toClient;
    const UInt32 messageID;
};

class MessageListResponse: public Response {
public:
    MessageListResponse(UInt8 version, const std::vector<Message>& messages);
    const std::vector<Message>& getMessages() const;
    static MessageListResponse* fromPayload(UInt8 version, const Bytes& payload);
private:
    const std::vector<Message> messages;
};

class ServerErrorResponse: public Response {
public:
    ServerErrorResponse(UInt8 version);
};
