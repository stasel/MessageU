//
//  Created by Stasel
//

#include "MessageUApp.h"
#include "../cryptopp_wrapper/RSAWrapper.h"
#include "../cryptopp_wrapper/AESWrapper.h"
#include "../Protocol/Message.h"

#define CLIENT_INFO_PATH "me.info"
#define GET_NAME_MESSAGE "Please enter the name of the client:"
#define CHECK_IS_REGISTERED if (!isRegistered()) { displayUnregisteredMessage(); return; }

using namespace std;

MessageUApp::MessageUApp(const UInt8 version,
                         const MainMenu& mainMenu,
                        ClientDirectory& clientDirectory,
                         const NetworkClient& networkClient)
: version(version), mainMenu(mainMenu), clientDirectory(clientDirectory), networkClient(networkClient) {
    try {
        clientInfo = new ClientInfo(CLIENT_INFO_PATH);
    }
    catch (std::exception&) {
        clientInfo = nullptr;
    }
}

MessageUApp::~MessageUApp() {
    delete clientInfo;
}

void MessageUApp::run() {
    while(true) {
        let selection = mainMenu.prompt();
        cout << endl;
        switch (selection) {
            case MENU_EXIT:                     return;
            case MENU_REGISTER:                 registerFlow();             break;
            case MENU_CLIENT_LIST:              clientListFlow();           break;
            case MENU_REQUEST_PUBLIC_KEY:       requestPublicKeyFlow();     break;
            case MENU_REQUEST_MESSAGES:         requestMessagesFlow();      break;
            case MENU_SEND_MESSAGE:             sendMessageFlow();          break;
            case MENU_REQUEST_SYMMETRIC_KEY:    requestSymmetricKeyFlow();  break;
            case MENU_SEND_SYMMETRIC_KEY:       sendSymmetricKeyFlow();     break;
            case MENU_SEND_FILE:                sendFileFlow();             break;
        }
    }
}

void MessageUApp::registerFlow() {
    // Do not allow register again if user has already been registered
    if (isRegistered()) {
        cout << "[!]  This client is already registered as '" << clientInfo->getName() << "'.\n";
        cout << "     If you wish to re-register, please remove the client info file: '" << CLIENT_INFO_PATH << "'\n";
        return;
    }
    
    // Get client name
    let message = "Please enter your name. (Must be 1 to " + to_string(USERNAME_MAX_LENGTH) + " ascii only characters)\n";
    String clientName = mainMenu.promptUserName(message);
    
    // Generate RSA key pair
    RSAPrivateWrapper rsaPrivate;
    let publicKey = stringToBytes(rsaPrivate.getPublicKey());
    Bytes privateKey = stringToBytes(rsaPrivate.getPrivateKey());
    
    
    // Send register request
    let request = RegisterRequest(version, clientName, publicKey);
    try {
        let response = networkClient.registerClient(request);
        clientInfo = new ClientInfo(clientName, response.getClientID(), privateKey);
        clientInfo->persist(CLIENT_INFO_PATH);
        cout << "[SUCCESS] Register successful.\n\tYour Client ID: " << response.getClientID().hex() << "\n\tClient name: " << clientName;
    }
    catch (std::exception& ex) {
        cout << "[ERROR]  Register failed: " << ex.what();
    }
}

void MessageUApp::clientListFlow() {
    CHECK_IS_REGISTERED;
    
    try {
        // Send request
        let request = ClientListRequest(clientInfo->getID(), version);
        let response = networkClient.getClientList(request);
        cout << "---------- List of clients (" << std::to_string(response.getClients().size()) << ") ----------\n";
        
        // Display all clients and save them in the directory
        for(let& client: response.getClients()) {
            cout << client.getName() << "\n";
            if (!clientDirectory.isClientIDExists(client.getID())) {
                clientDirectory.addClient(client.getID(), client.getName());
            }
            else {
                // Update client name if exists - Useful if we receive symmetric key from unknown client name
                clientDirectory.setClientName(client.getID(), client.getName());
            }
        }
    }
    catch (std::exception& ex) {
        cout << "[ERROR]  Client list request failed: " << ex.what();
    }
}



void MessageUApp::requestPublicKeyFlow() {
    CHECK_IS_REGISTERED;

    // Input username from the client
    String clientName = mainMenu.promptUserName(GET_NAME_MESSAGE);
    
    // Send request and save public key
    try {
        let remoteClientID = clientDirectory.getClientIDByName(clientName);
        let request = GetPublicKeyRequest(clientInfo->getID(), version, remoteClientID);
        let response = networkClient.getPublicKey(request);
        clientDirectory.setPublicKey(response.getClientID(), response.getPublicKey());
        cout << "[SUCCESS] Public key for '" << clientName << "' has been fetched successfully and saved locally.\n";
    }
    catch (std::exception& ex) {
        cout << "[ERROR]  Get public key request failed: " << ex.what();
    }
}

