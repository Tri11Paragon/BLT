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
#include <blt/std/filesystem.h>

inline bool readLargeBlockUsingNBTBufferedReader(const std::string& file, const blt::scoped_buffer<char>& bufferToCompare, size_t bufferSize) {
    blt::scoped_buffer<char> read_buffer{bufferToCompare.size};
    std::fstream largeBlockInputLarge(file, std::ios::in | std::ios::binary);
    blt::fs::fstream_block_reader byteLargeBlockInputLarge(largeBlockInputLarge, bufferSize);
    byteLargeBlockInputLarge.read(read_buffer.buffer, bufferToCompare.size);
    for (int i = 0; i < bufferToCompare.size; i++) {
        if (read_buffer[i] != bufferToCompare.buffer[i])
            return false;
    }
    largeBlockInputLarge.close();
    return true;
}

inline bool readIndividualUsingNBTBufferedReader(const std::string& file, const blt::scoped_buffer<char>& bufferToCompare, size_t bufferSize) {
    std::fstream largeBlockInput(file, std::ios::in | std::ios::binary);
    blt::fs::fstream_block_reader byteLargeBlockInput(largeBlockInput, bufferSize);
    for (int i = 0; i < bufferToCompare.size; i++) {
        char byte;
        byteLargeBlockInput.read(&byte, 1);
        if (byte != bufferToCompare[i]) {
            return false;
        }
    }
    largeBlockInput.close();
    return true;
}

inline void nbt_read_tests(){
    constexpr auto bufferSize = 1024 * 1024;
    
    blt::scoped_buffer<char> buffer{bufferSize};
    
    char* read_buffer = new char[bufferSize];
    char* read_block_buffer = new char[bufferSize];
    
    bool fstream_indv_correct = true;
    bool fstream_large_correct = true;
    
    for (int i = 0; i < bufferSize; i++)
        buffer.buffer[i] = i+1;
    
    BLT_START_INTERVAL("nbt read", "Raw Write");
    std::fstream largeOutput("HeyThere.txt", std::ios::out | std::ios::binary);
    largeOutput.write(buffer.buffer, bufferSize);
    largeOutput.flush();
    largeOutput.close();
    BLT_END_INTERVAL("nbt read", "Raw Write");
    
    BLT_START_INTERVAL("nbt read", "Raw Read Individual");
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
    BLT_END_INTERVAL("nbt read", "Raw Read Individual");
    
    BLT_START_INTERVAL("nbt read", "Raw Read Large");
    std::fstream largeInputLarge("HeyThere.txt", std::ios::in | std::ios::binary);
    largeInputLarge.read(read_buffer, bufferSize);
    for (int i = 0; i < bufferSize; i++) {
        if (read_buffer[i] != buffer[i])
            fstream_large_correct = false;
    }
    largeInputLarge.close();
    BLT_END_INTERVAL("nbt read", "Raw Read Large");
    
    BLT_INFO("FStream Read Correctly? %s;", fstream_indv_correct ? "True" : "False");
    BLT_INFO("FStream Large Read Correctly? %s;", fstream_large_correct ? "True" : "False");
    
    
    for (int i = 0; i < 8; i++) {
        auto size = (size_t) std::pow(2, 11 + i);
        auto size_str = std::to_string(size);
        auto profiler_string = "Block Read Individual " + size_str;
        bool nbt_block_correct = true;
        BLT_START_INTERVAL("nbt read individual", profiler_string);
        nbt_block_correct = readIndividualUsingNBTBufferedReader("HeyThere.txt", buffer, size);
        BLT_END_INTERVAL("nbt read individual", profiler_string);
        
        BLT_INFO("NBT Individual Block %s Stream Correctly? %s;\n", size_str.c_str(), nbt_block_correct ? "True" : "False");
    }
    
    for (int i = 0; i < 8; i++) {
        auto size = (size_t) std::pow(2, 11 + i);
        auto size_str = std::to_string(size);
        auto profiler_string = "Block Read " + size_str;
        bool nbt_block_large_correct = true;
        BLT_START_INTERVAL("nbt read block", profiler_string);
        nbt_block_large_correct = readLargeBlockUsingNBTBufferedReader("HeyThere.txt", buffer, size);
        BLT_END_INTERVAL("nbt read block", profiler_string);
        
        BLT_INFO("NBT Block %s Stream Correctly? %s;\n", size_str.c_str(), nbt_block_large_correct ? "True" : "False");
    }
    
    BLT_PRINT_ORDERED("nbt read");
    BLT_TRACE("{BLANK_LINE}");
    BLT_PRINT_ORDERED("nbt read block");
    BLT_TRACE("{BLANK_LINE}");
    BLT_PRINT_ORDERED("nbt read individual");
    
    delete[] read_buffer;
    delete[] read_block_buffer;
}

inline void nbt_write_tests(){
    constexpr auto bufferSize = 1024 * 1024;
    
    blt::scoped_buffer<char> buffer{bufferSize};
    blt::scoped_buffer<char> read_buffer{bufferSize};
    
    for (int i = 0; i < bufferSize; i++)
        buffer.buffer[i] = i+1;
    
    std::fstream fileOutput("IAmAFile.txt", std::ios::binary | std::ios::out);
    for (int i = 0; i < 8; i++) {
        auto size = (size_t) std::pow(2, 11 + i);
        auto size_str = std::to_string(size);
        auto profiler_string = "Writer " + size_str;
        blt::fs::fstream_block_writer writer(fileOutput, size);
        
        BLT_START_INTERVAL("nbt write block", profiler_string);
        writer.write(buffer.buffer, buffer.size);
        BLT_END_INTERVAL("nbt write block", profiler_string);
        BLT_START_INTERVAL("nbt write individual", profiler_string);
        for (int j = 0; j < bufferSize; j++) {
            writer.write(&buffer.buffer[j], 1);
        }
        BLT_END_INTERVAL("nbt write individual", profiler_string);
    }
    fileOutput.flush();
    
    std::fstream fileInput("IAmAFile.txt", std::ios::binary | std::ios::in);
    
    for (int i = 0; i < 8; i++) {
        auto size = (size_t) std::pow(2, 11 + i);
        auto size_str = std::to_string(size);
        bool results = true;
        fileInput.read(read_buffer.buffer, bufferSize);
        for (int j = 0; j < bufferSize; j++) {
            if (buffer[j] != read_buffer[j]) {
                results = false;
                BLT_FATAL("Error occurred at size %d and index %d", size, j);
                break;
            }
        }
        BLT_INFO("NBT %s Block Write Correctly? %s;\n", size_str.c_str(), results ? "True" : "False");
    
        results = true;
        fileInput.read(read_buffer.buffer, bufferSize);
        for (int j = 0; j < bufferSize; j++) {
            if (buffer[j] != read_buffer[j]) {
                results = false;
                BLT_FATAL("Error occurred at size %d and index %d", size, j);
                break;
            }
        }
        BLT_INFO("NBT %s Individual Write Correctly? %s;\n", size_str.c_str(), results ? "True" : "False");
    }
    
    BLT_PRINT_ORDERED("nbt write individual");
    BLT_TRACE("");
    BLT_PRINT_ORDERED("nbt write block");
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
    
    nbt_read_tests();
    nbt_write_tests();
    
}

#endif //BLT_TESTS_NBT_TESTS_H
