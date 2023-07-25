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
    
    // Used to grab the byte-data of any T element. Defaults to Big Endian, however can be configured to use little endian
    template <typename T>
    int toBytes(const T& in, char* out);
    
    // Used to cast the binary data of any T object, into a T object.
    template <typename T>
    int fromBytes(const char* in, T* out);
    
    
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
    
    class tag_t {
        protected:
            nbt_tag type;
            std::string name;
        public:
            explicit tag_t(nbt_tag type): type(type) {};
            explicit tag_t(nbt_tag type, std::string name): type(type), name(std::move(name)) {}
            virtual void writePayload(std::fstream& out) = 0;
            virtual void readPayload(std::fstream& in) = 0;
            void writeName(std::fstream& out);
            void readName(std::fstream& in);
    };
    
    template<typename T>
    class tag : public tag_t {
        protected:
            T t;
        public:
            explicit tag(nbt_tag type): tag_t(type) {};
            explicit tag(nbt_tag type, std::string name): tag_t(type, std::move(name)) {}
            [[nodiscard]] inline const T& get() const {return t;}
            inline T& get() {return t;}
    };
    
    class tag_end : public tag<char> {
        public:
            void writePayload(std::fstream& out) final;
            // nothing to read
            void readPayload(std::fstream&) final {}
    };
    
    class tag_byte : public tag<char> {
        public:
            void writePayload(std::fstream& out) final;
            void readPayload(std::fstream& in) final;
    };
    
    class tag_short : public tag<int16_t> {
        public:
            void writePayload(std::fstream& out) final;
            void readPayload(std::fstream& in) final;
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
