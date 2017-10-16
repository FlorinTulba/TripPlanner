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

#include "connection.h"

#include <cassert>
#include <stdexcept>
#include <cmath>

using namespace std;
using namespace boost::posix_time;

Connection::Connection(size_t from_, size_t to_,
					   const time_period &interval_,
					   int transpModes_, float price_, float dist) :
			_from(from_), _to(to_),
			_interval(interval_.begin(), interval_.end() + time_duration::unit()),
			_transpModes(size_t(transpModes_)),
			_price(price_), _distance(dist) {
	if(_from == _to || _interval.is_null() ||
	   price_ <= 0.f || dist <= 0.f ||
	   (transpModes_ != TranspModes::AIR && transpModes_ != TranspModes::RAIL &&
		transpModes_ != TranspModes::ROAD && transpModes_ != TranspModes::WATER))
		throw invalid_argument(__FUNCTION__
							   " expects strictly positive price_ and dist,"
							   " transpModes_ among the enum from TranspModes,"
							   " non-empty interval_ and distinct from_ and to_!");
}

ptime Connection::begin() const {
	return _interval.begin();
}

ptime Connection::end() const {
	return _interval.last();
}

size_t Connection::from() const {
	return _from;
}

size_t Connection::to() const {
	return _to;
}

time_duration Connection::duration() const {
	return _interval.length();
}

float Connection::price() const {
	return _price;
}

float Connection::distance() const {
	return _distance;
}

size_t Connection::transpModes() const {
	return _transpModes;
}
