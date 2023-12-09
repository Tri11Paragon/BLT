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
    std::cout << std::endl;
}

int testFunc(int a, int b)
{
    if (a == b)
    {
        a = 2 * b;
        b = 3 * a;
    }
    
    return a;
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
                                   ->with((new string::BinaryTreeFormatter::Node("total > 500"))
                                                  ->with(new string::BinaryTreeFormatter::Node("total -= 25"))),
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
    tableQ2i1.addRow({"Entry (0)", " -- ", "{<factor, ?>, <factorial, ?>}"});
    tableQ2i1.addRow({"(1)", "{<factor, ?>, <factorial, ?>}", "{<factor, 1>, <factorial, ?>}"});
    tableQ2i1.addRow({"(2)", "{<factor, 1>, <factorial, ?>}", "{<factor, 2>, <factorial, 2>}"});
    tableQ2i1.addRow({"(3)", "{<factor, 1>, <factorial, 2>}", "{}"});
    tableQ2i1.addRow({"(4)", "{}", "{}"});
    tableQ2i1.addRow({"(5)", "{}", "{}"});
    tableQ2i1.addRow({"Exit (6)", "{}", " -- "});
    
    printLines(tableQ2i1.createTable(true, true));
    
    blt::string::TableFormatter tableQ2i2("Iteration 1");
    tableQ2i2.addColumn("Statement");
    tableQ2i2.addColumn("IN");
    tableQ2i2.addColumn("OUT");
    tableQ2i2.addRow({"Entry (0)", " -- ", "{<factor, ?>, <factorial, ?>}"});
    tableQ2i2.addRow({"(1)", "{<factor, ?>, <factorial, ?>}", "{<factor, 1>, <factorial, ?>}"});
    tableQ2i2.addRow({"(2)", "{<factor, 1>, <factorial, ?>}", "{<factor, 2>, <factorial, 2>}"});
    tableQ2i2.addRow({"(3)", "{<factor, 1>, <factorial, 2>}", "{<factor, 1>, <factorial, 2>}"});
    tableQ2i2.addRow({"(4)", "{<factor, 1>, <factorial, 2>}", "{<factor, 1>, <factorial, 4>}"});
    tableQ2i2.addRow({"(5)", "{<factor, 1>, <factorial, 4>}", "{<factor, 5>, <factorial, 4>}"});
    tableQ2i2.addRow({"Exit (6)", "{<factor, 5>, <factorial, 4>}", " -- "});
    
    printLines(tableQ2i2.createTable(true, true));
    
    blt::string::TableFormatter tableQ2i3("Iteration 2");
    tableQ2i3.addColumn("Statement");
    tableQ2i3.addColumn("IN");
    tableQ2i3.addColumn("OUT");
    tableQ2i3.addRow({"Entry (0)", " -- ", "{<factor, ?>, <factorial, ?>}"});
    tableQ2i3.addRow({"(1)", "{<factor, ?>, <factorial, ?>}", "{<factor, 1>, <factorial, ?>}"});
    tableQ2i3.addRow({"(2)", "{<factor, 1>, <factorial, ?>}", "{<factor, 2>, <factorial, 2>}"});
    tableQ2i3.addRow({"(3)", "{<factor, 5>, <factorial, 4>}", "{<factor, 5>, <factorial, 4>}"});
    tableQ2i3.addRow({"(4)", "{<factor, 5>, <factorial, 4>}", "{<factor, 5>, <factorial, 4>}"});
    tableQ2i3.addRow({"(5)", "{<factor, 5>, <factorial, 4>}", "{<factor, 5>, <factorial, 4>}"});
    tableQ2i3.addRow({"Exit (6)", "{<factor, 5>, <factorial, 4>}", " -- "});
    
    printLines(tableQ2i3.createTable(true, true));
    
    blt::string::TableFormatter tableQ2i4("Iteration 3");
    tableQ2i4.addColumn("Statement");
    tableQ2i4.addColumn("IN");
    tableQ2i4.addColumn("OUT");
    tableQ2i4.addRow({"Entry (0)", " -- ", "{<factor, ?>, <factorial, ?>}"});
    tableQ2i4.addRow({"(1)", "{<factor, ?>, <factorial, ?>}", "{<factor, 1>, <factorial, ?>}"});
    tableQ2i4.addRow({"(2)", "{<factor, 1>, <factorial, ?>}", "{<factor, 2>, <factorial, 2>}"});
    tableQ2i4.addRow({"(3)", "{<factor, 5>, <factorial, 4>}", "{<factor, 5>, <factorial, 4>}"});
    tableQ2i4.addRow({"(4)", "{<factor, 5>, <factorial, 4>}", "{<factor, 5>, <factorial, 4>}"});
    tableQ2i4.addRow({"(5)", "{<factor, 5>, <factorial, 4>}", "{<factor, 5>, <factorial, 4>}"});
    tableQ2i4.addRow({"Exit (6)", "{<factor, 5>, <factorial, 4>}", " -- "});
    
    printLines(tableQ2i4.createTable(true, true));
    
    blt::string::TableFormatter tableQ4i0("Q4 Iteration 0");
    tableQ4i0.addColumn("Statement");
    tableQ4i0.addColumn("IN");
    tableQ4i0.addColumn("OUT");
    tableQ4i0.addRow({"Entry (1)", " -- ", " {} "});
    tableQ4i0.addRow({"(2)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"(3)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"(4)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"(5)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"(6)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"(7)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"(8)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"(9)", "{a + b, a * b, a - b, b + 1}", "{a + b, a * b, a - b, b + 1}"});
    tableQ4i0.addRow({"Exit (10)", "{a + b, a * b, a - b, b + 1}", " -- "});
    
    printLines(tableQ4i0.createTable(true, true));
    
    blt::string::TableFormatter tableQ4i1("Q4 Iteration 1");
    tableQ4i1.addColumn("Statement");
    tableQ4i1.addColumn("IN");
    tableQ4i1.addColumn("OUT");
    tableQ4i1.addRow({"Entry (1)", " -- ", " {} "});
    tableQ4i1.addRow({"(2)", "{}", "{}"});
    tableQ4i1.addRow({"(3)", "{}", "{}"});
    tableQ4i1.addRow({"(4)", "{}", "{a + b}"});
    tableQ4i1.addRow({"(5)", "{a + b}", "{a + b, a * b}"});
    tableQ4i1.addRow({"(6)", "{a + b, a * b}", "{a + b, a * b, a - b}"});
    tableQ4i1.addRow({"(7)", "{a + b, a * b, a - b}", "{}"});
    tableQ4i1.addRow({"(8)", "{}", "{a + b}"});
    tableQ4i1.addRow({"(9)", "{a + b}", "{a + b, a * b}"});
    tableQ4i1.addRow({"Exit (10)", "{a + b, a * b, a - b}", " -- "});
    
    printLines(tableQ4i1.createTable(true, true));
    
    blt::string::TableFormatter tableQ4i2("Q4 Iteration 2");
    tableQ4i2.addColumn("Statement");
    tableQ4i2.addColumn("IN");
    tableQ4i2.addColumn("OUT");
    tableQ4i2.addRow({"Entry (1)", " -- ", " {} "});
    tableQ4i2.addRow({"(2)", "{}", "{}"});
    tableQ4i2.addRow({"(3)", "{}", "{}"});
    tableQ4i2.addRow({"(4)", "{}", "{a + b}"});
    tableQ4i2.addRow({"(5)", "{a + b}", "{a + b, a * b}"});
    tableQ4i2.addRow({"(6)", "{a + b, a * b}", "{a + b, a * b, a - b}"});
    tableQ4i2.addRow({"(7)", "{a + b, a * b, a - b}", "{}"});
    tableQ4i2.addRow({"(8)", "{}", "{a + b}"});
    tableQ4i2.addRow({"(9)", "{a + b}", "{a + b, a * b}"});
    tableQ4i2.addRow({"Exit (10)", "{a + b, a * b, a - b}", " -- "});
    
    printLines(tableQ4i2.createTable(true, true));
}
