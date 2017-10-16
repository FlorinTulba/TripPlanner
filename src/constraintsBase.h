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

#include <boost/date_time/posix_time/time_period.hpp>

/// Provides the leave and arrive imposed intervals or maximum range if unconstrained
struct ITimeConstraints abstract {
	virtual ~ITimeConstraints() = 0 {}

	/// Imposed period for departure
	virtual const boost::posix_time::time_period& leavePeriod() const = 0;

	/// Imposed period for arrival
	virtual const boost::posix_time::time_period& arrivePeriod() const = 0;
};
