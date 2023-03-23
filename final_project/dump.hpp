#ifndef DUMP_HPP
#define DUMP_HPP

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <math.h>

#include "page.hpp"

class Dump {
    public:
        char* filename;
        std::vector<Page> pages;
        unsigned int uncompressed_size;

        std::vector<byte> bases;

        Dump(char* filename);
        void load();
        void histogram_binning(int num_bases, int num_bins);
};

#endif