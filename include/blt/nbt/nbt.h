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
    
    enum nbt_type {
        tag_end = 0,
        tag_byte = 1,
        tag_short = 2,
        tag_int = 3,
        tag_long = 4,
        tag_float = 5,
        tag_double = 6,
        tag_byte_array = 7,
        tag_string = 8,
        tag_list = 9,
        tag_compound = 10,
        tag_int_array = 11,
        tag_long_array = 12
    };
    
    class nbt_tag {
        public:
            virtual void readTag() = 0;
            virtual void writeTag() = 0;
    };
    
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
