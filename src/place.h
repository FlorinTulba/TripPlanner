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

#ifndef H_PLACE
#define H_PLACE

#include "placeBase.h"
#include "warnings.h"

// namespace trip planner - specifications
namespace tp { namespace specs {

  /// Realization of IfPlace
  class Place : public IfPlace {
  protected:
    GpsCoord<float> coord; ///< GPS location with single precision

    /// All known names of the place, sorted by popularity (UTF-8 encoded)
    std::vector<std::string> _names;

    /// Short description of the place, encoded in UTF-8
    std::string _shortDescr;
    
    unsigned _id;

  public:
    /**
    Fills Place information.
    Parameters names_ and shortDescr_ are UTF-8 encoded.
    names_ uses `|` as separator between consecutive aliases of the place.

    @throw invalid_argument when names_ is empty or contains duplicates
    */
	  Place(unsigned id_, const GpsCoord<float> &location,
          const std::string &names_, const std::string &shortDescr_ = u8"");

	  unsigned id() const override;

    /// @return the GPS coordinate
    const GpsCoord<float>& gpsCoord() const override;

    /// @return all known names of the place, sorted by popularity (UTF-8 encoded)
    const std::vector<std::string>& names() const override;

    /**
    Short description of the place, encoded in UTF-8.
    For any place, the pair (names[i], shortDescr()) is unique, no matter i!

    So, it helps locating the place known by names().
    Possible convention for its values:
    - could start with a country name abbreviation
    - optionally followed by county / region within the given country
    or a code for that region

    Example:
    names() = {u8"Venice", u8"Venezia"}
    and shortDescr() = u8"Ita" - the one from Italy
    or shortDescr() = u8"US, FL" - for the one from Florida, USA
    */
    const std::string& shortDescr() const override;
  };

}} // namespace tp::specs

#endif // H_PLACE_BASE
