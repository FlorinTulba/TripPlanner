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

#ifndef H_INFO_SOURCE
#define H_INFO_SOURCE

#include "placeBase.h"
#include "routeSharedInfoBase.h"
#include "routeAlternativeBase.h"

#pragma warning ( push, 0 )

#include <set>

#pragma warning ( pop )

// namespace trip planner - specifications
namespace tp { namespace specs {

  /// Retrieves necessary data from any provider realizing this interface
  struct InfoSource /*abstract*/ {
	  virtual ~InfoSource() /*= 0*/ {}

    /// Allows using the updates from the source
    virtual void reload() = 0;

	  /// Fills placeIds with the sorted set of id-s of all places from the map
	  virtual void idsOfAllPlaces(std::vector<unsigned> &placeIds) const = 0;

	  /**
	  Looks for the place with the given id.
	  Some places might have been removed, so there could be
	  missing values from the sequence of id-s.

	  @return the place data for the given id
	  @throw invalid_argument if id is not found
	  */
	  virtual const IfPlace& getPlace(unsigned id) const = 0;

    /**
    @return the place data for the given location
    @throw invalid_argument if there is no such place
    */
    virtual const IfPlace& getPlace(const GpsCoord<float> &gps) const = 0;

    /**
    @return the place with given name/alias and short description
    @throw invalid_argument if there is no such place
    */
    virtual const IfPlace& getPlace(const std::string &knownAs,
                                    const std::string &shortDescr = u8"") const = 0;

    /// Fills places with the pointers to all locations named name
    /// sorted by the corresponding description of each place
    virtual void getAllPlacesNamed(const std::string &name,
                                   std::vector<const IfPlace*> &places) const = 0;

    /// Fills routeSharedInfoIds with the sorted set of id-s of all routes from the map
    virtual void idsOfAllRoutes(std::vector<unsigned> &routeSharedInfoIds) const = 0;

    /// Finds the routes covering the location with placeId
	  virtual void routesForPlace(unsigned placeId,
                                std::vector<unsigned> &routeSharedInfoIds) const = 0;

	  /// Finds the routes covering the locations with placeIds
	  virtual void routesForPlaces(const std::set<unsigned> &placeIds,
                                 std::vector<unsigned> &routeSharedInfoIds) const = 0;

	  /// @return the route shared information with the given id
	  virtual IRouteSharedInfo& routeSharedInfo(unsigned routeSharedInfoId) const = 0;

	  /// @return the route alternative with the given id
	  virtual IRouteAlternative& routeAlternative(unsigned raId) const = 0;
  };

}} // namespace tp::specs

#endif // H_INFO_SOURCE
