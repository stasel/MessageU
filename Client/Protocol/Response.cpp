//
//  Created by Stasel
//

#include "Response.h"

// MARK: - Response
Response::Response(UInt8 version, ResponseCode code) : version(version), code(code) {}
UInt8 Response::getVersion() const { return version; }
ResponseCode Response::getCode() const { return code; }


// MARK: - RegisterSuccessResponse
RegisterSuccessResponse::RegisterSuccessResponse(UInt8 version, const UUID& clientID)
    : Response(version, RESPONSE_REGISTER_SUCCESS), clientID(clientID) {}

const UUID& RegisterSuccessResponse::getClientID() const {
    return clientID;
}

RegisterSuccessResponse* RegisterSuccessResponse::fromPayload(UInt8 version, const Bytes& payload) {
    if (payload.size() != UUID_SIZE) {
        throw std::runtime_error("Invalid payload size");
    }
    return new RegisterSuccessResponse(version, UUID(payload));
}


// MARK: - ClientListResponse
ClientListResponse::ClientListResponse(UInt8 version, const std::vector<RemoteClient>& clients)
    : Response(version, RESPONSE_CLIENT_LIST), clients(clients) { }
const std::vector<RemoteClient>& ClientListResponse::getClients() const {
    return clients;
}

ClientListResponse* ClientListResponse::fromPayload(UInt8 version, const Bytes& payload) {
    if (payload.size() % (UUID_SIZE + USERNAME_MAX_LENGTH) != 0) {
        throw std::runtime_error("Invalid payload size");
    }
    std::vector<RemoteClient> clientList;
    var it = payload.begin();
    while (it != payload.end()) {
        const Bytes clientID(it, it + UUID_SIZE);
        it += UUID_SIZE;
        String clientName(it, it+ USERNAME_MAX_LENGTH);
        
        // Remove trailing null characters:
        clientName.erase(std::find(clientName.begin(), clientName.end(), '\0'), clientName.end());
        
        it += USERNAME_MAX_LENGTH;
        clientList.push_back(RemoteClient(UUID(clientID), clientName));
    }
    return new ClientListResponse(version, clientList);
}


// MARK: - PublicKeyResponse
PublicKeyResponse::PublicKeyResponse(UInt8 version, const UUID& clientID, const Bytes& publicKey)
    : Response(version, RESPONSE_PUBLIC_KEY), clientID(clientID), publicKey(publicKey) {}
const Bytes& PublicKeyResponse::getPublicKey() const {
    return publicKey;
}

const UUID& PublicKeyResponse::getClientID() const {
    return clientID;
}

PublicKeyResponse* PublicKeyResponse::fromPayload(UInt8 version, const Bytes& payload) {
    if (payload.size() < UUID_SIZE) {
        throw std::runtime_error("Invalid payload size");
    }
    const Bytes clientID(payload.begin(), payload.begin() + UUID_SIZE);
    const Bytes publicKey(payload.begin() + UUID_SIZE, payload.end());
    return new PublicKeyResponse(version, UUID(clientID), publicKey);
}


// MARK: - MessageSentResponse
MessageSentResponse::MessageSentResponse(UInt8 version, const UUID& toClient, UInt32 messageID)
    : Response(version, RESPONSE_MESSAGE_SENT), toClient(toClient), messageID(messageID) { }
const UUID& MessageSentResponse::getToClient() const {
    return toClient;
}

const UInt32 MessageSentResponse::getMessageID() const {
    return messageID;
}

MessageSentResponse* MessageSentResponse::fromPayload(UInt8 version, const Bytes& payload) {
    if (payload.size() != UUID_SIZE + MESSAGE_ID_SIZE) {
        throw std::runtime_error("Invalid payload size");
    }
    const Bytes clientID(payload.begin(), payload.begin() + UUID_SIZE);
    const Bytes messageIDSplitted(payload.begin() + UUID_SIZE, payload.end());
    let messageID = joinBytes32((UInt8*)messageIDSplitted.data());
    return new MessageSentResponse(version, UUID(clientID), messageID);
}


// MARK: - MessageListResponse
MessageListResponse::MessageListResponse(UInt8 version, const std::vector<Message>& messages)
    : Response(version, RESPONSE_MESSAGE_LIST), messages(messages) { }

const std::vector<Message>& MessageListResponse::getMessages() const {
    return messages;
}

MessageListResponse* MessageListResponse::fromPayload(UInt8 version, const Bytes& payload) {
    std::vector<Message> messages;
    var it = payload.begin();
    while (it != payload.end()) {
        const Bytes senderClientID(it, it + UUID_SIZE);
        it += UUID_SIZE;
        let messageID = joinBytes32((UInt8*)Bytes(it, it + MESSAGE_ID_SIZE).data());
        it += MESSAGE_ID_SIZE;
        let messageType = *it;
        it++;
        let messageSize = joinBytes32((UInt8*)Bytes(it, it + 4).data());
        it += 4;
        Bytes messageContent(it, it + messageSize);
        it += messageSize;
        messages.push_back(Message(UUID(senderClientID), messageID, (MessageType)messageType, messageContent));
    }
    return new MessageListResponse(version, messages);
}


// MARK: - ServerErrorResponse
ServerErrorResponse::ServerErrorResponse(UInt8 version): Response(version, RESPONSE_SERVER_ERROR) {}

