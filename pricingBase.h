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
Simulating the computation of ticket prices.

Behavior:
- to ensure a profit, small distances are more expensive per km
than long travel distances
- if there is a business class, it's supposed to be
more expensive than the economy class
- airplane tickets use high fares for urgent bookings,
or when the airplane is almost fully booked
*/
struct ITicketPriceCalculator abstract {
	virtual ~ITicketPriceCalculator() = 0 {}

	/**
	The basic fare, based only on distance and seat class.
	Throws invalid_argument for 0 or negative tripDistance.

	@param tripDistance distance covered by the ticket
	@param economyClass true for economy class; false for business class
	*/
	virtual float normalFare(float tripDistance, bool economyClass = true) = 0;

	/**
	Fare considering also the urgency and occupancy.
	Throws invalid_argument for 0 or negative tripDistance,
	or when urgency / occupancy are outside [0,1] range.

	@param tripDistance distance covered by the ticket
	@param urgency how soon is the booked flight (1=today, 0=one year from now)
	@param occupancy percentage of occupied seats after the current booking
	of a number of seats within the desired class - business / economy
	@param economyClass true for economy class; false for business class
	*/
	virtual float airplaneFare(float tripDistance, float urgency, float occupancy,
							   bool economyClass = true) = 0;
};
