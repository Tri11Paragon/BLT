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
    
    
    auto bufferSize = 1024 * 128;
    char* buffer = new char[bufferSize];
    char* read_buffer = new char[bufferSize];
    char* read_block_buffer = new char[bufferSize];
    
    for (int i = 0; i < bufferSize; i++)
        buffer[i] = i+1;
    
    BLT_START_INTERVAL("nbt", "Raw Write");
    std::fstream largeOutput("HeyThere.txt", std::ios::in | std::ios::binary);
    
    largeOutput.write(buffer, bufferSize);
    BLT_END_INTERVAL("nbt", "Raw Write");
    
}

#endif //BLT_TESTS_NBT_TESTS_H
