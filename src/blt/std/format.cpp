/*
 * Created by Brett on 26/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/format.h>
#include <cmath>
#include "blt/std/logging.h"

std::string createPadding(int padAmount) {
    std::string padStr;
    for (int i = 0; i < padAmount; i++)
        padStr += " ";
    return padStr;
}

std::vector<std::string> blt::string::TableFormatter::createTable(bool top, bool bottom) {
    std::vector<std::string> table;
    const auto& tableHeader = generateColumnHeader();
    const auto& topSeparator = generateTopSeparator(tableHeader.size());
    const auto& lineSeparator = generateSeparator(tableHeader.size() - 1);
    
    if (top)
        table.push_back(topSeparator);
    
    table.push_back(tableHeader);
    table.push_back(lineSeparator);
    
    for (const auto& row : rows) {
        std::string rowString = "|";
        for (unsigned int i = 0; i < row.rowValues.size(); i++) {
            const auto& rowValue = row.rowValues[i];
            const auto& column = columns[i];
            const int spaceLeft = int(column.maxColumnLength) - int(rowValue.size());
            // we want to prefer putting the space on the right size, flooring left and ceiling right ensures this.
            rowString += createPadding((int)std::floor(spaceLeft/2.0) + m_columnPadding);
            rowString += rowValue;
            rowString += createPadding((int)std::ceil(spaceLeft/2.0) + m_columnPadding);
            rowString += "|";
        }
        table.push_back(rowString);
    }
    
    if (bottom)
        table.push_back(lineSeparator);
    
    return table;
}

std::string blt::string::TableFormatter::generateColumnHeader() {
    updateMaxColumnLengths();
    std::string header = "|";
    
    for (unsigned int i = 0; i < columns.size(); i++) {
        const auto& column = columns[i];
        auto columnPaddingLength = (int(column.maxColumnLength) - int(column.columnName.size()))/2.0;
        header += createPadding(int(m_columnPadding + (int)std::floor(columnPaddingLength)));
        
        header += column.columnName;
        
        header += createPadding(int(m_columnPadding + (int)std::ceil(columnPaddingLength)));
        if (i < columns.size()-1)
            header += "|";
    }
    
    header += "|";
    return header;
}

std::string blt::string::TableFormatter::generateTopSeparator(size_t size) {
    auto sizeOfName = m_tableName.empty() ? 0 : m_tableName.size() + 4;
    auto sizeNameRemoved = size - sizeOfName;
    
    std::string halfWidthLeftSeparator;
    std::string halfWidthRightSeparator;
    
    auto sizeNameFloor = (size_t) std::floor((double)sizeNameRemoved/2.0);
    auto sizeNameCeil = (size_t) std::ceil((double)sizeNameRemoved/2.0);
    
    halfWidthLeftSeparator.reserve(sizeNameCeil);
    halfWidthRightSeparator.reserve(sizeNameFloor);
    
    halfWidthLeftSeparator += "+";
    
    for (unsigned int i = 0; i < sizeNameFloor - 1; i++)
        halfWidthLeftSeparator += "-";
    
    for (unsigned int i = 0; i < sizeNameCeil - 1; i++)
        halfWidthRightSeparator += "-";
    
    halfWidthRightSeparator += "+";
    
    std::string separator;
    separator += halfWidthLeftSeparator;
    if (sizeOfName != 0) {
        separator += "{ ";
        separator += m_tableName;
        separator += " }";
    }
    separator += halfWidthRightSeparator;
    return separator;
}

std::string blt::string::TableFormatter::generateSeparator(size_t size) {
    size_t nextIndex = 0;
    size_t currentColumnIndex = 0;
    std::string wholeWidthSeparator;
    for (unsigned int i = 0; i < size; i++) {
        if (i == nextIndex) {
            auto currentColumnSize = columns[currentColumnIndex++].maxColumnLength + m_columnPadding*2;
            nextIndex += currentColumnSize + 1;
            wholeWidthSeparator += "+";
        } else
            wholeWidthSeparator += "-";
    }
    wholeWidthSeparator += "+";
    return wholeWidthSeparator;
}

void blt::string::TableFormatter::updateMaxColumnLengths() {
    for (unsigned int i = 0; i < columns.size(); i++) {
        auto& column = columns[i];
        column.maxColumnLength = column.columnName.size();
        for (const auto& row : rows) {
            column.maxColumnLength = std::max(column.maxColumnLength, row.rowValues[i].size());
        }
    }
}

