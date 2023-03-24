#ifndef DUMP_HPP
#define DUMP_HPP

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <list>
#include <unordered_map>

#include "page.hpp"

#define SAMPLE_SIZE 2000000
#define NUM_BASES 2048
#define MIN_BINS (NUM_BASES * 512)
#define MAX_BINS MIN_BINS

class Dump {
    public:
        char* filename;
        std::vector<Page> pages;
        unsigned int uncompressed_size;

        const unsigned long num_values = pow(2, WORD_SIZE * 8);
        const unsigned long smallest_bin_size = num_values / MAX_BINS;
        unsigned long sampled_values;
        std::unordered_map<unsigned long, unsigned long> histogram;
        std::unordered_map<unsigned int, unsigned long> smallest_bins;
        std::vector<unsigned long> bases;

        Dump(char* filename);
        void load();
        void histogram_binning(unsigned long num_bases, unsigned long num_bins);
};

#endif