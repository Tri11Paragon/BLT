/*
 * Created by Brett on 26/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/format.h>
#include <cmath>
#include "blt/std/logging.h"
#include "blt/std/assert.h"
#include <stack>
#include <queue>
#include <algorithm>

std::vector<std::string> blt::string::TableFormatter::createTable(bool top, bool bottom)
{
    std::vector<std::string> table;
    const auto& tableHeader = generateColumnHeader();
    const auto& topSeparator = generateTopSeparator(tableHeader.size());
    const auto& lineSeparator = generateSeparator(tableHeader.size() - 1);
    
    if (top)
        table.push_back(topSeparator);
    
    table.push_back(tableHeader);
    table.push_back(lineSeparator);
    
    for (const auto& row : rows)
    {
        std::string rowString = "|";
        for (unsigned int i = 0; i < row.rowValues.size(); i++)
        {
            const auto& rowValue = row.rowValues[i];
            const auto& column = columns[i];
            const int spaceLeft = int(column.maxColumnLength) - int(rowValue.size());
            // we want to prefer putting the space on the right size, flooring left and ceiling right ensures this.
            rowString += createPadding((int) std::floor(spaceLeft / 2.0) + m_columnPadding);
            rowString += rowValue;
            rowString += createPadding((int) std::ceil(spaceLeft / 2.0) + m_columnPadding);
            rowString += "|";
        }
        table.push_back(rowString);
    }
    
    if (bottom)
        table.push_back(lineSeparator);
    
    return table;
}

std::string blt::string::TableFormatter::generateColumnHeader()
{
    updateMaxColumnLengths();
    std::string header = "|";
    
    for (unsigned int i = 0; i < columns.size(); i++)
    {
        const auto& column = columns[i];
        auto columnPaddingLength = (int(column.maxColumnLength) - int(column.columnName.size())) / 2.0;
        header += createPadding(int(m_columnPadding + (int) std::floor(columnPaddingLength)));
        
        header += column.columnName;
        
        header += createPadding(int(m_columnPadding + (int) std::ceil(columnPaddingLength)));
        if (i < columns.size() - 1)
            header += "|";
    }
    
    header += "|";
    return header;
}

std::string blt::string::TableFormatter::generateTopSeparator(size_t size)
{
    auto sizeOfName = m_tableName.empty() ? 0 : m_tableName.size() + 4;
    auto sizeNameRemoved = size - sizeOfName;
    
    std::string halfWidthLeftSeparator;
    std::string halfWidthRightSeparator;
    
    auto sizeNameFloor = (size_t) std::floor((double) sizeNameRemoved / 2.0);
    auto sizeNameCeil = (size_t) std::ceil((double) sizeNameRemoved / 2.0);
    
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
    if (sizeOfName != 0)
    {
        separator += "{ ";
        separator += m_tableName;
        separator += " }";
    }
    separator += halfWidthRightSeparator;
    return separator;
}

std::string blt::string::TableFormatter::generateSeparator(size_t size)
{
    size_t nextIndex = 0;
    size_t currentColumnIndex = 0;
    std::string wholeWidthSeparator;
    for (unsigned int i = 0; i < size; i++)
    {
        if (i == nextIndex)
        {
            auto currentColumnSize = columns[currentColumnIndex++].maxColumnLength + m_columnPadding * 2;
            nextIndex += currentColumnSize + 1;
            wholeWidthSeparator += "+";
        } else
            wholeWidthSeparator += "-";
    }
    wholeWidthSeparator += "+";
    return wholeWidthSeparator;
}

void blt::string::TableFormatter::updateMaxColumnLengths()
{
    for (unsigned int i = 0; i < columns.size(); i++)
    {
        auto& column = columns[i];
        column.maxColumnLength = column.columnName.size();
        for (const auto& row : rows)
        {
            column.maxColumnLength = std::max(column.maxColumnLength, row.rowValues[i].size());
        }
    }
}

/*
 * -----------------------
 *     Tree Formatter
 * -----------------------
 */

struct node_data
{
    blt::string::TreeFormatter::Node* node;
    std::vector<std::string> box;
    size_t level;
    
    node_data(blt::string::TreeFormatter::Node* node, size_t level): node(node), level(level)
    {}
};

struct level_data
{
    std::vector<node_data> level;
    size_t count = 0;
    size_t depth = 0;
    size_t max_horizontal_length = 0;
};

