#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <string>
#include <locale.h>
#include <ncurses.h>
#include <vector>

#include "functions.h"


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    clear_log();
    if (argc < 2) exit(0);
    for (int64_t i = 0; i < argc; i++) {
        if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
            std::cout << "USAGE: sim [OPTIONS] [FILE]                      \n";
            std::cout << "                                                 \n";
            std::cout << "General Options:                                 \n";
            std::cout << " -h, --help               Prints this information\n";
            std::cout << "                                                 \n";
            std::cout << "For more information: https://github.com/sz47/sim\n";
            exit(0);
        }
    }

    std::ifstream ifs;
    ifs.open(argv[1]);
    std::stringstream strs;
    strs << ifs.rdbuf();
    std::string file_txt = strs.str();
    ifs.close();
    if (file_txt == "") {
        std::cout << "The file dosen't exists or is empty" << std::endl;
        exit(0);
    }

    std::vector <std::pair <int, std::string>> buff_text;
    std::vector <int> buff_text_lines;
    std::vector <int> buff_text_width;
    
    int index = 0;
    int length = 0;
    int lines = 0;
    for (int64_t i = 0; i < file_txt.length(); i++) {
        if (file_txt[i] == '\n') {
            length = i - index+1;
            lines++;
            buff_text.push_back({lines, file_txt.substr(index, length)});
            index = i+1;
        }
    }


    initscr();    
    start_color();
    init_color(COLOR_BLACK, 101, 105, 148);

    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_BLUE,    COLOR_BLACK);
    init_pair(3, COLOR_GREEN,   COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(6, COLOR_CYAN,    COLOR_BLACK);

    int window_x, window_y;
    getmaxyx(stdscr, window_y, window_x);

    for (auto x: buff_text) {
        buff_text_width.push_back(x.second.length());
        buff_text_lines.push_back(x.second.length()/(window_x-4));
        if (x.second.length()%(window_x-4) != 0) 
            buff_text_lines[buff_text_lines.size()-1]++;
    }

    std::vector <int> THICCNESS(window_y -2);
    std::vector <int> LINEON(window_y-2);

    Cursor cur;
    cur.line  = 0, cur.pos  = 4;
    cur.vline = 0, cur.vpos = 0;
    cur.oline = 0, cur.opos = 0;

    cur.mode = 'n';
    cur.vf = 0, cur.vl = 0;
    cur.maxx = 0;


    bool needed_text = true, 
         needed_line = true,
         needed_curs = true;

    while (true) {
        getmaxyx(stdscr, window_y, window_x);
        std::vector <std::pair <int, std::string>> 
            buff_text_temp(buff_text.begin() + cur.vf, buff_text.end());

        if (needed_text) {
            print_text(window_y-2, window_x, buff_text_temp, THICCNESS, LINEON, cur);
        }
        if (needed_line) {
            int percent = (window_y*100)/buff_text.size();
            print_line(window_y-2, window_x, cur.mode, percent);
        }
        if (needed_curs) {
            print_curs(cur, window_x, window_y);
        }

        move(window_y-1, 2);
        char input_char = getch();
        int parsing_nes = parse_command(
                input_char, cur, window_x, window_y, 
                buff_text.size(), THICCNESS, LINEON, buff_text_lines, buff_text_width
            );
//        needed_line = false;
//        needed_text = false;
//        needed_curs = false;
//        if (parsing_nes%5 == 0) {
//            needed_text = true;
//        }
//        if (parsing_nes%3 == 0) {
//            needed_line = true;
//        }
//        if (parsing_nes%2 == 0) {
//            needed_curs = true;
//        }
    }
    endwin();
}
