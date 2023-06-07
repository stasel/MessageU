//
//  Created by Stasel
//

#pragma once
#include "../Common.h"
#include "NetworkClient.h"
#include "ClientDirectory.h"
#include "MainMenu.h"
#include "../Info/ClientInfo.h"
#include "../Protocol/Message.h"

// MessageUApp class contains the client logic. It uses all it's dependencies and combines them together
class MessageUApp {
public:
    MessageUApp(const UInt8 version, const MainMenu& mainMenu, ClientDirectory& clientDirectory, const NetworkClient& networkClient);
    ~MessageUApp();
    void run();
private:
    const UInt8 version;
    const MainMenu& mainMenu;
    ClientDirectory& clientDirectory;
    const NetworkClient& networkClient;
    ClientInfo *clientInfo;
    
    // Flows
    void registerFlow();
    void clientListFlow();
    void requestPublicKeyFlow();
    void requestMessagesFlow();
    void sendMessageFlow();
    void requestSymmetricKeyFlow();
    void sendSymmetricKeyFlow();
    void sendFileFlow();
    
    // Helper methods
    bool isRegistered() const;
    void displayUnregisteredMessage() const;
    void handleIncomingMessage(const Message& message);
    Bytes decryptMessageContent(const Message& message) const;
    void handleIncomingSymmetricKey(const Message& message);
    String handleIncomingFile(const Message& message);
};
