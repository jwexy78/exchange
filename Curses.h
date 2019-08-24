#pragma once

#include <curses.h>

class Curses
{
  public:
    Curses();
    ~Curses();
    Curses(const Curses&) = delete;
    Curses& operator=(const Curses&) = delete;

    void clear();
    void refresh() const;
    void drawString(const std::string& str,
                    int x, int y);
  private:
    WINDOW* _window;
};

Curses::Curses()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    int h, w;
   getmaxyx(stdscr, h, w);

    _window = newwin(w, h, 0, 0);
}

Curses::~Curses()
{
    delwin(_window);
    endwin();
}

void Curses::clear()
{
    wclear(_window);
}

void Curses::drawString(const std::string& str,
                        int x, int y)
{
    wmove(_window, y, x);
    waddstr(_window, str.c_str());
}

void Curses::refresh() const
{
    // int height, width;
    // getmaxyx(stdscr, h, w);
    wmove(_window, 0, 0);
    wrefresh(_window);
}