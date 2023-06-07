//
//  Created by Stasel
//

#include "Request.h"

// MARK - Request
Request::Request(const UUID& clientID, UInt8 version, RequestCode code)
    : clientID(clientID), version(version), code(code) { }

Bytes Request::pack() const {
    return pack({});
}

Bytes Request::pack(const Bytes& payload) const {
    // Pack header
    Bytes packed = clientID.getBytes();
    packed.push_back(version);
    let packedCode = splitBytes16(code);
    packed.insert(packed.end(), packedCode.begin(), packedCode.end());
    
    // Pack payload
    let packedPayloadSize = splitBytes32((UInt32)payload.size());
    packed.insert(packed.end(), packedPayloadSize.begin(), packedPayloadSize.end());
    packed.insert(packed.end(), payload.begin(), payload.end());
    return packed;
}

std::ostream& operator<<(std::ostream& os, const Request& request) {
    os << "[Request] Code=" << request.code;
    return os;
}

// MARK - RegisterRequest
RegisterRequest::RegisterRequest(UInt8 version, const String& name, const Bytes& publicKey)
    : Request(UUID(), version, REQUEST_REGISTER), name(name), publicKey(publicKey) { }

Bytes RegisterRequest::pack() const {
    Bytes payload;
    let sanitizedName = name.substr(0, USERNAME_MAX_LENGTH - 1);
    std::copy(sanitizedName.begin(), sanitizedName.end(), std::back_inserter(payload));
    payload.resize(USERNAME_MAX_LENGTH, 0);
    payload.insert(payload.end(), publicKey.begin(), publicKey.end());
    return Request::pack(payload);
}

// MARK - ClientListRequest
ClientListRequest::ClientListRequest(const UUID& clientID, UInt8 version)
    : Request(clientID, version, REQUEST_CLIENT_LIST) { }

// MARK - GetPublicKeyRequest
GetPublicKeyRequest::GetPublicKeyRequest(const UUID& clientID, UInt8 version, const UUID& otherClientID)
    : Request(clientID, version, REQUEST_GET_PUBLIC_KEY), otherClientID(otherClientID) { }
Bytes GetPublicKeyRequest::pack() const {
    Bytes payload = otherClientID.getBytes();
    return Request::pack(payload);
}

// MARK - SendMessageRequest
SendMessageRequest::SendMessageRequest(const UUID& clientID, UInt8 version, const UUID& toClientID, MessageType messageType, const Bytes& messageContent)
    : Request(clientID, version, REQUEST_SEND_MESSAGE), toClientID(toClientID), messageType(messageType), messageContent(messageContent) { }
Bytes SendMessageRequest::pack() const {
    Bytes payload = toClientID.getBytes();
    payload.push_back(messageType);
    let contentSize = splitBytes32((UInt32)messageContent.size());
    payload.insert(payload.end(), contentSize.begin(), contentSize.end());
    payload.insert(payload.end(), messageContent.begin(), messageContent.end());
    return Request::pack(payload);
}

// MARK - GetMessagesRequest
GetMessagesRequest::GetMessagesRequest(const UUID& clientID, UInt8 version)
    : Request(clientID, version, REQUEST_GET_MESSAGES) { }

