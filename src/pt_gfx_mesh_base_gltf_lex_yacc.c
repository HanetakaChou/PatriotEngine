//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

// Shift/Reduce Conflicts
// https://www.gnu.org/software/bison/manual/html_node/Shift_002fReduce.html

// How to debug

// example
//
// %token NUMBER
// %token PLUS
// %type expession
// %start expession
// %%
// expession: expession PLUS expession
// expession: NUMBER
// %%

// bison
// add "-r all" option and generate the "*.output" file

// when there is a "%type"(nonterminal) right after the "."
// we should consider all the results which can be deduced
// and we should focus whether the shift and reduce can be distinguished by the first "%token"(the LA(1))
// the shift and reduce can exist at the same time as long as they can be distinguished by the first "%token"(the LA(1))

// search "State * conflicts: * shift/reduce" in "*.output" file
// in this case, we have "State 5 conflicts: 1 shift/reduce"

// search "State 5", we have "State 4" --expession--> "State 5"
// search "State 4", we have "State 2" --PLUS--> "State 4"
// ...
// finally, we have the chain "State 2" --PLUS--> "State 4" --expession--> "State 5"

// It should be noted that the "%type"(nonterminal) is right after "." in "State 2" and thus the deduction should be applied
//
// State 4

//    1 expession: . expession PLUS expession // which means expession PLUS (. expession PLUS expession) // deduction appiled
//    1          | expession PLUS . expession
//    2          | . NUMBER

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

