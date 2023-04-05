/*
 * Created by Brett on 29/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/nbt/nbt.h>

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
        
        auto strOut = std::move(blt::string::getStringFromUTF8(str));
        delete[] str.characters;
        return strOut;
    }
}