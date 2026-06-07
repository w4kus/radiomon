// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <vector>
#include <functional>
#include <string>

#pragma once

namespace util {

class menu
{
public:
    using menu_cb = bool(const std::string &input);

    struct entry
    {
        entry() : key { 0 } { }

        char key;
        const char *prompt;
        const char *description;
        std::function<menu_cb> handler;
    };

    menu();

    menu(const char *s);

    void display();

    void add(entry &e);

    void set_help(bool on) { handleHelp = on; }

    void processInput();

private:

    const char *title;
    int m_ActiveEntry;
    bool waitForNewline;
    bool handleHelp;

    std::vector<entry> m_Menu;
    std::string m_CurrentInput;
};

}
