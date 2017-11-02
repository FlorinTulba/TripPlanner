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

#ifndef H_ROUTE_CUSTOMIZABLE_INFO_BASE
#define H_ROUTE_CUSTOMIZABLE_INFO_BASE

#pragma warning ( push, 0 )

#include <set>
#include <bitset>
#include <memory>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/time_period.hpp>

#pragma clang diagnostic pop
#pragma warning ( pop )

// namespace trip planner - specifications
namespace tp { namespace specs {

   /// Provides specific information about an alternative for a given route
  struct IRouteCustomizableInfo /*abstract*/ {
	  virtual ~IRouteCustomizableInfo() /*= 0*/ {}

	  /// Operational days of a week
	  virtual std::shared_ptr<std::bitset<7>> operationalDaysOfWeek() const = 0;

	  /// The days from the year ahead when this transport is not available
	  virtual std::shared_ptr<std::set<boost::gregorian::date>>
		  unavailDaysForTheYearAhead() const = 0;
  };

}} // namespace tp::specs

#endif // H_ROUTE_CUSTOMIZABLE_INFO_BASE
