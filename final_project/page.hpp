#ifndef PAGE_HPP
#define PAGE_HPP

#include <iostream>
#include <iomanip>

#define PAGE_SIZE 4096
#define BYTES_PER_ROW 32

typedef uint8_t byte;

class Page {
    public:
        byte data[PAGE_SIZE];

        void set_to_zero();
        bool is_nonzero();
        void print();
};

#endif