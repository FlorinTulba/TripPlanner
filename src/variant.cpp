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

#include "variant.h"
#include "util.h"

#include <cassert>
#include <numeric>

using namespace std;
using namespace boost::posix_time;

void Variant::appendConnection(std::unique_ptr<IConnection> conn) {
	if(nullptr == conn)
		throw invalid_argument(__FUNCTION__ " expect non-null conn parameter!");

	if(!conns.empty()) {
		const IConnection *prev = conns.back().get();
		assert(prev != nullptr);
		if(conn->from() != prev->to() || conn->begin() <= prev->end())
			throw invalid_argument(__FUNCTION__
								   " needs a conn that binds to the previous last location"
								   " and only after the arrival at that last location!");
	}

	conns.push_back(move(conn));
}

const vector<unique_ptr<IConnection>>& Variant::connections() const {
	return conns;
}

ptime Variant::begin() const {
	assert(!conns.empty() && conns.back());
	return conns.front()->begin();
}

ptime Variant::end() const {
	assert(!conns.empty() && conns.back());
	return conns.back()->end();
}

time_duration Variant::duration() const {
	return end() - begin();
}

size_t Variant::from() const {
	assert(!conns.empty() && conns.front());
	return conns.front()->from();
}

size_t Variant::to() const {
	assert(!conns.empty() && conns.back());
	return conns.back()->to();
}

float Variant::price() const {
	return accumulate(CBOUNDS(conns), 0.f,
					  [] (float prevSum,
						  const unique_ptr<IConnection> &c) {
		assert(c);
		return prevSum + c->price();
	});
}

float Variant::distance() const {
	return accumulate(CBOUNDS(conns), 0.f,
					  [] (float prevSum,
						  const unique_ptr<IConnection> &c) {
		assert(c);
		return prevSum + c->distance();
	});
}

size_t Variant::transpModes() const {
	size_t result = 0ULL;
	for(const unique_ptr<IConnection> &c : conns) {
		assert(c);
		result = result | c->transpModes();
	}
	return result;
}

ostream& operator<<(ostream &os, const IVariant &variant) {
	const vector<unique_ptr<IConnection>> &conns = variant.connections();
	const size_t count = conns.size();
	os<<"Summary: "
		<<variant.from()<<" ["<<formatTimePoint(variant.begin())<<" - "
		<<TranspModes::toString(variant.transpModes())<<"; "
		<<roundf(10.f * variant.distance())/10.f<<"km; "
		<<formatDuration(variant.duration())<<"; "
		<<roundf(100.f * variant.price())/100.f<<"$ - "
		<<formatTimePoint(variant.end())<<"] "<<variant.to()
		<<endl;
	os<<"Details:"<<endl;
	os<<variant.from();
	for(size_t i = 0ULL; i < count; ++i) {
		const unique_ptr<IConnection> &c = conns[i];
		assert(c);
		os<<" ["<<formatTimePoint(c->begin())<<" - "
			<<TranspModes::toString(c->transpModes())<<"; "
			<<roundf(10.f * c->distance())/10.f<<"km; "
			<<formatDuration(c->duration())<<"; "
			<<roundf(100.f * c->price())/100.f<<"$ - "
			<<formatTimePoint(c->end())<<"] "<<c->to();
	}
	os<<endl;
	return os;
}
