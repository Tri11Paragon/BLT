/*
 * Created by Brett on 27/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_NBT_H
#define BLT_TESTS_NBT_H

#include <utility>

#include "blt/std/format.h"
#include "blt/std/filesystem.h"

namespace blt::nbt {
    void writeUTF8String(std::fstream& stream, const std::string& str);
    
    std::string readUTF8String(std::fstream& stream);
    
    enum class nbt_tag : char {
        END = 0,
        BYTE = 1,
        SHORT = 2,
        INT = 3,
        LONG = 4,
        FLOAT = 5,
        DOUBLE = 6,
        BYTE_ARRAY = 7,
        STRING = 8,
        LIST = 9,
        COMPOUND = 10,
        INT_ARRAY = 11,
        LONG_ARRAY = 12
    };
    
    class tag {
        public:
            virtual void writePayload(std::fstream& out) = 0;
            virtual void readPayload(std::fstream& in) = 0;
    };
    
    class named_tag : public tag {
        private:
            std::string name;
        public:
            explicit named_tag(std::string name): name(std::move(name)) {}
            named_tag() = default;
            void writeName(std::fstream& out);
            void readName(std::fstream& in);
    };
    
    class tag_end : public tag {
        public:
            void writePayload(std::fstream& out) final;
            // nothing to read
            void readPayload(std::fstream& in) final {}
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
