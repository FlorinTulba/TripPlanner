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

#ifndef H_ROUTE_SHARED_INFO
#define H_ROUTE_SHARED_INFO

#include "routeSharedInfoBase.h"

 // namespace trip planner - specifications
namespace tp { namespace specs {

  class RouteSharedInfo : public IRouteSharedInfo {
  protected:
	  /// Realization of IRouteCustomizableInfo
	  class RouteCustomizableInfo : public IRouteCustomizableInfo {
	  protected:
		  /// Operational days of a week
		  std::shared_ptr<std::bitset<7>> odw;

		  /// The days from the year ahead when this transport is not available
		  std::shared_ptr<std::set<boost::gregorian::date>> udya;

	  public:
		  RouteCustomizableInfo(const std::string &odw_, const std::string &udya_);

      RouteCustomizableInfo(const RouteCustomizableInfo&) = default;
      RouteCustomizableInfo(RouteCustomizableInfo&&) = default;
      RouteCustomizableInfo& operator=(const RouteCustomizableInfo&) = default;
      RouteCustomizableInfo& operator=(RouteCustomizableInfo&&) = default;

		  /// Operational days of a week
		  std::shared_ptr<std::bitset<7>> operationalDaysOfWeek() const override;

		  /// The days from the year ahead when this transport is not available
		  std::shared_ptr<std::set<boost::gregorian::date>>
			  unavailDaysForTheYearAhead() const override;
	  };

	  /// RouteAlternatives can use its data as is,
	  /// or they can create individual settings for any part of it
	  RouteCustomizableInfo routeCustomizableInfo;

	  size_t _transpMode = 0ULL;	///< air/road/rail/water. @see TranspModes
	
	  /// id-s of the places from this route where there are stops
	  std::vector<unsigned> stops;
	  size_t _stopsCount = 0ULL;	///< count of stops

	  /// better search performance when checking if a place is a stop for this route
	  std::set<unsigned> stopsSet;

	  std::vector<float> _distances;	///< the distances between consecutive stops

	  /// Provides a calculator for ticket prices
	  std::unique_ptr<ITicketPriceCalculator> pricingEng;

	  /// The id-s of the alternatives of this route (different schedule, capacity..., but same route)
	  std::set<unsigned> _alternatives;

	  unsigned _id = 0UL;	///< unique id

	  /// @throw out_of_range for an invalid stopIdx
	  void validateStopIdx(size_t stopIdx) const;

  public:
	  /**
	  Configures all data of a route shared
	  by several alternatives of the route.

	  The alternatives are added when they get created.
	  */
	  RouteSharedInfo(unsigned id_, size_t transpMode_,
                    std::unique_ptr<ITicketPriceCalculator> pricingEng_,
                    const std::string &udya_ = "",
                    const std::string &odw_ = "1111111");

    RouteSharedInfo(const RouteSharedInfo&) = default;
    RouteSharedInfo(RouteSharedInfo&&) = default;
    RouteSharedInfo& operator=(const RouteSharedInfo&) = default;
    RouteSharedInfo& operator=(RouteSharedInfo&&) = default;

	  // Getter methods

	  unsigned id() const override;	///< unique id

	  size_t transpMode() const override;	///< air/road/rail/water. @see TranspModes

	  size_t stopsCount() const override;	///< number of stops on the route

	  /// Checks if the transport stops at a certain place
	  bool containsStop(unsigned placeId) const override;

	  /// @return the id of the place where the route has its stopIdx-th stop
	  /// when traversing the routes in normal or reversed order (for returnTrip = true)
	  unsigned nthStop(size_t stopIdx, bool returnTrip) const override;
	
	  /// The stops on the route
	  const std::vector<unsigned>& traversedStops() const override;
	
	  /// The distances between consecutive stops
	  const std::vector<float>& distances() const override;
	
	  /// @return the value of the distIdx-th distance to traverse
	  /// in normal or reversed order (for returnTrip = true)
	  float nthDistance(size_t distIdx, bool returnTrip) const override;

	  /// Provides a calculator for ticket prices
	  ITicketPriceCalculator& pricingEngine() const override;

	  /// The id-s of the alternatives of this route (different schedule, capacity..., but same route)
	  const std::set<unsigned>& alternatives() const override;

	  /// Offers access to shared route information that can be customized
	  const IRouteCustomizableInfo& customizableInfo() const override;

	  // Modifiers below

	  /// Marks the starting place when traversing the route directly
	  void setFirstPlace(unsigned placeId);

	  /// Marks the next stopover and the distance to it
	  void setNextStop(unsigned placeId, float distToPrevStop);

	  /// Adds an alternatives of this route (different schedule, capacity..., but same route)
	  void addAlternative(unsigned alternativeId);
  };

}} // namespace tp::specs

#endif // H_ROUTE_SHARED_INFO
