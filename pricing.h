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

#pragma once

/**
Simulating the computation of the ticket prices.

Behavior:
- to ensure a profit, small distances are more expensive per km
than long travel distances
- if there is a business class, it's supposed to be
more expensive than the economy class
- airplane tickets use high fares for urgent bookings,
or when the airplane is almost fully booked
*/
class TicketPriceCalculator {
protected:
	float kEconomy;			///< k for economy class
	float kBusiness;		///< k for business class
	float lowFareFactor;	///< fare reduction factor (range [0,1])
	float highFareFactor;	///< fare increase factor (at least 1)

public:
	/// Throws invalid_argument for invalid k or factors
	TicketPriceCalculator(float kEconomy_, float kBusiness_ = 0.f,
						  float lowFareFactor_ = 1.f, float highFareFactor_ = 1.f);

	/**
	The basic fare, based only on distance and seat class.
	Throws invalid_argument for 0 or negative tripDistance.
	*/
	float normalFare(float tripDistance, bool economyClass = true);

	/**
	Fare considering also the urgency and occupancy.
	Throws invalid_argument for 0 or negative tripDistance,
	or when urgency / occupancy are outside [0,1] range.
	*/
	float airplaneFare(float tripDistance, float urgency, float occupancy,
					   bool economyClass = true);
};
