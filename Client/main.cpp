//
//  Created by Stasel
//

#include <iostream>
#include "Common.h"
#include "Info/ServerInfo.h"
#include "Application/NetworkClient.h"
#include "Application/MainMenu.h"
#include "Application/MessageUApp.h"

#define SERVER_INFO_PATH "server.info"

using namespace std;

const ServerInfo loadServerInfo() {
    try {
        return ServerInfo(SERVER_INFO_PATH);
    }
    catch (exception& ex) {
        cout << "Error: Could not load server info (" << SERVER_INFO_PATH << "): " << ex.what() << "\n";
        exit(EXIT_FAILURE);
    }
}

int main(int argc, const char * argv[]) {
    let serverInfo = loadServerInfo();
    cout << "Server: " << serverInfo << endl;
    MainMenu mainMenu;
    ClientDirectory clientDirectory;
    NetworkClient networkClient(serverInfo);
    MessageUApp app(CLIENT_VERSION, mainMenu, clientDirectory, networkClient);
    app.run();
    
    return 0;
}
