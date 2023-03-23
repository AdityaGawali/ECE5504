#include "page.hpp"

void Page::set_to_zero() {
    for (Block block : this->blocks) {
        block.set_to_zero();
    }
}

bool Page::is_nonzero() {
    for (Block block : this->blocks) {
        if (block.is_nonzero())
            return true;
    }
    return false;
}

void Page::print() {
    for (Block block : this->blocks) {
        block.print();
    }
}