// Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
//
// Licensed under the MIT License - see LICENSE file for details.

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "menu.h"

using namespace util;

menu::menu() : title { "\n\nTest Menu"},
                m_ActiveEntry { -1 }, waitForNewline { false }, handleHelp { false }
{
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

menu::menu(const char *s) : title { s },
                m_ActiveEntry { -1 }, waitForNewline { false }, handleHelp { false }
{
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void menu::display()
{
    printf("%s\n\n", title);

    for (size_t i=0;i < m_Menu.size();i++)
        printf("%c : %s\n", m_Menu[i].key, m_Menu[i].description);
}

void menu::add(entry &e)
{
    m_Menu.push_back(e);
}

void menu::processInput()
{
    auto c = getchar();

    if (c == EOF)
        return;

    if (m_ActiveEntry > -1)
    {
        if (c != '\n')
        {
            // accumulate the input to a menu item
            m_CurrentInput += static_cast<char>(c);
        }
        else if (waitForNewline)
        {
            // skips the newline associated with selecting a menu item
            waitForNewline = false;
            return;
        }
        else
        {
            // input entered, proccess it
            if (m_Menu[m_ActiveEntry].handler(m_CurrentInput))
                m_ActiveEntry = -1;

            // else the menu item wants to stay active...

            m_CurrentInput.clear();
        }
    }
    else
    {
        // skips the newline associated with a data-less menu item - generally for items which
        // only display information (signified with a null prompt)
        if (c == '\n') return;

        if (handleHelp && (c == 'h'))
        {
            display();
            return;
        }

        for (size_t i=0;i < m_Menu.size();i++)
        {
            if (m_Menu[i].key == c)
            {
                // If no prompt, then assume it just prints stuff out and that's it. No need to
                // set it as active.
                if (m_Menu[i].prompt)
                {
                    waitForNewline = true;
                    m_ActiveEntry = i;
                    printf("%s> ", m_Menu[i].prompt);
                }
                else
                    m_Menu[i].handler(m_CurrentInput);

                break;
            }
        }
   }

}