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

#include "placeBase.h"
#include "routesBundleBase.h"
#include "routeAlternativeBase.h"

#include <set>

/// Retrieves necessary data from any provider realizing this interface
struct InfoSource abstract {
	virtual ~InfoSource() = 0 {}

	/**
	Looks for the place with the given id.
	Some places might have been removed, so there could be
	missing values from the sequence of id-s.

	@return the place data for the given id
	@throw invalid_argument if id is not found
	*/
	virtual IfPlace& getPlace(unsigned id) const = 0;

	/**
	@return the place data for the given uniquePlaceTraits
	@throw invalid_argument if there is no such place
	*/
	virtual IfPlace& getPlace(const IfUniquePlace &uniquePlaceTraits) const = 0;

	/// Finds the route bundles covering the location with placeId
	virtual void bundlesForPlace(unsigned placeId, std::set<unsigned> &bundleIds) const = 0;

	/// Finds the route bundles covering the locations with placeIds
	virtual void bundlesForPlaces(const std::set<unsigned> &placeIds,
								  std::set<unsigned> &bundleIds) const = 0;

	/// @return the bundle with the given id
	virtual IRoutesBundle& getRoutesBundle(unsigned bundleId) const = 0;

	/// @return the route alternative with the given id
	virtual IRouteAlternative& getRouteAlternative(unsigned raId) const = 0;
};
