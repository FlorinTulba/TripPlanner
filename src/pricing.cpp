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

#include "pricing.h"

#pragma warning ( push, 0 )

#include <cmath>
#include <algorithm>
#include <stdexcept>

#pragma warning ( pop )

using namespace std;

// namespace trip planner - specifications
namespace tp { namespace specs {

  TicketPriceCalculator::TicketPriceCalculator(float kEconomy_,
                                               float kBusiness_/* = 0.f*/,
                                               float lowFareFactor_/* = 1.f*/,
                                               float highFareFactor_/* = 1.f*/) :
		  kEconomy(kEconomy_), kBusiness(kBusiness_),
		  lowFareFactor(lowFareFactor_), highFareFactor(highFareFactor_) {
	  if(kEconomy <= 0.f || kBusiness < 0.f ||
       lowFareFactor <= 0.f || lowFareFactor > 1.f || highFareFactor < 1.f)
		  throw invalid_argument(string(__func__) +
                             " doesn't accept negative parameters, "
                             "nor a lowFareFactor outside [0,1] and neither a "
                             "highFareFactor < 1!");

	  if(kBusiness != 0.f && kBusiness < kEconomy)
		  throw invalid_argument(string(__func__) +
                             " expects kBusiness to be larger than kEconomy");
  }

  /// @return ln(1 + k * tripDistance)
  float TicketPriceCalculator::normalFare(float tripDistance,
                                          bool economyClass/* = true*/) {
	  if(tripDistance <= 0.f)
		  throw invalid_argument(string(__func__) + " expects a strictly positive "
                             "tripDistance parameter!");

	  const float k = economyClass ? kEconomy : kBusiness;
	  return log1pf(k * tripDistance);
  }

  /// @return ln(1 + k * tripDistance) *
  /// (lowFareFactor + (highFareFactor - lowFareFactor) *
  /// (e^max(urgency, occupancy) - 1) / (e - 1))
  float TicketPriceCalculator::airplaneFare(float tripDistance,
                                            float urgency, float occupancy,
                                            bool economyClass/* = true*/) {
	  const float basePrice = normalFare(tripDistance, economyClass);

	  if(urgency < 0.f || urgency > 1.f || occupancy < 0.f || occupancy > 1.f)
		  throw invalid_argument(string(__func__) + " expects urgency and occupancy "
                             "parameters within [0,1] range!");

	  return basePrice * (lowFareFactor +
						  (highFareFactor - lowFareFactor) *
						  expm1f(max(urgency, occupancy)) / expm1f(1.f));
  }

}} // namespace tp::specs
