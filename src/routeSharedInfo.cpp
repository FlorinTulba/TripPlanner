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

#include "routeSharedInfo.h"
#include "customDateTimeProcessor.h"
#include "util.h"

#pragma warning ( push, 0 )

#include <sstream>
#include <algorithm>

#pragma warning ( pop )

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

// namespace trip planner - specifications
namespace tp { namespace specs {
    RouteSharedInfo::RouteCustomizableInfo::RouteCustomizableInfo(
			  const string &odw_, const string &udya_) :
		  /*
		  The days of the week are specified as 0(Sun), 1(Mon), ..., 6(Sat),
		  but they appear in the bit string at the position (6-corresponding value).
		  This means that the read bit string must be reversed before interpreting it.
		  When not provided, the transportation is available every day of the week.
		  */
		  odw(make_shared<bitset<7>>(string(CRBOUNDS(odw_)))),
		  udya(make_shared<set<date>>()) {
	  assert(nullptr != udya);
	  updateUnavailDaysForTheYearAhead(udya_, *udya);
  }

  shared_ptr<bitset<7>>
  RouteSharedInfo::RouteCustomizableInfo::operationalDaysOfWeek() const {
	  assert(nullptr != odw);
	  return odw;
  }

  shared_ptr<set<date>>
          RouteSharedInfo::RouteCustomizableInfo::unavailDaysForTheYearAhead() const {
	  assert(nullptr != udya);
	  return udya;
  }

  void RouteSharedInfo::validateStopIdx(size_t stopIdx) const {
	  if(stopIdx < _stopsCount)
		  return;

	  ostringstream oss;
	  oss<<__func__<<" uses an index ("<<stopIdx
		  <<") >= the limit ("<<_stopsCount<<")!";
	  throw out_of_range(oss.str());
  }

  RouteSharedInfo::RouteSharedInfo(
      unsigned id_, size_t transpMode_,
      unique_ptr<ITicketPriceCalculator> pricingEng_,
      const string &udya_/* = ""*/,
      const string &odw_/* = "1111111"*/) :
	  _id(id_), _transpMode(transpMode_),
	  routeCustomizableInfo(odw_, udya_),
	  pricingEng(move(pricingEng_)) {}

  unsigned RouteSharedInfo::id() const {
	  return _id;
  }

  size_t RouteSharedInfo::transpMode() const {
	  return _transpMode;
  }

  size_t RouteSharedInfo::stopsCount() const {
	  return _stopsCount;
  }

  bool RouteSharedInfo::containsStop(unsigned placeId) const {
	  return stopsSet.find(placeId) != stopsSet.cend();
  }

  unsigned RouteSharedInfo::nthStop(size_t stopIdx, bool returnTrip) const {
	  validateStopIdx(stopIdx);
	
	  if(returnTrip)
		  stopIdx = _stopsCount - 1ULL - stopIdx;

	  return stops[stopIdx];
  }

  const vector<unsigned>& RouteSharedInfo::traversedStops() const {
	  return stops;
  }

  const vector<float>& RouteSharedInfo::distances() const {
	  return _distances;
  }

  float RouteSharedInfo::nthDistance(size_t distIdx, bool returnTrip) const {
	  validateStopIdx(distIdx + 1ULL);

	  if(returnTrip)
		  distIdx = _stopsCount - 2ULL - distIdx;

	  return _distances[distIdx];
  }

  ITicketPriceCalculator& RouteSharedInfo::pricingEngine() const {
	  assert(nullptr != pricingEng);
	  return *pricingEng;
  }

  const set<unsigned>& RouteSharedInfo::alternatives() const {
	  return _alternatives;
  }

  const IRouteCustomizableInfo& RouteSharedInfo::customizableInfo() const {
	  return routeCustomizableInfo;
  }

  void RouteSharedInfo::setFirstPlace(unsigned placeId) {
	  if(!stops.empty())
		  throw logic_error(string(__func__) + " cannot be called twice!");

	  _stopsCount = 1ULL;
	  stopsSet.insert(placeId);
	  stops.push_back(placeId);
  }

  void RouteSharedInfo::setNextStop(unsigned placeId, float distToPrevStop) {
	  if(stops.empty())
		  throw logic_error(string(__func__) + " must be called after setFirstPlace()!");

	  if(false == stopsSet.insert(placeId).second)
		  throw domain_error(string(__func__) + " detected a duplicate place id: " +
                         to_string(placeId));

	  stops.push_back(placeId);
	  _distances.push_back(distToPrevStop);
	  ++_stopsCount;
  }

  void RouteSharedInfo::addAlternative(unsigned alternativeId) {
	  if(false == _alternatives.insert(alternativeId).second)
      throw domain_error(string(__func__) +
                         " detected a duplicate alternative id: " +
                         to_string(alternativeId));
  }

}} // namespace tp::specs
