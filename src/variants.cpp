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

#include "variants.h"

#pragma warning ( push, 0 )

#include <stdexcept>
#include <cassert>

#pragma warning ( pop )

using namespace std;

// namespace trip planner - queries
namespace tp { namespace queries {

  void Variants::add(std::unique_ptr<IVariant> variant) {
	  if(nullptr == variant)
		  throw invalid_argument(string(__func__) +
                             " expect non-null variant parameter!");

	  variants.push_back(move(variant));
  }

  const vector<unique_ptr<IVariant>>& Variants::get() const {
	  return variants;
  }

}} // namespace tp::queries

ostream& operator<<(ostream &os, const tp::queries::IVariants &variants) {
	const vector<unique_ptr<tp::queries::IVariant>> &v = variants.get();
	const size_t count = v.size();
	for(size_t i = 0ULL; i < count; ++i) {
		assert(v[i]);
		os<<"Variant "<<i<<endl<<*v[i]<<endl;
	}
	return os;
}
