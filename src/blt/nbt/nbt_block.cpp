/*
 * Created by Brett on 27/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/nbt/nbt_block.h>
#include <string>
#include <fstream>

namespace blt::nbt {
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