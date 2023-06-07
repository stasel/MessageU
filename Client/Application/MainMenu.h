//
//  Created by Stasel
//

#pragma once

#include <vector>
#include "../Common.h"

enum MenuChoice {
    MENU_EXIT = 0,
    MENU_REGISTER = 10,
    MENU_CLIENT_LIST = 20,
    MENU_REQUEST_PUBLIC_KEY = 30,
    MENU_REQUEST_MESSAGES = 40,
    MENU_SEND_MESSAGE = 50,
    MENU_REQUEST_SYMMETRIC_KEY = 51,
    MENU_SEND_SYMMETRIC_KEY = 52,
    MENU_SEND_FILE = 53
};

struct MenuItem {
    const MenuChoice choice;
    const String text;
};

// This class in charge of displaying the main menu and reading user's selection
class MainMenu {
public:
    MainMenu();
    MenuChoice prompt() const;
    String promptUserName(const String &message) const;
private:
    std::vector<MenuItem> menuItems;
    const String buildPromptMessage() const;
};
