/*
 * Created by Brett on 26/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_FORMAT_H
#define BLT_TESTS_FORMAT_H

#include <string>
#include <utility>
#include <vector>

namespace blt::string {
    
    /**
     * Ensure that string str has expected length, pad after the string otherwise.
     * @param str string to pad
     * @param expectedLength expected length of the string.
     * @return a space padded string
     */
    static inline std::string postPadWithSpaces(const std::string& str, size_t expectedLength) {
        auto currentSize = (int) (str.length() - 1);
        if ((int) expectedLength - currentSize <= 0)
            return str;
        auto paddedString = str;
        for (int i = 0; i < expectedLength - currentSize; i++)
            paddedString += " ";
        return paddedString;
    }
    
    /**
     * Ensure that string str has expected length, pad before the string otherwise.
     * @param str string to pad
     * @param expectedLength expected length of the string.
     * @return a space padded string
     */
    static inline std::string prePadWithSpaces(const std::string& str, size_t expectedLength) {
        auto currentSize = str.length() - 1;
        auto paddedString = std::string();
        for (int i = 0; i < expectedLength - currentSize; i++)
            paddedString += " ";
        paddedString += str;
        return paddedString;
    }
    
    struct utf8_string {
        char* characters;
        unsigned int size;
    };
    
    // taken from java, adapted for c++.
    static inline utf8_string createUTFString(const std::string& str) {
        
        const unsigned int strlen = str.size();
        unsigned int utflen = strlen;
        
        for (int i = 0; i < strlen; i++) {
            unsigned char c = str[i];
            if (c >= 0x80 || c == 0)
                utflen += (c >= 0x800) ? 2 : 1;
        }
        
        if (utflen > 65535 || /* overflow */ utflen < strlen)
            throw "UTF Error";
        
        utf8_string chars{};
        chars.size = utflen + 2;
        chars.characters = new char[chars.size];
        
        int count = 0;
        chars.characters[count++] = (char) ((utflen >> 0) & 0xFF);
        chars.characters[count++] = (char) ((utflen >> 8) & 0xFF);
        
        int i = 0;
        for (i = 0; i < strlen; i++) { // optimized for initial run of ASCII
            int c = (unsigned char) str[i];
            if (c >= 0x80 || c == 0) break;
            chars.characters[count++] = (char) c;
        }
        
        for (; i < strlen; i++) {
            int c = (unsigned char) str[i];
            if (c < 0x80 && c != 0) {
                chars.characters[count++] = (char) c;
            } else if (c >= 0x800) {
                chars.characters[count++] = (char) (0xE0 | ((c >> 12) & 0x0F));
                chars.characters[count++] = (char) (0x80 | ((c >> 6) & 0x3F));
                chars.characters[count++] = (char) (0x80 | ((c >> 0) & 0x3F));
            } else {
                chars.characters[count++] = (char) (0xC0 | ((c >> 6) & 0x1F));
                chars.characters[count++] = (char) (0x80 | ((c >> 0) & 0x3F));
            }
        }
        return chars;
    }
    
    static inline std::string getStringFromUTF8(const utf8_string& str) {
        auto utflen = str.size;
        int c, char2, char3;
        int count = 0;
        int chararr_count = 0;
        
        auto chararr = new char[utflen + 1];
        
        while (count < utflen) {
            c = (int) str.characters[count] & 0xff;
            if (c > 127) break;
            count++;
            chararr[chararr_count++] = (char) c;
        }
        
        while (count < utflen) {
            c = (int) str.characters[count] & 0xff;
            switch (c >> 4) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    /* 0xxxxxxx*/
                    count++;
                    chararr[chararr_count++] = (char) c;
                    break;
                case 12:
                case 13:
                    /* 110x xxxx   10xx xxxx*/
                    count += 2;
                    if (count > utflen)
                        throw "malformed input: partial character at end";
                    char2 = (int) str.characters[count - 1];
                    if ((char2 & 0xC0) != 0x80)
                        throw "malformed input around byte " + std::to_string(count);
                    chararr[chararr_count++] = (char) (((c & 0x1F) << 6) |
                                                       (char2 & 0x3F));
                case 14:
                    /* 1110 xxxx  10xx xxxx  10xx xxxx */
                    count += 3;
                    if (count > utflen)
                        throw "malformed input: partial character at end";
                    char2 = (int) str.characters[count - 2];
                    char3 = (int) str.characters[count - 1];
                    if (((char2 & 0xC0) != 0x80) || ((char3 & 0xC0) != 0x80))
                        throw "malformed input around byte " + std::to_string(count - 1);
                    chararr[chararr_count++] = (char) (((c & 0x0F) << 12) |
                                                       ((char2 & 0x3F) << 6) |
                                                       ((char3 & 0x3F) << 0));
                    break;
                default:
                    /* 10xx xxxx,  1111 xxxx */
                    throw "malformed input around byte " + std::to_string(count);
                    break;
            }
        }
        chararr[utflen] = '\0';
        std::string strs {chararr};
        delete[] chararr;
        return strs;
    }
    
    struct TableColumn {
        std::string columnName;
        size_t maxColumnLength = 0;
        
        TableColumn(std::string columnName): columnName(std::move(columnName)) {}
    };
    
    struct TableRow {
        std::vector<std::string> rowValues;
    };
    
    class TableFormatter {
        private:
            int m_columnPadding;
            int m_maxColumnWidth;
            std::vector<TableColumn> columns;
            std::vector<TableRow> rows;
            
            static std::string generateTopSeparator(size_t size);
            
            std::string generateColumnHeader();
            
            std::string generateSeparator(size_t size);
            
            void updateMaxColumnLengths();
            
            [[nodiscard]] inline size_t columnSize(const TableColumn& column) const {
                return column.columnName.size() + m_columnPadding * 2;
            }
        
        public:
            explicit TableFormatter(int columnPadding = 2, int maxColumnWidth = 500):
                    m_columnPadding(columnPadding), m_maxColumnWidth(maxColumnWidth) {}
            
            inline void addColumn(const TableColumn& column) {
                columns.push_back(column);
            }
            
            inline void addRow(TableRow row) {
                if (row.rowValues.size() > columns.size())
                    throw "Cannot insert more rows than columns!\n";
                // ensure every row populates every column. This is important as the table generator assumes that all rows are complete!
                if (row.rowValues.size() < columns.size())
                    for (auto i = row.rowValues.size(); i < columns.size(); i++)
                        row.rowValues.emplace_back(" ");
                rows.push_back(std::move(row));
            }
            
            inline void addRow(const std::initializer_list<std::string>& values) {
                TableRow row;
                for (const auto& value : values)
                    row.rowValues.push_back(value);
                addRow(row);
            }
            
            std::vector<std::string> createTable(bool top = false, bool bottom = false);
    };
    
    class LineGraphFormatter {
        private:
        
        public:
        
    };
    
}
#endif //BLT_TESTS_FORMAT_H
