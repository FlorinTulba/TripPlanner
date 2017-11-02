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

#include "placeBase.h"

using namespace std;

// namespace trip planner - specifications
namespace tp { namespace specs {

#if (defined(_MSC_VER) || defined(__clang__)) && !defined(__GNUC__)
  template GpsCoord<float>;
  template GpsCoord<double>;
  template GpsCoord<long double>;
#endif // (_MSC_VER || __clang__) && !__GNUC__

  string IfPlace::toString() const {
	  ostringstream oss; // showing the most well known name
    oss<<"Place{ id="<<id()<<"; name='"<<names().front()<<'\'';
    const string &descr = shortDescr();
    if(!descr.empty())
      oss<<" - "<<descr; 
    oss<<", gps="<<gpsCoord().toString(true)<<" }"; // in degrees
	  return oss.str();
  }

}} // namespace tp::specs
