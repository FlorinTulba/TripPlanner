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

#include "placeBase.h"
#include "warnings.h"

/// Realization of IfUniquePlace
class UniquePlace : public virtual IfUniquePlace {
protected:
	std::string _name;

public:
	UniquePlace(const std::string &name_);

	/// Allows treating the interface IfUniquePlace like the actual realization class
	UniquePlace(const IfUniquePlace &other);

	const std::string& name() const override;
};

#pragma warning ( disable : INHERITANCE_VIA_DOMINANCE )
/// Realization of IfPlace
class Place : public UniquePlace, public IfPlace {
protected:
	unsigned _id;

public:
	Place(unsigned id_, const std::string &name_);

	unsigned id() const override;
};
#pragma warning ( default : INHERITANCE_VIA_DOMINANCE )
