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

#include "planner.h"
#include "constraints.h"
#include "results.h"

using namespace std;

static TimeConstraints defaultConstraints;

TripPlanner::TripPlanner(const string &dataPath) {}

unique_ptr<IResults> TripPlanner::search(const string &fromPlace, 
										 const string &toPlace,
										 size_t maxCountPerCategory,
										 const ITimeConstraints *timeConstraints/* = nullptr*/) const {
	if(fromPlace.compare(toPlace) == 0 || maxCountPerCategory == 0ULL)
		return nullptr; // the 2 places coincide or no results are allowed

	const ITimeConstraints &constraints =
		(nullptr != timeConstraints) ? *timeConstraints : defaultConstraints;
	
	unique_ptr<IResults> results = make_unique<Results>();
	return results;
}
