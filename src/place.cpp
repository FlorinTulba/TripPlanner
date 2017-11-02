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

#include "place.h"

#pragma warning ( push, 0 )

#include <set>
#include <stdexcept>

#pragma warning ( pop )

using namespace std;

// namespace trip planner - specifications
namespace tp { namespace specs {

  Place::Place(unsigned id_, const GpsCoord<float> &location,
               const string &names_, const string &shortDescr_/* = u8""*/) :
      _id(id_), coord(location),
      _names(tokenize(trim(names_), R"(\s*\|\s*)")),
      _shortDescr(trim(shortDescr_)) {
    if(_names.empty())
      throw invalid_argument(string(__func__) +
                             " needs at least one name for each place.");

    set<string> uniqueNames(CBOUNDS(_names));
    if(uniqueNames.size() != _names.size())
      throw invalid_argument(string(__func__) +
                             " needs non-duplicate names of the place. "
                             "Received instead: "s + names_);

    if(uniqueNames.find("") != cend(uniqueNames))
      throw invalid_argument(string(__func__) +
                             " needs non-empty names for the place. "
                             "Received instead: "s + names_);
  }

  unsigned Place::id() const {
	  return _id;
  }

  const GpsCoord<float>& Place::gpsCoord() const {
    return coord;
  }

  const vector<string>& Place::names() const {
    return _names;
  }

  const string& Place::shortDescr() const {
    return _shortDescr;
  }

}} // namespace tp::specs
