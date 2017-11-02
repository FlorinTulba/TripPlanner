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

#ifndef H_RESULTS
#define H_RESULTS

#include "resultsBase.h"

#include <vector>

// namespace trip planner - queries
namespace tp { namespace queries {

  /// Realization of IResults
  class Results : public IResults {
  protected:
	  std::vector<std::unique_ptr<IVariants>> results;

	  IVariants& get(size_t categ) const;

  public:
	  /// Initializes empty results
	  Results();

	  /// @return editable variants for the given category
	  IVariants& operator[](size_t categ);

	  /// @return the variants for the given category
	  const IVariants& operator[](size_t categ) const override;
  };

}} // namespace tp::queries

#endif // H_RESULTS
