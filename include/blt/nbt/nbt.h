/*
 * Created by Brett on 27/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_NBT_H
#define BLT_TESTS_NBT_H

#include <fstream>
#include <ios>

#include "blt/std/format.h"

namespace blt::nbt {
    void writeUTF8String(std::fstream& stream, const std::string& str);
    
    std::string readUTF8String(std::fstream& stream);
    
    class NBTByteReader {
        public:
            virtual void readBytes(char* buffer, size_t bytes) = 0;
    };
    
    class NBTByteFStreamReader : public NBTByteReader {
        private:
            std::fstream& m_stream;
            size_t m_bufferSize;
            char* m_buffer;
            size_t readIndex = 0;
        public:
            explicit NBTByteFStreamReader(std::fstream& stream, size_t bufferSize):
                    m_stream(stream), m_bufferSize(bufferSize), m_buffer(new char[bufferSize]) {}
            
            explicit NBTByteFStreamReader(NBTByteFStreamReader& copy) = delete;
            
            explicit NBTByteFStreamReader(NBTByteFStreamReader&& move) = delete;
            
            NBTByteFStreamReader& operator=(const NBTByteFStreamReader& copy) = delete;
            
            void readBytes(char* buffer, size_t bytes) override;
            
            ~NBTByteFStreamReader() {
                delete[] m_buffer;
            }
    };
    
    class NBTDecoder {
        private:
            NBTByteReader* m_reader;
        public:
            explicit NBTDecoder(NBTByteReader* reader): m_reader(reader) {}
        
    };
    
    /**
     * Reads the entire NBT file when the read() function is called.
     */
    class NBTReader {
        private:
            std::string m_file;
        
        public:
            explicit NBTReader(std::string file): m_file(std::move(file)) {}
    };
    
}

#endif //BLT_TESTS_NBT_H
