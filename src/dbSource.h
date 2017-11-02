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

#ifndef H_DB_SOURCE
#define H_DB_SOURCE

#include "infoSource.h"
#include "credentialsBase.h"

// namespace trip planner - specifications
namespace tp { namespace specs {

  /// Database provider of input data
  class DbSource : public InfoSource {
  public:
	  /// Ensures the database can be accessed; Throws otherwise
	  DbSource(const std::string &dbUrl, const std::string &dbName,
             const IfCredentials &credentialsProvider);

    /// Allows using the updates from the source
    void reload() override;

    /// Fills placeIds with the set of id-s of all places from the map
	  void idsOfAllPlaces(std::vector<unsigned> &placeIds) const override;

	  /**
	  Looks for the place with the given id.
	  Some places might have been removed, so there could be
	  missing values from the sequence of id-s.

	  @return the place data for the given id
	  @throw invalid_argument if id is not found
	  */
	  const IfPlace& getPlace(unsigned id) const override;

    /**
    @return the place data for the given location
    @throw invalid_argument if there is no such place
    */
    const IfPlace& getPlace(const GpsCoord<float> &gps) const override;

    /**
    @return the place with given name/alias and short description
    @throw invalid_argument if there is no such place
    */
    const IfPlace& getPlace(const std::string &knownAs,
                            const std::string &shortDescr = u8"") const override;

    /// Fills places with the pointers to all locations named name
    /// sorted by the corresponding description of each place
    void getAllPlacesNamed(const std::string &name,
                           std::vector<const IfPlace*> &places) const override;


    /// Fills routeSharedInfoIds with the sorted set of id-s of all routes from the map
    void idsOfAllRoutes(std::vector<unsigned> &routeSharedInfoIds) const override;
    
    /// Finds the routes covering the location with placeId
	  void routesForPlace(unsigned placeId,
                        std::vector<unsigned> &routeSharedInfoIds) const override;

	  /// Finds the routes covering the locations with placeIds
	  void routesForPlaces(const std::set<unsigned> &placeIds,
                         std::vector<unsigned> &routeSharedInfoIds) const override;

	  /// @return the route shared information with the given id
	  IRouteSharedInfo& routeSharedInfo(unsigned routeSharedInfoId) const override;

	  /// @return the route alternative with the given id
	  IRouteAlternative& routeAlternative(unsigned raId) const override;
  };

}} // namespace tp::specs

#endif // H_DB_SOURCE
