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

#include "constraints.h"

#include <cassert>
#include <stdexcept>

using namespace std;
using namespace boost::posix_time;

TimeConstraints::TimeConstraints(const time_period &leavePeriod_/* =
									time_period(
										nowUTC(),
										hours(366 * 24))*/,
								 const time_period &arrivePeriod_/* =
									time_period(
										nowUTC(),
										hours(366 * 24))*/) :
		_leavePeriod(leavePeriod_.begin(), leavePeriod_.end() + time_duration::unit()),
		_arrivePeriod(arrivePeriod_.begin(), arrivePeriod_.end() + time_duration::unit()) {
	if(_leavePeriod.begin() > _arrivePeriod.begin() ||
	   _leavePeriod.last() > _arrivePeriod.last())
		throw invalid_argument(__FUNCTION__
							   " leavePeriod_ must begin before arrivePeriod_ starts"
							   " and must end before arrivePeriod_ ends");
}

const time_period& TimeConstraints::leavePeriod() const {
	return _leavePeriod;
}

const time_period& TimeConstraints::arrivePeriod() const {
	return _arrivePeriod;
}
