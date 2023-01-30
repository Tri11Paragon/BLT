/*
 * Created by Brett on 27/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_NBT_H
#define BLT_TESTS_NBT_H

#include "blt/std/format.h"
#include "blt/std/filesystem.h"

namespace blt::nbt {
    void writeUTF8String(std::fstream& stream, const std::string& str);
    
    std::string readUTF8String(std::fstream& stream);
    
    class NBTDecoder {
        private:
            blt::fs::block_reader* m_reader;
        public:
            explicit NBTDecoder(blt::fs::block_reader* reader): m_reader(reader) {}
        
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
