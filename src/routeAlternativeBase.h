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

#include <vector>
#include <set>
#include <bitset>
#include <memory>

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/time_period.hpp>

/// Provides specific information about an alternative for a given route
struct IRouteAlternative abstract {
	virtual ~IRouteAlternative() = 0 {}

	virtual unsigned id() const = 0;	///< unique id

	/// Id of the routes bundle with common information relevant for this alternative
	virtual unsigned bundleId() const = 0;

	virtual bool returnTrip() const = 0;	///< represents a return trip

	/// Operational days of a week
	virtual std::shared_ptr<std::bitset<7>> operationalDaysOfWeek() const = 0;
	
	/// The days from the year ahead when this transport is not available
	virtual std::shared_ptr<std::set<boost::gregorian::date>>
		unavailDaysForTheYearAhead() const = 0;

	/// capacity of economy class seats
	virtual unsigned economySeatsCapacity() const = 0;

	/// capacity of business class seats
	virtual unsigned businessSeatsCapacity() const = 0;

	/// The timetable for this alternative of the route
	virtual const std::vector<boost::posix_time::time_period>& timetable() const = 0;
};
