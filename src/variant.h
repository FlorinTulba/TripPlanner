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

#include "variantsBase.h"

/// Realization of IVariant
class Variant : public IVariant {
protected:
	std::vector<std::unique_ptr<IConnection>> conns;	///< the computed connections

public:
	/// Extends the connections chain
	void appendConnection(std::unique_ptr<IConnection> conn);

	/// @return the connections
	const std::vector<std::unique_ptr<IConnection>>& connections() const override;

	/// The start of the interval
	boost::posix_time::ptime begin() const override;

	/// The end of the interval
	boost::posix_time::ptime end() const override;

	/// Trip duration for the connected locations.
	/// Use formatDuration from util.h to display this value
	boost::posix_time::time_duration duration() const override;

	/// @return the index of the starting point (local / global)
	size_t from() const override;

	/// @return the index of the destination point (local / global)
	size_t to() const override;

	/// Price of the ticket(s) between the connected locations
	float price() const override;

	/// Distance between the connected locations in the given configuration
	float distance() const override;

	/// Utilized transportation modes. Use TranspModes::toString to display it.
	/// @return overlapping bitmasks for every specific mode
	size_t transpModes() const override;
};
