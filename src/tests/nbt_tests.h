/*
 * Created by Brett on 29/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_NBT_TESTS_H
#define BLT_TESTS_NBT_TESTS_H

#include <blt/nbt/nbt.h>
#include <blt/profiling/profiler.h>
#include <blt/std/logging.h>
#include <blt/std/format.h>

struct nbt_scoped_buffer {
    char* buffer;
    size_t bufferSize;
    explicit nbt_scoped_buffer(size_t bufferSize): bufferSize(bufferSize) {
        buffer = new char[bufferSize];
    }
    ~nbt_scoped_buffer(){
        delete[] buffer;
    }
    inline char operator[](size_t index) const {
        return buffer[index];
    }
};

inline bool readLargeBlockUsingNBTBufferedReader(const std::string& file, const nbt_scoped_buffer& bufferToCompare, size_t bufferSize) {
    nbt_scoped_buffer read_buffer{bufferToCompare.bufferSize};
    std::fstream largeBlockInputLarge(file, std::ios::in | std::ios::binary);
    blt::nbt::NBTByteFStreamReader byteLargeBlockInputLarge(largeBlockInputLarge, bufferSize);
    byteLargeBlockInputLarge.readBytes(read_buffer.buffer, bufferToCompare.bufferSize);
    for (int i = 0; i < bufferToCompare.bufferSize; i++) {
        if (read_buffer[i] != bufferToCompare.buffer[i])
            return false;
    }
    largeBlockInputLarge.close();
    return true;
}

inline void nbt_tests(){
    std::fstream testOutput("Hello.txt", std::ios::out | std::ios::binary);
    
    char testByte[] = {3, 'A', 91};
    short testShort = 6132;
    int testInt = 6600099;
    
    
    testOutput.write(testByte, 3);
    testOutput.write(reinterpret_cast<char*>(&testShort), sizeof(short));
    testOutput.write(reinterpret_cast<char*>(&testInt), sizeof(int));
    blt::nbt::writeUTF8String(testOutput, "HelloHowManyCanWeFit!");
    
    //testOutput.flush();
    testOutput.close();
    
    std::fstream testInput("Hello.txt", std::ios::in | std::ios::binary);
    
    char testByteIn[3];
    short testShortIn;
    int testIntIn;
    
    testInput.read(testByteIn, 3);
    testInput.read(reinterpret_cast<char*>(&testShortIn), sizeof(short));
    testInput.read(reinterpret_cast<char*>(&testIntIn), sizeof(int));
    std::string strIn = blt::nbt::readUTF8String(testInput);
    
    testInput.close();
    BLT_INFO("%d, %c, %d, %d, %d, %s", testByteIn[0], testByteIn[1], testByteIn[2], testShortIn, testIntIn, strIn.c_str());
    
    
    constexpr auto bufferSize = 1024 * 128;
    
    nbt_scoped_buffer buffer{bufferSize};
    
    char* read_buffer = new char[bufferSize];
    char* read_block_buffer = new char[bufferSize];
    
    bool fstream_indv_correct = true;
    bool fstream_large_correct = true;
    bool nbt_block_indv_correct = true;
    
    for (int i = 0; i < bufferSize; i++)
        buffer.buffer[i] = i+1;
    
    BLT_START_INTERVAL("nbt", "Raw Write");
        std::fstream largeOutput("HeyThere.txt", std::ios::out | std::ios::binary);
        largeOutput.write(buffer.buffer, bufferSize);
        largeOutput.flush();
        largeOutput.close();
    BLT_END_INTERVAL("nbt", "Raw Write");
    
    BLT_START_INTERVAL("nbt", "Raw Read Individual");
        std::fstream largeInput("HeyThere.txt", std::ios::in | std::ios::binary);
        for (int i = 0; i < bufferSize; i++) {
            char byte;
            largeInput.read(&byte, 1);
            if (byte != buffer[i]) {
                fstream_indv_correct = false;
                break;
            }
        }
        largeInput.close();
    BLT_END_INTERVAL("nbt", "Raw Read Individual");
    
    BLT_START_INTERVAL("nbt", "Block Read Individual");
        std::fstream largeBlockInput("HeyThere.txt", std::ios::in | std::ios::binary);
        blt::nbt::NBTByteFStreamReader byteLargeBlockInput(largeBlockInput, 1024 * 8);
        for (int i = 0; i < bufferSize; i++) {
            char byte;
            byteLargeBlockInput.readBytes(&byte, 1);
            if (byte != buffer[i]) {
                nbt_block_indv_correct = false;
                break;
            }
        }
        largeBlockInput.close();
    BLT_END_INTERVAL("nbt", "Block Read Individual");
    
    BLT_START_INTERVAL("nbt", "Raw Read Large");
    std::fstream largeInputLarge("HeyThere.txt", std::ios::in | std::ios::binary);
    largeInputLarge.read(read_buffer, bufferSize);
    for (int i = 0; i < bufferSize; i++) {
        if (read_buffer[i] != buffer[i])
            fstream_large_correct = false;
    }
    largeInputLarge.close();
    BLT_END_INTERVAL("nbt", "Raw Read Large");
    
    BLT_INFO("FStream Read Correctly? %s;", fstream_indv_correct ? "True" : "False");
    BLT_INFO("FStream Large Read Correctly? %s;", fstream_large_correct ? "True" : "False");
    BLT_INFO("NBT Block Stream Correctly? %s;\n", nbt_block_indv_correct ? "True" : "False");
    
    for (int i = 0; i < 8; i++) {
        auto size = (size_t) std::pow(2, 11 + i);
        auto size_str = std::to_string(size);
        auto profiler_string = "Block Read " + size_str;
        bool nbt_block_large_correct = true;
        BLT_START_INTERVAL("nbt", profiler_string);
        nbt_block_large_correct = readLargeBlockUsingNBTBufferedReader("HeyThere.txt", buffer, size);
        BLT_END_INTERVAL("nbt", profiler_string);
    
        BLT_INFO("NBT Block %s Stream Correctly? %s;\n", size_str.c_str(), nbt_block_large_correct ? "True" : "False");
    }
    
    BLT_PRINT_ORDERED("nbt");
    
    delete[] read_buffer;
    delete[] read_block_buffer;
    
}

#endif //BLT_TESTS_NBT_TESTS_H
