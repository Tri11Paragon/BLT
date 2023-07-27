/*
 * Created by Brett on 29/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/nbt/nbt.h>
#include <blt/std/logging.h>

#include <type_traits>

namespace blt::nbt {
    void writeUTF8String(blt::fs::block_writer& stream, const std::string& str) {
        blt::string::utf8_string str8 = blt::string::createUTFString(str);
        stream.write(str8.characters, str8.size);
        delete[] str8.characters;
    }
    
    std::string readUTF8String(blt::fs::block_reader& stream) {
        int16_t utflen;
        
        readData(stream, utflen);
        
        blt::string::utf8_string str{};
        str.size = utflen;
        str.characters = new char[str.size];
        
        stream.read(str.characters, str.size);
        
        auto strOut = blt::string::getStringFromUTF8(str);
        delete[] str.characters;
        return strOut;
    }
    
    void NBTReader::read() {
        char t = reader.get();
        if (t != (char)nbt_tag::COMPOUND)
            throw std::runtime_error("Incorrectly formatted NBT data! Root tag must be a compound tag!");
        root = new tag_compound;
        root->readName(reader);
        root->readPayload(reader);
    }
}