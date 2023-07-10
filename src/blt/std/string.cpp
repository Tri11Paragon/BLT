//
// Created by brett on 7/9/23.
//
#include <blt/std/string.h>

void blt::string::StringBuffer::expand() {
    size_t multiplier = size / BLOCK_SIZE;
    auto newSize = BLOCK_SIZE * (multiplier * 2);
    characterBuffer = static_cast<char*>(realloc(characterBuffer, newSize));
    size = newSize;
}

void blt::string::StringBuffer::trim() {
    characterBuffer = static_cast<char*>(realloc(characterBuffer, front+1));
    size = front+1;
    characterBuffer[front] = '\0';
}

blt::string::StringBuffer& blt::string::StringBuffer::operator<<(char c) {
    characterBuffer[front++] = c;
    if (front > size)
        expand();
    return *this;
}

std::string blt::string::StringBuffer::str() {
    trim();
    return std::string{characterBuffer};
}
