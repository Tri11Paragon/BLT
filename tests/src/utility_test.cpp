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
    tableTest.addColumn("Thing");
    tableTest.addColumn("Value");
    
    tableTest.addRow({"Cuddles", "1 / minute"});
    tableTest.addRow({"Hand Job", "10"});
    tableTest.addRow({"Head", "100"});
    tableTest.addRow({"Sleeping Together (Non-Sexual)", "1,000"});
    tableTest.addRow({"Actual Sex", "5,000"});
    tableTest.addRow({"Sleeping Together (Sexual)", "10,000"});
    tableTest.addRow({"Relationship (I would do anything for you)", "1,000,000,000,000"});
    
    
    printLines(tableTest.createTable(true, true));
    
    blt::string::BinaryTreeFormatter::TreeFormat format;
    format.horizontalPadding = 3;
    format.verticalPadding = 0;
    format.horizontalSpacing = 3;
    format.collapse = true;
    blt::string::BinaryTreeFormatter treeFormatter("I love Men", format);
    treeFormatter.getRoot()->with(
            (new string::BinaryTreeFormatter::Node("Guys"))->with(nullptr, new string::BinaryTreeFormatter::Node("Child2")),
            (new string::BinaryTreeFormatter::Node("Femboys"))
                    ->with(nullptr, new string::BinaryTreeFormatter::Node("Child1")));
    printLines(treeFormatter.construct());
    
    blt::string::BinaryTreeFormatter assign1("price > 50", format);
    assign1.getRoot()->with(
            // left
            (new string::BinaryTreeFormatter::Node("member"))
                ->with((new string::BinaryTreeFormatter::Node("total -= total * 0.15"))
                        ->with((new string::BinaryTreeFormatter::Node("total > 500"))->with(new string::BinaryTreeFormatter::Node("total -= 25"))),
                       (new string::BinaryTreeFormatter::Node("total -= total * 0.05"))),
            // right
            (new string::BinaryTreeFormatter::Node("quality"))
                ->with((new string::BinaryTreeFormatter::Node("total -= total * 0.02")),
                       (new string::BinaryTreeFormatter::Node("total -= total * 0.05")))
            );
    printLines(assign1.construct());
    
    blt::string::TableFormatter tableQ2i1("Iteration 0");
    tableQ2i1.addColumn("Statement");
    tableQ2i1.addColumn("IN");
    tableQ2i1.addColumn("OUT");
    tableQ2i1.addRow({"Entry (0)", "{}", "{}"});
    tableQ2i1.addRow({"(1)", "{}", "{}"});
    tableQ2i1.addRow({"(2)", "{}", "{}"});
    tableQ2i1.addRow({"(3)", "{}", "{}"});
    tableQ2i1.addRow({"(4)", "{}", "{}"});
    tableQ2i1.addRow({"(5)", "{}", "{}"});
    tableQ2i1.addRow({"Exit (6)", "{}", "{}"});
    
    printLines(tableQ2i1.createTable(true, true));
}
