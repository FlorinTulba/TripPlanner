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

#ifndef H_VARIANTS_BASE
#define H_VARIANTS_BASE

#pragma warning ( push, 0 )

#include <memory>
#include <vector>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"

#include <boost/date_time/posix_time/ptime.hpp>

#pragma clang diagnostic pop
#pragma warning ( pop )

 // namespace trip planner - queries
namespace tp { namespace queries {

  /// @return the names of the considered categories
  extern const std::vector<const char*>& variantCategories();

  /// Connects 2 locations (directly or not)
  struct IConnection /*abstract*/ {
    virtual ~IConnection() /*= 0*/ {}

	  /// @return the departure time for this connection (local / global)
	  virtual boost::posix_time::ptime begin() const = 0;

	  /// @return the arrival time for this connection (local / global)
	  virtual boost::posix_time::ptime end() const = 0;

	  /// @return the travel duration for this connection (local / global)
	  virtual boost::posix_time::time_duration duration() const = 0;

	  /// @return the index of the starting point (local / global)
	  virtual size_t from() const = 0;

	  /// @return the index of the destination point (local / global)
	  virtual size_t to() const = 0;

	  /// Price of the ticket(s) between the connected locations
	  virtual float price() const = 0;

	  /// Distance between the connected locations in the given configuration
	  virtual float distance() const = 0;

	  /// Utilized transportation modes. Use TranspModes::toString to display it.
	  /// @return overlapping bitmasks for every specific mode
	  virtual size_t transpModes() const = 0;
  };

  /// Trip variant, typically a composite containing successive connections
  struct IVariant /*abstract*/ : IConnection {
    virtual ~IVariant() /*= 0*/ {}

    /// @return the connections 
	  virtual const std::vector<std::unique_ptr<IConnection>>& connections() const = 0;
  };

  /// The variants within a certain category
  struct IVariants /*abstract*/ {
	  virtual ~IVariants() /*= 0*/ {}

	  /// @return the stored variants
	  virtual const std::vector<std::unique_ptr<IVariant>>& get() const = 0;
  };

}} // namespace tp::queries

std::ostream& operator<<(std::ostream &os, const tp::queries::IVariant &variant);
std::ostream& operator<<(std::ostream &os, const tp::queries::IVariants &variants);

#endif // H_VARIANTS_BASE
