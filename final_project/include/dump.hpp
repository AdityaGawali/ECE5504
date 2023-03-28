#ifndef DUMP_HPP
#define DUMP_HPP

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <bitset>

#include "page.hpp"

const unsigned int SAMPLE_SIZE = 2000000;
const unsigned int NUM_BASES = 2048;
const unsigned int MAX_CONST_BITS = WORD_SIZE_BITS - 2;
const unsigned int MIN_CONST_BITS = MAX_CONST_BITS - 10;

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

        const unsigned int MAX_DELTA = 16 - std::log2(NUM_BASES);
        unsigned int sampled_values;
        std::unordered_map<unsigned int, unsigned int> histogram;
        std::unordered_map<unsigned int, unsigned int> smallest_bins;

        std::vector<unsigned int> bases;
        std::vector<unsigned int> deltas;

        std::vector<Packed_data> packed_data;



        Dump(char* filename);
        void load();
        void histogram_binning(unsigned int num_bases, unsigned int num_bits);
        unsigned int get_bin_id(unsigned int value, unsigned int num_bits);
        unsigned int bit_difference(unsigned int value1, unsigned int value2);
        
        float pack();
        unsigned int find_closest(std::vector<unsigned long> vec, unsigned int target);
        unsigned int getClosest(unsigned int val1,unsigned int val2, int target);
        unsigned int getIndexfromVec(std::vector<unsigned int> vec, unsigned int target);
        bool get_bit(unsigned int value, unsigned int bit_pos);
};

#endif