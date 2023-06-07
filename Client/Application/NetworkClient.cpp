//
//  Created by Stasel
//

#include "NetworkClient.h"

#define BUFFER_SIZE 4096

using boost::asio::ip::tcp;

NetworkClient::NetworkClient(const ServerInfo& serverInfo) : serverInfo(serverInfo) { }

RegisterSuccessResponse NetworkClient::registerClient(const RegisterRequest& request) const {
    let response = sendRequest(request);
    if (response->getCode() == RESPONSE_SERVER_ERROR) {
        throw std::runtime_error("Server responded with an error");
    }
    RegisterSuccessResponse stackResponse = *(RegisterSuccessResponse*)response;
    delete response;
    return stackResponse;
}

ClientListResponse NetworkClient::getClientList(const ClientListRequest& request) const {
    let response = sendRequest(request);
    if (response->getCode() == RESPONSE_SERVER_ERROR) {
        throw std::runtime_error("Server responded with an error");
    }
    ClientListResponse stackResponse = *(ClientListResponse*)response;
    delete response;
    return stackResponse;
}

PublicKeyResponse NetworkClient::getPublicKey(const GetPublicKeyRequest& request) const {
    let response = sendRequest(request);
    if (response->getCode() == RESPONSE_SERVER_ERROR) {
        throw std::runtime_error("Server responded with an error");
    }
    PublicKeyResponse stackResponse = *(PublicKeyResponse*)response;
    delete response;
    return stackResponse;
}

MessageSentResponse NetworkClient::sendMessage(const SendMessageRequest& request) const {
    let response = sendRequest(request);
    if (response->getCode() == RESPONSE_SERVER_ERROR) {
        throw std::runtime_error("Server responded with an error");
    }
    MessageSentResponse stackResponse = *(MessageSentResponse*)response;
    delete response;
    return stackResponse;
}

MessageListResponse NetworkClient::getPendingMessages(const GetMessagesRequest& request) const {
    let response = sendRequest(request);
    if (response->getCode() == RESPONSE_SERVER_ERROR) {
        throw std::runtime_error("Server responded with an error");
    }
    MessageListResponse stackResponse = *(MessageListResponse*)response;
    delete response;
    return stackResponse;
}

Response* NetworkClient::sendRequest(const Request &request) const {
    // Connect
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve(serverInfo.getHost(), std::to_string(serverInfo.getPort())));

    // Send request in chunks
    let packedRequest = request.pack();
    UInt32 sent = 0;
    while (sent < packedRequest.size()) {
        let bytesToRead = std::min((UInt32)BUFFER_SIZE, (UInt32)(packedRequest.size() - sent));
        let chunk = Bytes(packedRequest.begin() + sent, packedRequest.begin() + sent + bytesToRead);
        boost::asio::write(socket, boost::asio::buffer(chunk));
        sent += bytesToRead;
    }
    
    // Receive response
    let response = parseResponse(std::move(socket));
    socket.close();
    return response;
};

Response* NetworkClient::parseResponse(boost::asio::ip::tcp::socket socket) const {
    UInt8 response[BUFFER_SIZE] = { 0 };
    boost::system::error_code errorCode;
    size_t readSize = boost::asio::read(socket, boost::asio::buffer(response, BUFFER_SIZE), errorCode);
    
    if (errorCode && errorCode != boost::asio::error::eof) {
        throw std::runtime_error("Error receiving data from server. Code " + errorCode.message());
    }
    
    const UInt8 headerSize = VERSION_SIZE_BYTES + RESPONSE_CODE_SIZE_BYTES + PAYLOAD_SIZE_BYTES;
    if (readSize < headerSize) {
        throw std::runtime_error("Server response is too short");
    }
    
    // Read header
    UInt8 version = response[0];
    UInt8 splittedCode[2] = { response[1], response[2] };
    UInt16 code = joinBytes16(splittedCode);
    UInt8 splittedPayloadSize[4] = { response[3], response[4], response[5], response[6] };
    UInt32 payloadSize = joinBytes32(splittedPayloadSize);
    
    // Get all payload in chunks
    Bytes payload(response + headerSize, response + readSize);
    while (payload.size() < payloadSize) {
        // Reset old response buffer
        memset(response, 0, BUFFER_SIZE * sizeof(response[0]));
        // Read from socket to buffer
        readSize = boost::asio::read(socket, boost::asio::buffer(response, BUFFER_SIZE), errorCode);
        // Check for errors
        if (errorCode && errorCode != boost::asio::error::eof) {
            throw std::runtime_error("Error receiving data from server. Code " + errorCode.message());
        }
        // Create Byte vector from buffer, limit the size up to `payloadSize` and ignore other
        const Bytes chunk(response, response + std::min(payloadSize - payload.size(),readSize));
        // Add chunk to the full payload array
        payload.insert(payload.end(), chunk.begin(), chunk.end());
        // Stop receiving if didn't read anything to avoid infinite loops
        if(readSize == 0) {
            break;
        }
    }
    
    // Parse and return payload
    switch (code) {
        case RESPONSE_REGISTER_SUCCESS: return RegisterSuccessResponse::fromPayload(version, payload);
        case RESPONSE_CLIENT_LIST:      return ClientListResponse::fromPayload(version, payload);
        case RESPONSE_PUBLIC_KEY:       return PublicKeyResponse::fromPayload(version, payload);
        case RESPONSE_MESSAGE_SENT:     return MessageSentResponse::fromPayload(version, payload);
        case RESPONSE_MESSAGE_LIST:     return MessageListResponse::fromPayload(version, payload);
        case RESPONSE_SERVER_ERROR:     return new ServerErrorResponse(version);
        default:
            throw std::runtime_error("Unknown response code");
    }
}
