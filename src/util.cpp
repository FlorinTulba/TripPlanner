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

#include "util.h"

#pragma warning ( push, 0 )

#include <regex>
#include <algorithm>
#include <iterator>

#pragma warning ( pop )

using namespace std;

const void* addrUnreferenced;

#if (defined(_MSC_VER) || defined(__clang__)) && !defined(__GNUC__)
template vector<string>; // forced template instantiation
#endif // (_MSC_VER || __clang__) && !__GNUC__
vector<string> tokenize(const string &s,
                        const string &regexDelimStr/* = R"(\s+)"*/) {
	vector<string> tokens;
	if(s.empty()) // without this test, the resulted vector will contain one empty string
		return tokens;

	const regex regexDelim(regexDelimStr);
	copy(sregex_token_iterator(CBOUNDS(s), regexDelim, -1), {}, back_inserter(tokens));
	return tokens;
}

static string trimHelper(const string &s, const regex &trimRE) {
  smatch matchRE;
  if(!regex_search(s, matchRE, trimRE) || matchRE.size() < 2ULL)
    throw logic_error(string(__func__) + " couldn't extract the trim pattern!");
  return matchRE[1ULL].str();
}

string trim(const string &s) {
  static const regex trimRE(R"(^\s*((\S.*\S)?)\s*$)");
  return trimHelper(s, trimRE);
}

string ltrim(const string &s) {
  static const regex trimRE(R"(^\s*((\S[\s\S]*)?)$)");
  return trimHelper(s, trimRE);
}

string rtrim(const string &s) {
  static const regex trimRE(R"(^(([\s\S]*\S)?)\s*$)");
  return trimHelper(s, trimRE);
}

#if (defined(_MSC_VER) || defined(__clang__)) && !defined(__GNUC__)
template radians<float>;
template radians<double>;
template radians<long double>;
#endif // (_MSC_VER || __clang__) && !__GNUC__

constexpr radians<long double> operator"" _deg(long double degrees) {
  return radians<long double>::fromDegrees(degrees);
}
