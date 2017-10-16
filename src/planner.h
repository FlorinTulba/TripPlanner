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
#include "resultsBase.h"

#include <memory>
#include <string>

/**
*/
class TripPlanner {
protected:
public:
	TripPlanner(const std::string &dataPath);

	/**
	Searches for itinerary variants between the 2 places.

	@param fromPlace starting location
	@param toPlace destination location
	@param maxCountPerCategory maximum number of variants
	for each considered category (price, distance, duration, soonest at destination)
	@param timeConstraints the imposed periods when to leave and when to arrive or nullptr if unconstrained

	@return the found variants for the trip if the places exist, are distinct and can be connected; nullptr otherwise
	*/
	std::unique_ptr<IResults> search(const std::string &fromPlace,
									 const std::string &toPlace,
									 size_t maxCountPerCategory,
									 const ITimeConstraints *timeConstraints = nullptr) const;
};
