//
//  Created by Stasel
//

#include "MainMenu.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace std;

MainMenu::MainMenu() {
    menuItems.push_back(MenuItem { MENU_REGISTER, "Register" });
    menuItems.push_back(MenuItem { MENU_CLIENT_LIST, "Request for clients list" });
    menuItems.push_back(MenuItem { MENU_REQUEST_PUBLIC_KEY, "Request for public key" });
    menuItems.push_back(MenuItem { MENU_REQUEST_MESSAGES, "Request for pending messages" });
    menuItems.push_back(MenuItem { MENU_SEND_MESSAGE, "Send a text message" });
    menuItems.push_back(MenuItem { MENU_REQUEST_SYMMETRIC_KEY, "Send a request for symmetric key" });
    menuItems.push_back(MenuItem { MENU_SEND_SYMMETRIC_KEY, "Send your symmetric key" });
    menuItems.push_back(MenuItem { MENU_SEND_FILE, "Send file" });
    menuItems.push_back(MenuItem { MENU_EXIT, "Exit client" });
}

const String MainMenu::buildPromptMessage() const {
    String message = "\n--------------------------------------------------------------------------------\n";
    message += "MessageU client at your service.\n\n";
    for(let &item: menuItems) {
        message += to_string(item.choice) + ") " + item.text + "\n";
    }
    message += "? ";
    return message;
}

MenuChoice MainMenu::prompt() const {
    const String message = buildPromptMessage();
    cout << message;

    while (true) {
        String stringInput;
        getline(cin, stringInput);
        try {
            UInt32 input = stoi(stringInput);
            let item = find_if(menuItems.begin(), menuItems.end(), [input](MenuItem item) {
                return item.choice == input;
            });
            if (item != menuItems.end()) {
                return (*item).choice;
            }
            else {
                throw invalid_argument("Invalid option");
            }
        }
        catch (exception&) {
            cout << "Invalid option. Please enter a number from the list above\n? ";
        }
    }
}

String MainMenu::promptUserName(const String &message) const {
    String name;
    cout << message << endl;
    UInt32 retries = 0;
    while(retries < 100) {
        retries++;
        cout << PROMPT;
        getline(cin, name);
        boost::trim(name);
        if (name.empty()) {
            continue;
        }
        
        if (name.length() > USERNAME_MAX_LENGTH) {
            cout << "Client name is too long.\n" << PROMPT;
            continue;
        }
        
        if (!isAsciiOnly(name)) {
            cout << "Client name contains non ascii characters.\n" << PROMPT;
            continue;
        }
        return name;
    }
    // max retries reached
    return "";
}

