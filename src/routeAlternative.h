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

#include "routeAlternativeBase.h"
#include "routeSharedInfoBase.h"

 /// Provides specific information about an alternative for a given route
class RouteAlternative : public IRouteAlternative {
protected:
	/// Route shared information relevant for this alternative
	IRouteSharedInfo &_rsi;

	/// Operational days of a week
	std::shared_ptr<std::bitset<7>> odw;
	
	/// The days from the year ahead when this transport is not available
	std::shared_ptr<std::set<boost::gregorian::date>> udya;

	/// The timetable for this alternative of the route
	std::vector<boost::posix_time::time_period> _timetable;
	unsigned _id;		///< unique id
	unsigned esa;		///< capacity of economy class seats
	unsigned bsa;		///< capacity of business class seats
	bool _returnTrip;	///< represents a return trip

public:
	/**
	Builds an alternative of a route by setting its critical data:

	@param id_ unique id
	@param rsi route shared information relevant for this alternative
	@param esa_ capacity of economy class seats
	@param bsa_ capacity of business class seats
	@param timetable_ the stops schedule for this alternative of the route
	@param returnTrip_ states whether this is a return trip or not
	*/
	RouteAlternative(unsigned id_, IRouteSharedInfo &rsi, unsigned esa_, unsigned bsa_,
					 const std::string &timetable_, bool returnTrip_);

	// Getter methods

	unsigned id() const override; ///< unique id

	/// The route shared information relevant for this alternative
	const IRouteSharedInfo& routeSharedInfo() const override;

	bool returnTrip() const override;	///< represents a return trip

	/// Operational days of a week
	std::shared_ptr<std::bitset<7>> operationalDaysOfWeek() const override;
	
	/// The days from the year ahead when this transport is not available
	std::shared_ptr<std::set<boost::gregorian::date>>
		unavailDaysForTheYearAhead() const override;

	/// capacity of economy class seats
	unsigned economySeatsCapacity() const override;

	/// capacity of business class seats
	unsigned businessSeatsCapacity() const override;

	/**
	The timetable for this alternative of the route.
	These times are always traversed and kept in the forward direction,
	even for return trips.
	*/
	const std::vector<boost::posix_time::time_period>& timetable() const override;

	// Modifiers below

	/// Updates the days from the year ahead when this transport is not available
	/// Should be called monthly
	void updateUnavailDaysForTheYearAhead(const std::string &udya_);

	/// Changes the set of operational days of a week
	void updateOperationalDaysOfWeek(const std::string &odw_);

	/**
	Parses the timetable string and assigns the intervals to the empty _timetable.
	
	Example of string to parse:
		21:0-25:0|25:30-27:45|28:0-29:30|29:45-30:50|31:0-32:30
	*/
	void updateTimetable(const std::string &timetable_);
};