void MessageUApp::requestMessagesFlow() {
    CHECK_IS_REGISTERED;
    try {
        let request = GetMessagesRequest(clientInfo->getID(), version);
        let response = networkClient.getPendingMessages(request);
        cout << "New messages (" << to_string(response.getMessages().size()) << ")\n-----------------------------\n";
        for(let& message: response.getMessages()) {
            handleIncomingMessage(message);
        }
    }
    catch (std::exception& ex) {
        cout << "[ERROR]  Get pending messages request failed: " << ex.what();
    }
}

void MessageUApp::sendMessageFlow() {
    CHECK_IS_REGISTERED;
    
    // Get client name
    String recipientName = mainMenu.promptUserName(GET_NAME_MESSAGE);
    if (!clientDirectory.isClientNameExists(recipientName)) {
        cout << "[ERROR]  Could not find recipient name";
        return;
    }
    
    // Get symmetric key
    let recipientID = clientDirectory.getClientIDByName(recipientName);
    let key = clientDirectory.getSymmetricKey(recipientID);
    if (key.empty()) {
        cout << "[ERROR]  Could not send message: Missing symmetric key with the other client";
        return;
    }
    
    // Get message:
    String message;
    cout << "Please enter a message:\n";
    while(message.empty()) {
        cout << PROMPT;
        getline(cin, message);
    }

    try {
        // encrypt message:
        AESWrapper aes(key.data(), (UInt32)key.size());
        let cipther = aes.encrypt(message.c_str(), (UInt32)message.size());
        let request = SendMessageRequest(clientInfo->getID(), version, recipientID, MESSAGE_TYPE_TEXT, stringToBytes(cipther));
        let response = networkClient.sendMessage(request);
        cout << "[SUCCESS] Message successfully sent to " << recipientName << "\n";
    }
    catch (std::exception& ex) {
        cout << "[ERROR]  Send message failed: " << ex.what();
    }
}

void MessageUApp::requestSymmetricKeyFlow() {
    CHECK_IS_REGISTERED;
    
    // Get client name
    String recipientName = mainMenu.promptUserName(GET_NAME_MESSAGE);
    if (!clientDirectory.isClientNameExists(recipientName)) {
        cout << "[ERROR]  Could not find client";
        return;
    }
    
    // Send Symmetric key request
    try {
        let recipientID = clientDirectory.getClientIDByName(recipientName);
        let request = SendMessageRequest(clientInfo->getID(), version, recipientID, MESSAGE_TYPE_SYMMETRIC_KEY_REQUEST, Bytes());
        let response = networkClient.sendMessage(request);
        cout << "Successfully Sent symmetric key request to " << recipientName << "\n";
    }
    catch (std::exception& ex) {
        cout << "[ERROR]  Symmetric key request failed: " << ex.what();
    }
}

void MessageUApp::sendSymmetricKeyFlow() {
    CHECK_IS_REGISTERED;
    
    // Get client name
    String recipientName = mainMenu.promptUserName(GET_NAME_MESSAGE);
    if (!clientDirectory.isClientNameExists(recipientName)) {
        cout << "[ERROR] Could not find client";
        return;
    }
    
    // Get public key - we will use it to encrypt the symmetric key
    let recipientID = clientDirectory.getClientIDByName(recipientName);
    let recipientPublicKey = clientDirectory.getPublicKey(recipientID);
    if (recipientPublicKey.empty()) {
        cout << "[ERROR] Could not find saved public key for '" << recipientName << "'. Please request one and try again.";
        return;
    }
    
    // Generate AES key and encrypt it with the recepient's public key
    var key = AESWrapper::GenerateKey();
    RSAPublicWrapper rsaPublic(bytesToString(recipientPublicKey));
    let encryptedKey = rsaPublic.encrypt(bytesToString(key));
    
    // Save symmetric key for the client
    clientDirectory.setSymmetricKey(recipientID, key);
    key.clear();
    
    // Send encrypted Symmetric key. This will override any existing key
    try {
        let request = SendMessageRequest(clientInfo->getID(), version, recipientID, MESSAGE_TYPE_SYMMETRIC_KEY, stringToBytes(encryptedKey));
        let response = networkClient.sendMessage(request);
        cout << "Successfully sent symmetric key to " << recipientName << "\n";
    }
    catch (std::exception& ex) {
        cout << "[ERROR] Sending symmetric key failed: " << ex.what();
    }
}

