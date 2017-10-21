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

#include <string>

/**
Unique data about each mentioned place:
- for now the name (this interface does not use id-s)

More realistic scenarios would use either GPS location or several from the
following aspects:
- aliases, continent, country, county, locale, timezone, zip, currency, etc.

Realizations of this interface are enough to query for a specific place,
to determine its id and the rest of its non-unique information.
*/
struct IfUniquePlace abstract {
	virtual ~IfUniquePlace() = 0 {}

	virtual const std::string& name() const = 0;

	/// Since these are unique values, an ordering can be implemented
	virtual bool operator<(const IfUniquePlace &rhs) const;

	/// @return an arrangement of the details of a place
	virtual std::string toString() const;
};

/**
Adds the unique place id to the place.
Later, some other less unique aspects can be added.
*/
struct IfPlace abstract : virtual IfUniquePlace {
	virtual unsigned id() const = 0;

	/// @return an arrangement of the details of a place
	std::string toString() const override;
};
