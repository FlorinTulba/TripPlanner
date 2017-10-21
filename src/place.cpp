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

#include "place.h"

using namespace std;

UniquePlace::UniquePlace(const string &name_) : _name(name_) {}

UniquePlace::UniquePlace(const IfUniquePlace &other) :
	UniquePlace(other.name()) {}

const string& UniquePlace::name() const {
	return _name;
}

Place::Place(unsigned id_, const string &name_) :
	UniquePlace(name_), _id(id_) {}

unsigned Place::id() const {
	return _id;
}
