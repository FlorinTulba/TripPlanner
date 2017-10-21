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

#include "pricingBase.h"

#include <set>
#include <vector>
#include <bitset>
#include <memory>

#include <boost/date_time/gregorian/greg_date.hpp>

/// Common information relevant for several alternatives of a route
struct IRoutesBundle abstract {
	virtual ~IRoutesBundle() = 0 {}

	virtual unsigned id() const = 0;	///< unique id

	virtual size_t transpMode() const = 0;	///< air/road/rail/water. @see TranspModes

	/// Operational days of a week
	virtual std::shared_ptr<std::bitset<7>> operationalDaysOfWeek() const = 0;
	
	/// The days from the year ahead when this transport is not available
	virtual std::shared_ptr<std::set<boost::gregorian::date>>
		unavailDaysForTheYearAhead() const = 0;

	virtual size_t stopsCount() const = 0;	///< number of stops on the route

	/// Checks if the transport stops at a certain place
	virtual bool containsStop(unsigned placeId) const = 0;

	/// @return the id of the place where the route has its stopIdx-th stop
	/// when traversing the routes in normal or reversed order (for returnTrip = true)
	virtual unsigned nthStop(size_t stopIdx, bool returnTrip) const = 0;

	/// The stops on the route
	virtual const std::vector<unsigned>& traversedStops() const = 0;

	/// The distances between consecutive stops
	virtual const std::vector<float>& distances() const = 0;

	/// @return the value of the distIdx-th distance to traverse
	/// in normal or reversed order (for returnTrip = true)
	virtual float nthDistance(size_t distIdx, bool returnTrip) const = 0;

	/// Provides a calculator for ticket prices
	virtual ITicketPriceCalculator& pricingEngine() const = 0;

	/// The id-s of the alternatives of this route (different schedule, capacity..., but same route)
	virtual const std::set<unsigned>& alternatives() const = 0;
};
