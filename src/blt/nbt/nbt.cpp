/*
 * Created by Brett on 29/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/nbt/nbt.h>
#include <cstring>

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
        return std::move(strOut);
    }
    
    void NBTByteFStreamReader::readBytes(char* buffer, size_t bytes) {
        if (readIndex == 0)
            m_stream.read(m_buffer, (long) m_bufferSize);
        if (readIndex + bytes >= m_bufferSize) {
            auto bytesLeft = m_bufferSize - readIndex;
            memcpy(buffer, m_buffer + readIndex, bytesLeft);
            readIndex = 0;
            bytes -= bytesLeft;
            readBytes(buffer + bytesLeft, bytes);
//            m_stream.read(m_buffer, (long) m_bufferSize);
//            memcpy(buffer + bytesLeft, m_buffer, bytes);
//            readIndex += bytes;
        } else {
            std::memcpy(buffer, m_buffer + readIndex, bytes);
            readIndex += bytes;
        }
    }
}