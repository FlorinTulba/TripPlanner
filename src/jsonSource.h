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

#ifndef H_JSON_SOURCE
#define H_JSON_SOURCE

#include "infoSource.h"

#pragma warning ( push, 0 )

#include <boost/filesystem/path.hpp>

#pragma warning ( pop )

// namespace trip planner - specifications
namespace tp { namespace specs {

  /// Json provider of input data
  class JsonSource : public InfoSource {
  protected:
	  struct DataManager;	///< handle class
	  DataManager *dm = nullptr;	///< handle

    /// Path of the json file used for (re)loading.
    /// Associated to the constructor with `path` parameter
    boost::filesystem::path jsonFile;
    
    /// Pointer to a Json content string maintained externally
    /// and provided by the constructor with `string` parameter
    const std::string * const jsonContent = nullptr;

	  /**
	  Makes sure the memory for dm is released when destructing this
	  or if an exception gets caught during construction.
	  */
	  void cleanup();

    /// Allows using the updates from the source presented as jsonStream
    void reload(std::istringstream &&jsonStream);

  public:
    /**
    Parses the provided jsonContent_ and throws if any problems.
    It keeps the address of jsonContent_ to allow reload operations
    based on the new content assigned to the string.
    */
	  explicit JsonSource(const std::string &jsonContent_);

	  /**
    Parses the provided json file and throws if any problems.
    The updated version of the same file is used for reload operations.
    */
    explicit JsonSource(const boost::filesystem::path &jsonFile_);

    JsonSource(const JsonSource&) = delete;
    JsonSource(JsonSource&&) = delete;
    void operator=(const JsonSource&) = delete;
    void operator=(JsonSource&&) = delete;

	  ~JsonSource(); ///< calls cleanup()

    /// Allows using the updates from the source
    void reload() override;

	  /// Fills placeIds with the sorted set of id-s of all places from the map
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

#endif // H_JSON_SOURCE