std::vector<std::string> blt::string::TreeFormatter::construct()
{
    std::stack<level_data> levels;
    std::queue<node_data> bfs;
    bfs.emplace(root, 0);
    // construct a stack of the highest node -> the lowest node.
    level_data currentLevel;
    currentLevel.depth = 0;
    while (!bfs.empty())
    {
        auto n = bfs.front();
        if (currentLevel.depth != n.level)
        {
            levels.push(currentLevel);
            currentLevel = {};
        }
        currentLevel.count++;
        if (n.node != nullptr)
        {
            auto box = generateBox(n.node);
            currentLevel.max_horizontal_length = std::max(currentLevel.max_horizontal_length, box[0].size());
            n.box = std::move(box);
        }
        currentLevel.level.push_back(n);
        currentLevel.depth = n.level;
        bfs.pop();
        //std::cout << "Node at level " << n.level << " " << n.node << "\n";
        if (n.node == nullptr)
            continue;
        if (n.node->left != nullptr)
            bfs.emplace(n.node->left, n.level + 1);
        else
            bfs.emplace(nullptr, n.level + 1);
        
        if (n.node->right != nullptr)
            bfs.emplace(n.node->right, n.level + 1);
        else
            bfs.emplace(nullptr, n.level + 1);
    }
    std::vector<std::string> lines;
    size_t lineLength = 0;
    const size_t lineHeight = format.verticalPadding * 2 + 3;
    //std::cout << levels.size() << "\n";
    while (!levels.empty())
    {
        std::vector<std::string> currentLines;
        const auto& n = levels.top();
        
        for (const auto& b : n.level)
        {
            std::vector<std::string> box = b.box;
            if (b.node == nullptr || box.empty())
            {
                for (size_t i = 0; i < lineHeight; i++)
                    box.push_back(createPadding(n.max_horizontal_length));
            }
            if (currentLines.empty())
            {
                for (const auto& v : box)
                    currentLines.push_back(v);
            } else
            {
                BLT_ASSERT(currentLines.size() == box.size() && "Box lines should match current lines!");
                for (size_t i = 0; i < currentLines.size(); i++)
                {
                    currentLines[i] += createPadding(format.horizontalSpacing);
                    currentLines[i] += box[i];
                }
            }
        }
        std::int64_t padLength = (static_cast<std::int64_t>(lineLength) - static_cast<std::int64_t>(currentLines[0].length())) / 2;
        if (padLength < 0)
            padLength = 0;
        for (const auto& v : currentLines)
        {
            lineLength = std::max(lineLength, v.length());
            lines.push_back(createPadding(padLength) + v);
        }
        levels.pop();
        //if (!levels.empty())
        //    for (int i = 0; i < format.verticalSpacing; i++)
        //        lines.emplace_back("&");
    }
    
    size_t index = 1;
    size_t startLine = 0;
    size_t endLine = 0;
    while (index < lines.size() - 1)
    {
        auto& line = lines[index];
        size_t beginMarkerIndex = 0;
        size_t endMarkerIndex = 0;
        startLine = index++;
        beginMarkerIndex = line.find('%');
        if (beginMarkerIndex != std::string::npos)
        {
            // find endLine we need to connect with
            while (index < lines.size())
            {
                auto& line2 = lines[index];
                endMarkerIndex = line2.find('%');
                if (endMarkerIndex != std::string::npos)
                {
                    endLine = index;
                    break;
                }
                index++;
            }
            
            while (true)
            {
                if (beginMarkerIndex == std::string::npos || endMarkerIndex == std::string::npos)
                    break;
                
                BLT_TRACE(std::abs(static_cast<std::int64_t>(endMarkerIndex) - static_cast<std::int64_t>(beginMarkerIndex)));
                auto connector = createPadding(std::abs(static_cast<std::int64_t>(endMarkerIndex) - static_cast<std::int64_t>(beginMarkerIndex)), '-');
                auto frontPad = createPadding(std::min(beginMarkerIndex, endMarkerIndex));
                lines.insert(lines.begin() + static_cast<std::int64_t>(startLine) + 1, frontPad += connector);
                index++;
                
                beginMarkerIndex = line.find('%', beginMarkerIndex + 1);
                endMarkerIndex = line.find('%', endMarkerIndex + 1);
            }
            index++;
        }
    }
    
    std::reverse(lines.begin(), lines.end());
    return lines;
}

std::vector<std::string> blt::string::TreeFormatter::generateBox(blt::string::TreeFormatter::Node* node) const
{
    if (node == nullptr)
        return {};
    std::vector<std::string> lines;
    
    auto& data = node->data;
    auto dataLength = data.length();
    auto paddingLeft = format.horizontalPadding;
    auto paddingRight = format.horizontalPadding;
    auto totalLength = paddingLeft + paddingRight + dataLength;
    
    if (totalLength % 2 != 0)
    {
        paddingRight += 1;
        totalLength += 1;
    }
    
    // create horizontal line based on the calculated length
    std::string hline = createLine(totalLength, '+', '-');
    hline[(hline.size() - 1) / 2] = '%';
    std::string paddedLine = createLine(totalLength, '|', ' ');
    std::string dataStr;
    dataStr.reserve(totalLength);
    dataStr += '|';
    dataStr += createPadding(paddingLeft - 1);
    dataStr += data;
    dataStr += createPadding(paddingRight - 1);
    dataStr += '|';
    
    lines.push_back(hline);
    for (int i = 0; i < format.verticalPadding; i++)
        lines.push_back(paddedLine);
    lines.push_back(std::move(dataStr));
    for (int i = 0; i < format.verticalPadding; i++)
        lines.push_back(paddedLine);
    
    lines.push_back(std::move(hline));
    
    return lines;
}

std::string blt::string::createLine(size_t totalLength, char endingChar, char spacingChar)
{
    std::string line;
    line.reserve(totalLength);
    line += endingChar;
    line += createPadding(totalLength - 2, spacingChar);
    line += endingChar;
    return line;
}

std::string blt::string::createPadding(size_t length, char spacing)
{
    std::string padding;
    padding.reserve(length);
    for (size_t i = 0; i < length; i++)
        padding += spacing;
    return padding;
}
