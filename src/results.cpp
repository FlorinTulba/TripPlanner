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

#include "results.h"
#include "variants.h"

#include <stdexcept>
#include <cassert>

using namespace std;

const string& TranspModes::toString(size_t masksCombination) {
	static const vector<string> combinations {
		"None",	// 0
		"Air",	// 1
		"Rail",	// 2
		"Air+Rail",	// 3
		"Road",	// 4
		"Air+Road", // 5
		"Rail+Road", // 6
		"Air+Rail+Road", // 7
		"Water", // 8
		"Air+Water", // 9
		"Rail+Water", // 10
		"Air+Rail+Water", // 11
		"Road+Water", // 12
		"Air+Road+Water", // 13
		"Rail+Road+Water", // 14
		"Air+Rail+Road+Water" // 15
	};

	if(masksCombination == 0ULL || masksCombination >= combinations.size())
		throw invalid_argument(__FUNCTION__ " accepts values between [1,15]!");

	return combinations[masksCombination];
}

const vector<string>& variantCategories() {
	static const vector<string> categs {
		"most rapid variants",
		"cheapest variants",
		"shortest routes",
		"variants getting to the destination the soonest"
	};
	return categs;
}

Results::Results() {
	const size_t categories = variantCategories().size();
	results.reserve(categories);
	for(size_t i = 0ULL; i < categories; ++i)
		results.emplace_back(new Variants);
}

IVariants& Results::get(size_t categ) const {
	if(categ >= variantCategories().size())
		throw out_of_range(__FUNCTION__ " invalid categ!");

	assert(results[categ]);
	return *results[categ];
}

const IVariants& Results::operator[](size_t categ) const {
	return get(categ);
}

IVariants& Results::operator[](size_t categ) {
	return get(categ);
}

ostream& operator<<(ostream &os, const IResults &results) {
	const size_t categories = variantCategories().size();
	for(size_t i = 0ULL; i < categories; ++i) {
		os<<"Viewing the "<<variantCategories()[i]<<':'<<endl<<endl;
		os<<results[i];
		os<<endl<<string(80ULL, '=')<<endl<<endl<<endl;
	}

	return os;
}
