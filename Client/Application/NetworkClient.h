//
//  Created by Stasel
//

#pragma once
#include <boost/asio.hpp>
#include "../Common.h"
#include "../Protocol/Request.h"
#include "../Protocol/Response.h"
#include "../Info/ServerInfo.h"

// NetworkClient is a stateless class that in charge of sending requests and receiving responses with no further logic
class NetworkClient {
public:
    NetworkClient(const ServerInfo& serverInfo);
    RegisterSuccessResponse registerClient(const RegisterRequest& request) const;
    ClientListResponse getClientList(const ClientListRequest& request) const;
    PublicKeyResponse getPublicKey(const GetPublicKeyRequest& request) const;
    MessageSentResponse sendMessage(const SendMessageRequest& request) const;
    MessageListResponse getPendingMessages(const GetMessagesRequest& request) const;
private:
    const ServerInfo serverInfo;
    Response* sendRequest(const Request& request) const;
    Response* parseResponse(boost::asio::ip::tcp::socket socket) const;
};
