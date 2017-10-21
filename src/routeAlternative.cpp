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

#include "routeAlternative.h"
#include "customDateTimeProcessor.h"
#include "util.h"

#include <boost/date_time/gregorian/parsers.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

RouteAlternative::RouteAlternative(unsigned id_, IRoutesBundle &rb,
								   unsigned esa_, unsigned bsa_,
								   const string &timetable_,
								   bool returnTrip_) :
		_rb(rb), odw(rb.operationalDaysOfWeek()), udya(rb.unavailDaysForTheYearAhead()),
		_id(id_), _returnTrip(returnTrip_), esa(esa_), bsa(bsa_) {
	updateTimetable(timetable_);
}

unsigned RouteAlternative::id() const {
	return _id;
}

unsigned RouteAlternative::bundleId() const {
	return _rb.id();
}

bool RouteAlternative::returnTrip() const {
	return _returnTrip;
}

shared_ptr<bitset<7>> RouteAlternative::operationalDaysOfWeek() const {
	assert(nullptr != odw);
	return odw;
}

shared_ptr<set<date>> RouteAlternative::unavailDaysForTheYearAhead() const {
	assert(nullptr != udya);
	return udya;
}

unsigned RouteAlternative::economySeatsCapacity() const {
	return esa;
}

unsigned RouteAlternative::businessSeatsCapacity() const {
	return bsa;
}

const vector<time_period>& RouteAlternative::timetable() const {
	return _timetable;
}

void RouteAlternative::updateUnavailDaysForTheYearAhead(const string &udya_) {
	udya = make_shared<set<date>>();
	::updateUnavailDaysForTheYearAhead(udya_, *udya);
}

void RouteAlternative::updateOperationalDaysOfWeek(const string &odw_) {
	odw = make_shared<bitset<7>>(string(CRBOUNDS(odw_)));
}

void RouteAlternative::updateTimetable(const string &timetable_) {
	_timetable.clear();

	// Constructing ptime requires also a Gregorian date value
	// The actual date value is not important, as only the hours and minutes matter.
	static const date aDate(from_simple_string("2017-Jan-1"));

	ostringstream oss; // to report eventual errors

	// The intervals are separated by '|' among 0 or more space-like symbols
	const vector<string> intervals = tokenize(timetable_, R"(\s*\|\s*)");
	if(intervals.size() + 1ULL != _rb.stopsCount()) {
		oss<<"Current route bundle involves "<<_rb.stopsCount()<<" stops. "
			"However, the timetable `"<<timetable_<<"` presents a different situation.";
		throw domain_error(oss.str());
	}

	for(const string &interval : intervals) {
		// Each interval contains 2 time moments separated by '-' among 0 or more space-like symbols
		const vector<string> moments = tokenize(interval, R"(\s*-\s*)");
		if(moments.size() != 2ULL) {
			oss<<"All time intervals from a timetable need 2 moments. "
				"This doesn't happen in `"<<timetable_<<'`';
			throw domain_error(oss.str());
		}

		const ptime t1(aDate, duration_from_string(moments.front()));
		const ptime t2(aDate, duration_from_string(moments.back()));

		if(t1 >= t2 || (!_timetable.empty() &&
						_timetable.back().last() >= t1)) {
			oss<<"The times need to be distinct and ordered in: `"<<timetable_<<'`';
			throw domain_error(oss.str());
		}
		_timetable.emplace_back(t1, t2 + time_duration::unit());
	}
}
