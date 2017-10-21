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

#include "routesBundle.h"
#include "customDateTimeProcessor.h"
#include "util.h"

#include <sstream>
#include <algorithm>

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

void RoutesBundle::validateStopIdx(size_t stopIdx) const {
	if(stopIdx < _stopsCount)
		return;

	ostringstream oss;
	oss<<__FUNCTION__ " uses an index ("<<stopIdx
		<<") >= the limit ("<<_stopsCount<<")!";
	throw out_of_range(oss.str());
}

RoutesBundle::RoutesBundle(unsigned id_, size_t transpMode_, 
						 unique_ptr<ITicketPriceCalculator> pricingEng_,
						 const string &odw_/* = "1111111"*/) :
		_id(id_), _transpMode(transpMode_),
		udya(make_shared<set<date>>()),
		pricingEng(move(pricingEng_)) {
	/*
	The days of the week are specified as 0(Sun), 1(Mon), ..., 6(Sat),
	but they appear in the bit string at the position (6-corresponding value).
	This means that the read bit string must be reversed before interpreting it.
	When not provided, the transportation is available every day of the week.
	*/
	odw = make_shared<bitset<7>>(string(CRBOUNDS(odw_)));
}

unsigned RoutesBundle::id() const {
	return _id;
}

size_t RoutesBundle::transpMode() const {
	return _transpMode;
}

shared_ptr<bitset<7>> RoutesBundle::operationalDaysOfWeek() const {
	assert(nullptr != odw);
	return odw;
}

shared_ptr<set<date>> RoutesBundle::unavailDaysForTheYearAhead() const {
	assert(nullptr != udya);
	return udya;
}

size_t RoutesBundle::stopsCount() const {
	return _stopsCount;
}

bool RoutesBundle::containsStop(unsigned placeId) const {
	return stopsSet.find(placeId) != stopsSet.cend();
}

unsigned RoutesBundle::nthStop(size_t stopIdx, bool returnTrip) const {
	validateStopIdx(stopIdx);
	
	if(returnTrip)
		stopIdx = _stopsCount - 1ULL - stopIdx;

	return stops[stopIdx];
}

const vector<unsigned>& RoutesBundle::traversedStops() const {
	return stops;
}

const vector<float>& RoutesBundle::distances() const {
	return _distances;
}

float RoutesBundle::nthDistance(size_t distIdx, bool returnTrip) const {
	validateStopIdx(distIdx + 1ULL);

	if(returnTrip)
		distIdx = _stopsCount - 2ULL - distIdx;

	return _distances[distIdx];
}

ITicketPriceCalculator& RoutesBundle::pricingEngine() const {
	assert(nullptr != pricingEng);
	return *pricingEng;
}

const set<unsigned>& RoutesBundle::alternatives() const {
	return _alternatives;
}

void RoutesBundle::updateUnavailDaysForTheYearAhead(const string &udya_) {
	assert(nullptr != udya);
	::updateUnavailDaysForTheYearAhead(udya_, *udya);
}

void RoutesBundle::setFirstPlace(unsigned placeId) {
	if(!stops.empty()) {
		ostringstream oss;
		oss<<__FUNCTION__ " cannot be called twice!";
		throw logic_error(oss.str());
	}

	_stopsCount = 1ULL;
	stopsSet.insert(placeId);
	stops.push_back(placeId);
}

void RoutesBundle::setNextStop(unsigned placeId, float distToPrevStop) {
	if(stops.empty()) {
		ostringstream oss;
		oss<<__FUNCTION__ " must be called after setFirstPlace()!";
		throw logic_error(oss.str());
	}

	if(false == stopsSet.insert(placeId).second) {
		ostringstream oss;
		oss<<__FUNCTION__ " detected a duplicate place id: "<<placeId;
		throw domain_error(oss.str());
	}

	stops.push_back(placeId);
	_distances.push_back(distToPrevStop);
	++_stopsCount;
}

void RoutesBundle::addAlternative(unsigned alternativeId) {
	if(false == _alternatives.insert(alternativeId).second) {
		ostringstream oss;
		oss<<__FUNCTION__ " detected a duplicate alternative id: "<<alternativeId;
		throw domain_error(oss.str());
	}
}
