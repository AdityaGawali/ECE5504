#include "block.hpp"

void Block::set_to_zero() {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        this->bytes[i] = (byte) 0;
    }
}

bool Block::is_nonzero() {
    for (byte data : this->bytes) {
        if (data)
            return true;
    }
    return false;
}

void Block::print() {
    for (int j = 0; j < BLOCK_SIZE / BYTES_PER_ROW; j++) {
        for (int i = 0; i < BYTES_PER_ROW; i++) {
            std::cout << std::hex << std::setw(3) << static_cast<int>(this->bytes[i + j]);
        }
        std::cout << std::endl;
    }
}