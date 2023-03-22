#include "page.hpp"

void Page::set_to_zero() {
    for (int i = 0; i < PAGE_SIZE; i++) {
        this->data[i] = (byte) 0;
    }
}

bool Page::is_nonzero() {
    for (int i = 0; i < PAGE_SIZE; i++) {
        if (this->data[i]) {
            return true;
        }
    }
    return false;
}

void Page::print() {
    for (int j = 0; j < PAGE_SIZE / BYTES_PER_ROW; j++) {
        for (int i = 0; i < BYTES_PER_ROW; i++) {
            std::cout << std::hex << std::setw(3) << static_cast<int>(this->data[i + j]);
        }
        std::cout << std::endl;
    }
}