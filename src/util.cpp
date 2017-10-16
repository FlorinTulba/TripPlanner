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

#include <sstream>
#include <iomanip>
#include <cassert>

using namespace std;
using namespace boost::posix_time;

string formatTimePoint(const ptime &mom) {
	const auto timePart = mom.time_of_day();
	const auto datePart = mom.date();
	const auto ymd = datePart.year_month_day();
	ostringstream oss;
	oss<<datePart.day_of_week().as_short_string()<<' '
		<<ymd.month.as_short_string()<<'-'
		<<ymd.day<<'-'
		<<ymd.year<<' '
		<<setw(2)<<setfill('0')<<timePart.hours()<<':'
		<<setw(2)<<setfill('0')<<timePart.minutes();
	return oss.str();
}

string formatDuration(const time_duration& dur) {
	const auto asHours = dur.hours();
	const div_t days_hours = div(asHours, 24);
	const auto mins = dur.minutes();

	ostringstream oss;
	bool daysPartPresent = false;
	if(days_hours.quot > 0) {
		daysPartPresent = true;
		oss<<days_hours.quot<<" day";
		if(days_hours.quot > 1)
			oss<<'s';
		oss<<", ";
	}
	if(days_hours.rem > 0 || daysPartPresent) {
		oss<<days_hours.rem<<" hour";
		if(days_hours.rem != 1)
			oss<<'s';
		oss<<" and ";
	}
	oss<<mins<<" minute";
	if(mins != 1)
		oss<<'s';

	return oss.str();
}
