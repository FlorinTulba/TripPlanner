/*****************************************************************************
 TripPlanner explores various issues common to navigation and booking systems.

 Copyrights from the libraries used by the program:
 - (c) 2017 Boost (www.boost.org)
		License: <http://www.boost.org/LICENSE_1_0.txt>
 
 (c) 2017 Florin Tulba <florintulba@yahoo.com>

 This program is free software: you can use its results,
 redistribute it and/or modify it under the terms of the GNU
 Affero General Public License version 3 as published by the
 Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program ('agpl-3.0.txt').
 If not, see <http://www.gnu.org/licenses/agpl-3.0.txt>.
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>

/// Prevents warnings about unused parameters
#define UNREFERENCED(p) p;

/// Specifying container ranges
#define CBOUNDS(cont) std::cbegin(cont), std::cend(cont)
#define CRBOUNDS(cont) std::crbegin(cont), std::crend(cont)
#define BOUNDS(cont) std::begin(cont), std::end(cont)
#define RBOUNDS(cont) std::rbegin(cont), std::rend(cont)

// Let just 1 unit instantiate this template used below
extern template std::vector<std::string>;

/// Tokenizes a string based on the provided regex delimiter expression
/// Default delimiter is one or more space-like characters
std::vector<std::string> tokenize(const std::string &s,
								  const std::string &regexDelimStr = R"(\s+)");
