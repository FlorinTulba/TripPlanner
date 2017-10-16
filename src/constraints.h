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

#include "constraintsBase.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

/// Realization of ITimeConstraints
class TimeConstraints : public ITimeConstraints {
protected:
	/// Imposed period for departure
	boost::posix_time::time_period _leavePeriod;

	/// Imposed period for arrival
	boost::posix_time::time_period _arrivePeriod;

public:
	/**
	Imposed intervals for leaving and arrival.
	The default intervals are each from now until a year from now.
	*/
	TimeConstraints(const boost::posix_time::time_period &leavePeriod_ =
						boost::posix_time::time_period(
							boost::posix_time::second_clock::universal_time(),
							boost::posix_time::hours(366 * 24)),
					const boost::posix_time::time_period &arrivePeriod_ =
						boost::posix_time::time_period(
							boost::posix_time::second_clock::universal_time(),
							boost::posix_time::hours(366 * 24)));

	/// Imposed period for departure
	const boost::posix_time::time_period& leavePeriod() const override;

	/// Imposed period for arrival
	const boost::posix_time::time_period& arrivePeriod() const override;
};
