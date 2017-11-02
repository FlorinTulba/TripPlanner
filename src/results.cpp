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

#include "results.h"
#include "variants.h"

#pragma warning ( push, 0 )

#include <cassert>

#pragma warning ( pop )

using namespace std;

namespace tp { namespace queries {

  const vector<const char*>& variantCategories() {
	  static const vector<const char*> categs {
		  "most rapid variants",
		  "cheapest variants",
		  "shortest routes",
		  "variants getting to the destination the soonest",
		  "variants with shortest stationary time"
	  };
	  return categs;
  }

  Results::Results() {
	  const size_t categories = variantCategories().size();
	  results.reserve(categories);
	  for(size_t i = 0ULL; i < categories; ++i)
		  results.emplace_back(new Variants);
  }

  IVariants& Results::get(size_t categ) const {
	  if(categ >= variantCategories().size())
		  throw out_of_range(string(__func__) + " invalid categ!");

	  assert(results[categ]);
	  return *results[categ];
  }

  const IVariants& Results::operator[](size_t categ) const {
	  return get(categ);
  }

  IVariants& Results::operator[](size_t categ) {
	  return get(categ);
  }

}} // namespace tp::queries

ostream& operator<<(ostream &os, const tp::queries::IResults &results) {
  const size_t categories = tp::queries::variantCategories().size();
  for(size_t i = 0ULL; i < categories; ++i) {
    os<<"Viewing the "<<tp::queries::variantCategories()[i]<<':'<<endl<<endl;
    os<<results[i];
    os<<endl<<string(80ULL, '=')<<endl<<endl<<endl;
  }

  return os;
}
