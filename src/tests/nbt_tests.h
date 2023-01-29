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
    
    
    auto bufferSize = 1024 * 1024;
    char* buffer = new char[bufferSize];
    char* read_buffer = new char[bufferSize];
    char* read_block_buffer = new char[bufferSize];
    
    for (int i = 0; i < bufferSize; i++)
        buffer[i] = i+1;
    
    BLT_START_INTERVAL("nbt", "Raw Write");
        std::fstream largeOutput("HeyThere.txt", std::ios::out | std::ios::binary);
        largeOutput.write(buffer, bufferSize);
        largeOutput.flush();
        largeOutput.close();
    BLT_END_INTERVAL("nbt", "Raw Write");
    
    BLT_START_INTERVAL("nbt", "Raw Read");
        std::fstream largeInput("HeyThere.txt", std::ios::in | std::ios::binary);
        largeInput.read(read_buffer, bufferSize);
        largeInput.close();
    BLT_END_INTERVAL("nbt", "Raw Read");
    
    BLT_START_INTERVAL("nbt", "Block Read");
        std::fstream largeBlockInput("HeyThere.txt", std::ios::in | std::ios::binary);
        blt::nbt::NBTByteFStreamReader byteLargeBlockInput(largeBlockInput, 1024 * 128);
        byteLargeBlockInput.readBytes(read_block_buffer, bufferSize);
        largeBlockInput.close();
    BLT_END_INTERVAL("nbt", "Block Read");
    
    bool fstream_in_correct = true;
    bool nbt_block_in_correct = true;
    for (int i = 0; i < bufferSize; i++) {
        if (read_buffer[i] != buffer[i])
            fstream_in_correct = false;
        if (read_block_buffer[i] != buffer[i])
            nbt_block_in_correct = false;
        if (!fstream_in_correct && !nbt_block_in_correct)
            break;
    }
    
    BLT_INFO("FStream Read Correctly? %s;", fstream_in_correct ? "True" : "False");
    BLT_INFO("NBT Block Stream Correctly? %s;\n", nbt_block_in_correct ? "True" : "False");
    BLT_PRINT_ORDERED("nbt");
    
    delete[] read_buffer;
    delete[] read_block_buffer;
    delete[] buffer;
    
}

#endif //BLT_TESTS_NBT_TESTS_H
