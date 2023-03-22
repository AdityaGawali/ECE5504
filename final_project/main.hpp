#ifndef MAIN_HPP
#define MAIN_HPP

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <iostream>
#include <iomanip>

#define PAGE_SIZE 4096
#define BYTES_PER_ROW 32

typedef uint8_t byte;

void process_dump_file(char*);

class Page {
    public:
        byte data[PAGE_SIZE];

        void set_to_zero();
        bool is_nonzero();
        void print();
};

#endif