void MessageUApp::sendFileFlow() {
    CHECK_IS_REGISTERED;

    // Get client name
    String recipientName = mainMenu.promptUserName(GET_NAME_MESSAGE);
    if (!clientDirectory.isClientNameExists(recipientName)) {
        cout << "[ERROR]  Could not find recipient name";
        return;
    }
    
    // Get symmetric key
    let recipientID = clientDirectory.getClientIDByName(recipientName);
    let key = clientDirectory.getSymmetricKey(recipientID);
    if (key.empty()) {
        cout << "[ERROR]  Could not send file: Missing symmetric key with the other client";
        return;
    }
    
    // Get file path:
    String filePath;
    cout << "Please enter the full path of the file to send:\n";
    while(filePath.empty()) {
        cout << PROMPT;
        getline(cin, filePath);
    }
    
    try {
        // Read file
        var fileContents = readFile(filePath);
        
        // encrypt file:
        AESWrapper aes(key.data(), (UInt32)key.size());
        var encryptedFile = aes.encrypt(bytesToString(fileContents).c_str(), (UInt32)fileContents.size());
        
        // Send encrypted file
        let request = SendMessageRequest(clientInfo->getID(), version, recipientID, MESSAGE_TYPE_FILE, stringToBytes(encryptedFile));
        let response = networkClient.sendMessage(request);
        cout << "[SUCCESS] File successfully sent to " << recipientName << "\n";
        fileContents.clear();
        encryptedFile.clear();
    }
    catch (std::exception& ex) {
        cout << "[ERROR] Send File failed: " << ex.what();
    }
}


bool MessageUApp::isRegistered() const {
    return clientInfo != nullptr;
}

void MessageUApp::displayUnregisteredMessage() const {
    cout << "[!]  Cannot complete operation: This client is not yet registered.\n";
    cout << "Please register first with the menu option '" << std::to_string(MENU_REGISTER) << "'\n";
}

void MessageUApp::handleIncomingMessage(const Message& message) {
    // Get sender name
    String senderName;
    if (clientDirectory.isClientIDExists(message.getSenderClientID())) {
        senderName = clientDirectory.getClientName(message.getSenderClientID());
    }
    else {
        // Cannot get sender name from directoy, show partial Client ID for security reasons
        senderName = "Unknown (ID " + message.getSenderClientID().hex().substr(0,8) + ")";
    }
    
    String content;
    switch (message.getType()) {
        case MESSAGE_TYPE_TEXT:
            try {
                content = bytesToString(decryptMessageContent(message));
            }
            catch (std::exception& ex) {
                content = "<<[!] Can't decrypt message (" + String(ex.what()) + ")>>";
            }
            break;
        case MESSAGE_TYPE_SYMMETRIC_KEY_REQUEST:
            content = "<<Request for symmetric key>>";
            break;
        case MESSAGE_TYPE_SYMMETRIC_KEY: {
            handleIncomingSymmetricKey(message);
            content = "<<Symmetric key recieved>>";
            break;
        }
        case MESSAGE_TYPE_FILE:
            content = handleIncomingFile(message);
            break;
        default:
            content = "[!] Could not read message: Unknown Message type " + to_string(message.getType());
            break;
    }
    cout << "From: " << senderName << endl << "Content:\n" << content << "\n-----<EOM>-----\n";
}

Bytes MessageUApp::decryptMessageContent(const Message& message) const {
    // Check if we have shared symmetric key
    let symmetricKey = clientDirectory.getSymmetricKey(message.getSenderClientID());
    if (symmetricKey.empty()) {
        throw std::runtime_error("Missing symmetric key");
    }
    
    // Try to decrypt message with symmetric key
    AESWrapper aes(symmetricKey.data(), (UInt32)symmetricKey.size());
    let decrypted = aes.decrypt((char*)message.getContent().data(), (UInt32)message.getContent().size());
    return stringToBytes(decrypted);
}

// Decrypt and add symmetric key to client directory.
void MessageUApp::handleIncomingSymmetricKey(const Message& message) {
    if (!clientDirectory.isClientIDExists(message.getSenderClientID())) {
        // We don't have the name yet so we will use the client ID as a placeholder
        clientDirectory.addClient(message.getSenderClientID(), message.getSenderClientID().hex());
    }
    try {
        // Decrypt symmetric key with this user's private key.
        let encryptedKey = message.getContent();
        RSAPrivateWrapper rsaPrivate(bytesToString(clientInfo->getPrivateKey()));
        let key = rsaPrivate.decrypt(bytesToString(encryptedKey));
        // Save symmetric key in the directory
        clientDirectory.setSymmetricKey(message.getSenderClientID(), stringToBytes(key));
    }
    catch (std::exception&) {
        // decryption fails - do nothing
    }
}

String MessageUApp::handleIncomingFile(const Message& message) {
    let tempFile = tempFilePath();
    try {
        let decryptedFile = decryptMessageContent(message);
        writeFile(tempFile, decryptedFile);
        return "<< FILE: " + tempFile + " >>";
    }
    catch (std::exception& ex) {
        return "<<[!] Received file but can't save or read it (" + String(ex.what()) + ")>>";
    }
}
