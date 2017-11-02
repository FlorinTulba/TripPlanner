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

#include "transpModes.h"

#pragma warning ( push, 0 )

#include <map>
#include <sstream>
#include <stdexcept>

#pragma warning ( pop )

using namespace std;

// namespace trip planner - specifications
namespace tp { namespace specs {

  const char* TranspModes::toString(size_t masksCombination) {
	  static const char* combinations[] {
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

	  static constexpr size_t namesCount = sizeof(combinations) / sizeof(char*);
	  if(masksCombination == 0ULL || masksCombination >= namesCount) {
		  ostringstream oss;
		  oss<<__func__<<" accepts values between [1,"
			  <<namesCount<<"]. Received instead: "<<masksCombination;
		  throw invalid_argument(oss.str());
	  }

	  return combinations[masksCombination];
  }

  size_t TranspModes::fromString(const char* description) {
	  static const map<string, size_t> maskForDescription = []() {
		  map<string, size_t> result;
		  for(size_t i = 1, lim = size_t(TranspModes::last<<1); i < lim; ++i)
			  result.emplace(string(toString(i)), i);
		
		  return result;
	  } ();

    try {
      return maskForDescription.at(description);
    } catch(out_of_range&) {
      ostringstream oss;
      oss<<__func__<<" was unable to recognize the transport mode combination: "
        <<description;
      throw invalid_argument(oss.str());
    }
  }

}} // namespace tp::specs
