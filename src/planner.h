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

#ifndef H_PLANNER
#define H_PLANNER

#include "constraintsBase.h"
#include "resultsBase.h"
#include "infoSource.h"

#pragma warning ( push, 0 )

#include <memory>
#include <string>
#include <shared_mutex>

#pragma warning ( pop )

namespace tp { // trip planner

  /**
  Maintains the graph of places, routes and schedules and
  allows planning trips between pairs of locations.
  */
  class TripPlanner {
  protected:
	  /// The provider of places, routes and schedules
	  const std::unique_ptr<specs::InfoSource> infoSrc;

	  /// Handle class for building the map`s graph and resolving queries
	  class GraphMap;
	  GraphMap *g = nullptr; ///< the actual planner

    /**
    Allows controlling the access to data.
    When performing database updates / patching, the access is denied,
    otherwise the information is shared.
    */
    mutable std::shared_timed_mutex dataAccess;

    /// Rebuilds g from an updated infoSrc
    void reset();

    /// @return id of a place with a given name.
    /// Solves ambiguities by prompting the user using promptStream and outStream
    unsigned pickPlace(const std::string &name,
                       std::istream &promptStream = std::cin,
                       std::ostream &outStream = std::cout) const;

  public:
	  /**
	  Reads the provided `map` and builds the required graph.
	
	  @param infoSrc_ either a JsonSource or DbSource object
	  */
	  TripPlanner(std::unique_ptr<specs::InfoSource> infoSrc_);

    TripPlanner(const TripPlanner&) = delete;
    TripPlanner(TripPlanner&&) = delete;
    void operator=(const TripPlanner&) = delete;
    void operator=(TripPlanner&&) = delete;

    ~TripPlanner(); ///< ensure the release of g and the unlocking of dataAccess

    /**
    Manipulates or relies on dataAccess to control or get data access.
    When allowed is true, it updates g before unlocking dataAccess.
    When allowed is false, it locks dataAccess, thus
    waiting until all ongoing operations have finished.

    After allowDataAccess(false) returns, the admin can perform
    database maintenance:
    - updating the map, the routes, ...
    - looking for booked tickets issues - purchased tickets
      where some of the travel conditions have changed,
      to be able to notify the customers, at least
    */
    void allowDataAccess(bool allowed = true);

	  /**
	  Searches for itinerary variants between the 2 places.

	  @param fromPlace starting location
	  @param toPlace destination location
	  @param maxCountPerCategory maximum number of variants
	    for each considered category (price, distance, duration, soonest at destination)
	  @param timeConstraints the imposed periods when to leave and when to arrive
      or nullptr if unconstrained

	  @return the found variants for the trip
    
    @throw invalid_argument when:
    - the specified locations don`t exist, or if they are not distinct
    - maxCountPerCategory is 0
    @throw runtime_error when dataAccess is not shared-lockable for 50ms
    */
	  std::unique_ptr<queries::IResults>
      search(const std::string &fromPlace, const std::string &toPlace,
             size_t maxCountPerCategory,
             const queries::ITimeConstraints *timeConstraints = nullptr) const;
  };

} // namespace tp

#endif // H_PLANNER
