/*
 *  <Short Description>
 *  Copyright (C) 2023  Brett Terpstra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <blt/std/utility.h>
#include <blt/std/format.h>
#include <blt/std/logging.h>
#include <utility_test.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <limits>

std::optional<int> get()
{
    return 10;
}

void printLines(const std::vector<std::string>& lines)
{
    for (const auto& v : lines)
        std::cout << v << "\n";
}

void blt::test::utility::run()
{
    //std::vector<int> temp;
    std::optional<int> hi(10);
    if (auto test = get())
    {
    
    }
    
    blt::string::TableFormatter tableTest("Intrinsic Action Value Table");
    tableTest.addColumn({"Thing"});
    tableTest.addColumn({"Value"});
    
    tableTest.addRow({"Cuddles", "1 / minute"});
    tableTest.addRow({"Hand Job", "10"});
    tableTest.addRow({"Head", "100"});
    tableTest.addRow({"Sleeping Together (Non-Sexual)", "1,000"});
    tableTest.addRow({"Actual Sex", "5,000"});
    tableTest.addRow({"Sleeping Together (Sexual)", "10,000"});
    tableTest.addRow({"Relationship (I would do anything for you)", "1,000,000,000,000"});
    
    
    printLines(tableTest.createTable(true, true));
    
    
    blt::string::TreeFormatter treeFormatter("I love Men");
    treeFormatter.getRoot()->with(
            (new string::TreeFormatter::Node("Guys"))
                    ->with(new string::TreeFormatter::Node("Child1"), nullptr),
            (new string::TreeFormatter::Node("Femboys"))
                    ->with(nullptr, new string::TreeFormatter::Node("Child2")));
    printLines(treeFormatter.construct());
    
    
}