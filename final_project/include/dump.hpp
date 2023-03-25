#ifndef DUMP_HPP
#define DUMP_HPP

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <list>
#include <unordered_map>

#include "page.hpp"

#define SAMPLE_SIZE 2000000
#define NUM_BASES 2048
#define MIN_BINS (NUM_BASES * 4096)
#define MAX_BINS MIN_BINS
#define MAX_DELTA 4

struct _Packed_data {
            
    unsigned int value;
    unsigned int base_pointer;
    unsigned int delta;
    bool mask;
};
typedef _Packed_data Packed_data;

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
        std::vector<unsigned int> deltas;

        std::vector<Packed_data> packed_data;

        Dump(char* filename);
        void load();
        void histogram_binning(unsigned long num_bases, unsigned long num_bins);
        void pack();
        
        unsigned int find_closest(std::vector<unsigned long> vec, unsigned int target);
        unsigned int getClosest(unsigned int val1,unsigned int val2, int target);
        unsigned int getIndexfromVec(std::vector<unsigned long> vec, unsigned long target);


};

#endif