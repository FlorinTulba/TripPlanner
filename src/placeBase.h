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

#ifndef H_PLACE_BASE
#define H_PLACE_BASE

#include "util.h"
#include "warnings.h"

#pragma warning ( push, 0 )

#include <sstream>
#include <cmath>

#pragma warning ( pop )

 // namespace trip planner - specifications
namespace tp { namespace specs {

  /// GPS coordinate with precision given by Floating: float/double
  template<class Floating, typename = std::enable_if_t<
    std::is_floating_point<Floating>::value>>
  class GpsCoord {
  protected:
    radians<Floating> latRad;   ///< GPS latitude
    radians<Floating> longRad;  ///< GPS longitude

  public:
    /// Validates and then stores the provided coordinate
    GpsCoord(const radians<Floating> &latitude,
             const radians<Floating> &longitude) :
        latRad(latitude), longRad(longitude) {
      static constexpr Floating 
        piPlusEps = Floating(1e-5L + Pi),
        halfPiPlusEps = Floating(1e-5L + Pi * .5L);

#pragma warning ( disable : RESERVED_LITERAL_SUFFIX )
      using std::string_literals::operator""s;
#pragma warning ( default : RESERVED_LITERAL_SUFFIX )

      if(fabs(latRad) > halfPiPlusEps)
        throw std::invalid_argument("Latitudes are values between [-pi/2, pi/2] "
                                    "radians. Received instead "s +
                                    std::to_string(latitude));
      if(fabs(longRad) > piPlusEps)
        throw std::invalid_argument("Longitudes are values between [-pi, pi] "
                                    "radians. Received instead "s +
                                    std::to_string(longitude));
    }

    /// @return copy of the latitude (the object is smaller than its reference)
    inline radians<Floating> latitude() const { return latRad; }

    /// @return copy of the longitude (the object is smaller than its reference)
    inline radians<Floating> longitude() const { return longRad; }

    /// @return true if rhs is more to the North than this.
    /// For same latitude, returns true if rhs is more to the East than this.
    inline bool operator < (const GpsCoord<Floating> &rhs) const {
      return latRad < rhs.latitude() ||
        ((latRad == rhs.latitude()) && (longRad < rhs.longitude()));
    }

    /// Distance between 2 GPS coordinates in kilometers or miles
    Floating distanceTo(const GpsCoord<Floating> &p, bool km = true) const {
      static constexpr Floating
        One = (Floating)1, Two = (Floating)2, InvTwo = (Floating).5L,

        // values from https://en.wikipedia.org/wiki/Earth_radius
        earthMeanRadiusKm =     (Floating)6'371.008'8L,
        earthMeanRadiusMiles =  (Floating)3'958.761'3L;

      // See the numerical-stable relation from:
      //https://www.movable-type.co.uk/scripts/latlong.html
      const Floating
        earthMeanDiameter = 
        (km ? earthMeanRadiusKm : earthMeanRadiusMiles) * Two,

        sinHalfLatDiff = sin((p.latitude() - latRad) * InvTwo),
        sinHalfLongDiff = sin((p.longitude() - longRad) * InvTwo),

        a = sinHalfLatDiff * sinHalfLatDiff +
            sinHalfLongDiff * sinHalfLongDiff *
              cos(latRad) * cos(p.latitude());

      return earthMeanDiameter * atan2(sqrt(a), sqrt(One - a));
    }

    /// @return a string representation of the coordinate in degrees / radians
    std::string toString(bool degrees = true) const {
      static constexpr Floating Zero = (Floating)0;
      const Floating lat = latRad.get(degrees), lon = longRad.get(degrees);
      std::ostringstream oss;
      oss<<'('
        <<fabs(lat)<<(lat >= Zero ? "N" : "S")<<" , "
        <<fabs(lon)<<(lon >= Zero ? "E" : "W")<<')';
      return oss.str();
    }
  };
#if (defined(_MSC_VER) || defined(__clang__)) && !defined(__GNUC__)
  // Let just 1 unit instantiate these templates
  extern template GpsCoord<float>;
  extern template GpsCoord<double>;
  extern template GpsCoord<long double>;
#endif // (_MSC_VER || __clang__) && !__GNUC__

  /// Basic information for a place. Uses single precision (float)
  struct IfPlace /*abstract*/ {
    virtual ~IfPlace() /*= 0*/ {}

	  virtual unsigned id() const = 0;

    /// @return the GPS coordinate
    virtual const GpsCoord<float>& gpsCoord() const = 0;

    /// @return all known names of the place, sorted by popularity (UTF-8 encoded)
    virtual const std::vector<std::string>& names() const = 0;

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
    virtual const std::string& shortDescr() const = 0;

    /// @return an arrangement of the details of a place (UTF-8 encoded)
    virtual std::string toString() const;
  };

}} // namespace tp::specs

#endif // H_PLACE_BASE
