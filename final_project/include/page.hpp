#ifndef PAGE_HPP
#define PAGE_HPP

#include "block.hpp"

#define PAGE_SIZE 4096
#define BLOCKS_PER_PAGE PAGE_SIZE/BLOCK_SIZE
class Page {
    public:
        Block blocks[BLOCKS_PER_PAGE];

        void set_to_zero();
        bool is_nonzero();
        void print();
};

#endif