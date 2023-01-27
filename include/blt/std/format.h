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
            inline void addRow(TableRow row){
                if (row.rowValues.size() > columns.size())
                    throw "Cannot insert more rows than columns!\n";
                // ensure every row populates every column. This is important as the table generator assumes that all rows are complete!
                if (row.rowValues.size() < columns.size())
                    for (auto i = row.rowValues.size(); i < columns.size(); i++)
                        row.rowValues.emplace_back(" ");
                rows.push_back(std::move(row));
            }
            inline void addRow(const std::initializer_list<std::string>& values){
                TableRow row;
                for (const auto& value : values)
                    row.rowValues.push_back(value);
                addRow(row);
            }
            std::vector<std::string> createTable(bool top = false, bool bottom = false);
    };
    
}
#endif //BLT_TESTS_FORMAT_H
