#include "block.hpp"

void Block::set_to_zero() {
    for (int j = 0; j < WORDS_PER_BLOCK; j++) {
        for (int i = 0; i < WORD_SIZE; i++)
            this->words[j].bytes[i] = (byte) 0;
    }
}

bool Block::is_nonzero() {
    for (Word word : this->words) {
        for (byte data : word.bytes) {
            if (data)
                return true;
        }
    }
    
    return false;
}

void Block::print() {
    for (int j = 0; j < BLOCK_SIZE / BYTES_PER_ROW; j++) {
        for (int i = 0; i < BYTES_PER_ROW; i++) {
            std::cout << std::hex << std::setw(3) << static_cast<int>(this->words[(j * BYTES_PER_ROW + i) % WORDS_PER_BLOCK].bytes[(j * BYTES_PER_ROW + i) % WORD_SIZE]);
        }
        std::cout << std::endl;
    }
    std::cout << std::dec;
}

unsigned long word_to_value(Word word) {
    unsigned long return_value = 0;
    for (int i = 0; i < WORD_SIZE; i++) {
        return_value += (unsigned long) (word.bytes[i] << (i * 8));
    }
    return return_value;
}