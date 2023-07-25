/*
 * Created by Brett on 29/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/nbt/nbt.h>
#include <blt/std/logging.h>
#include <cstring>
#include <bit>

namespace blt::nbt {
    void writeUTF8String(std::fstream& stream, const std::string& str) {
        blt::string::utf8_string str8 = blt::string::createUTFString(str);
        stream.write(str8.characters, str8.size);
        delete[] str8.characters;
    }
    
    std::string readUTF8String(std::fstream& stream) {
        unsigned short utflen;
        
        stream.read(reinterpret_cast<char*>(&utflen), sizeof(utflen));
        
        blt::string::utf8_string str{};
        str.size = utflen;
        str.characters = new char[str.size];
        
        stream.read(str.characters, str.size);
        
        auto strOut = blt::string::getStringFromUTF8(str);
        delete[] str.characters;
        return strOut;
    }
    
    template<typename T>
    int toBytes(const T& in, char* const out) {
        std::memcpy(out, (void*) &in, sizeof(T));
        
        if constexpr (std::endian::native == std::endian::little) {
            for (size_t i = 0; i < sizeof(T) / 2; i++)
                std::swap(out[i], out[sizeof(T) - 1 - i]);
        }
        
        return 0;
    }
    
    template<typename T>
    int fromBytes(const char* const in, T* const out) {
        memcpy(out, in, sizeof(T));

        if constexpr (std::endian::native == std::endian::little) {
            for (size_t i = 0; i < sizeof(T) / 2; i++)
                std::swap(((char*) (out))[i], ((char*) (out))[sizeof(T) - 1 - i]);
        }
        
        return 0;
    }
    
    void tag_t::writeName(std::fstream& out) {
        writeUTF8String(out, name);
    }
    
    void tag_t::readName(std::fstream& in) {
        name = readUTF8String(in);
    }
    
    void tag_end::writePayload(std::fstream& out) {
        out.put('\0');
    }
    
    void tag_byte::writePayload(std::fstream& out) {
        // single byte no need for conversion
        out.put(t);
    }
    
    void tag_byte::readPayload(std::fstream& in) {
        in.read(&t, 1);
    }
    
    void tag_short::writePayload(std::fstream& out) {
        char data[sizeof(t)];
        toBytes(t, data);
        out.write(data, sizeof(t));
    }
    
    void tag_short::readPayload(std::fstream& in) {
        char data[sizeof(t)];
        in.read(data, sizeof(t));
        fromBytes(data, &t);
    }
}