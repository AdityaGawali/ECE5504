#ifndef DUMP_HPP
#define DUMP_HPP

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "page.hpp"

class Dump {
    public:
        char* filename;
        std::vector<Page> pages;

        Dump(char* filename);
        void process();
};

#endif