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
#include <set>

#include <boost/date_time/posix_time/ptime.hpp>

/**
Returns the current moment in UTC.

If necessary, it should be able to provide a sequence of constants,
instead of the actual current moment. See the nowReplacements from below.

Various scenarios might need reusing specific moments in time,
to investigate or check the behavior of the system.

This mechanism allows going into the past / future, particularly in Unit tests.
*/
boost::posix_time::ptime nowUTC();

// Let just 1 unit instantiate this template used below
extern template std::vector<boost::posix_time::ptime>;

/**
Allows using the values from this container
instead of actually reading the current moment.

The values to be reported are always read from the back of the vector.
Between consecutive read operations, the vector can be updated
dynamically in any way.
*/
extern std::vector<boost::posix_time::ptime> nowReplacements;

/// Converting a moment object to string: "shortDayName shortMonthName-day-year hours:minutes"
std::string formatTimePoint(const boost::posix_time::ptime &mom);

/// Converting a duration object to string: [[DDD day[s], ]H hour[s] and ]M minute[s]
std::string formatDuration(const boost::posix_time::time_duration &dur);

/**
Replacing the content of udyaSet with the dates from udyaStr.
The dates are delimited by '|' among 0 or more space-like symbols.
The dates referring to months before current month actually point to
those months from the next year.
*/
void updateUnavailDaysForTheYearAhead(const std::string &udyaStr,
									  std::set<boost::gregorian::date> &udyaSet);
