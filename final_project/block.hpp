#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <iostream>
#include <iomanip>

#define BLOCK_SIZE 64
#define BYTES_PER_ROW 32

typedef uint8_t byte;

class Block {
    public:
        byte bytes[BLOCK_SIZE];

        void set_to_zero();
        bool is_nonzero();
        void print();
};

#endif