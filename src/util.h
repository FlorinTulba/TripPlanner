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

#ifndef H_UTIL
#define H_UTIL

#pragma warning ( push, 0 )

#include <string>
#include <vector>

#pragma warning ( pop )

/// Prevents warnings about unused parameters
extern const void* addrUnreferenced;
#define UNREFERENCED(p) addrUnreferenced = (const void*)&p;

// Specifying container ranges
#define CBOUNDS(cont) std::cbegin(cont), std::cend(cont)
#define CRBOUNDS(cont) std::crbegin(cont), std::crend(cont)
#define BOUNDS(cont) std::begin(cont), std::end(cont)
#define RBOUNDS(cont) std::rbegin(cont), std::rend(cont)

// Show a value together with the expression generating it
// Include <iostream> when using them
#define PRINT(expr) std::cout<<#expr " = "<<(expr)
#define PRINTLN(expr) PRINT(expr)<<std::endl

#if (defined(_MSC_VER) || defined(__clang__)) && !defined(__GNUC__)
// Let just 1 unit instantiate this template used below
extern template std::vector<std::string>;
#endif // (_MSC_VER || __clang__) && !__GNUC__

/**
Tokenizes a string based on the provided regex delimiter expression.
Default delimiter is one or more space-like characters.
It works with multi-line strings, as well.
*/
std::vector<std::string> tokenize(const std::string &s,
                                  const std::string &regexDelimStr = R"(\s+)");

// The trimmers from below use regex expressions containing
// ^ (line start) and $ (line end)
// and they won't work for multi-line strings!
std::string trim(const std::string &s);   ///< Trims a string
std::string ltrim(const std::string &s);  ///< Trims the start of a string
std::string rtrim(const std::string &s);  ///< Trims the end of a string

/// Miles to kilometers
template<class T>
constexpr T milesToKm(T miles) {
  return (T)1.609'344L * miles;
}

/// Kilometers to miles
template<class T>
constexpr T kmToMiles(T km) {
  return (T)0.621'371'19L * km;
}

// Value of PI
//template<class T> // Variable templates not supported yet
//constexpr T Pi = T(3.14159265358979323846L);
#define Pi 3.14159265358979323846L

/// Ensures radians awareness where needed
template<class T>
class radians {
protected:
  static constexpr T multiplierDegToRad = T(Pi/180.L);
  static constexpr T multiplierRadToDeg = T(180.L/Pi);

  T value; ///< value in radians

public:
  /// Stores the provided radians value
  constexpr radians(T valRad) : value(valRad) {}

  /// Creates a `radians` object from a values in degrees
  static inline constexpr radians<T> fromDegrees(T valDeg) {
    return radians<T>(valDeg * multiplierDegToRad);
  }

  /// @return value
  inline constexpr T get(bool degrees = false) const {
    return degrees ? (value * multiplierRadToDeg) : value;
  }

  /// Allows working directly with the radians value
  inline constexpr operator T() const { return value; }

  /// Allows working directly with the radians value converted to U
  template<class U>
  explicit inline constexpr operator U() const { return (U)value; }
};
#if (defined(_MSC_VER) || defined(__clang__)) && !defined(__GNUC__)
// Let just 1 unit instantiate these class templates
extern template radians<float>;
extern template radians<double>;
extern template radians<long double>;
#endif // (_MSC_VER || __clang__) && !__GNUC__

extern constexpr radians<long double> operator"" _deg(long double degrees);

#endif // H_UTIL
