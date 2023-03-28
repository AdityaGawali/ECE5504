#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <iostream>
#include <iomanip>

const unsigned int BLOCK_SIZE = 64;
const unsigned int WORD_SIZE = 4;
const unsigned int WORD_SIZE_BITS = WORD_SIZE * 8;
const unsigned int WORDS_PER_BLOCK = BLOCK_SIZE / WORD_SIZE;
const unsigned int BYTES_PER_ROW = 32;

typedef uint8_t byte;

struct _Word {
    byte bytes[WORD_SIZE];
};
typedef _Word Word;

class Block {
    public:
        Word words[WORDS_PER_BLOCK];

        void set_to_zero();
        bool is_nonzero();
        void print();
};

unsigned long word_to_value(Word word);

#endif