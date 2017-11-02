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

#ifndef H_TRANSP_MODES
#define H_TRANSP_MODES

#pragma warning ( push, 0 )

#include <cstddef>

#pragma warning ( pop )

 // namespace trip planner - specifications
namespace tp { namespace specs {

    /// Transportation modes
  struct TranspModes {
	  enum {
		  AIR = 1<<0,
		  RAIL = 1<<1,
		  ROAD = 1<<2,
		  WATER = 1<<3,

		  last = WATER // update this to be the last introduce transport mode
	  };

	  /// @return the description of the utilized transportation modes
	  static const char* toString(std::size_t masksCombination);

	  /// @return the masksCombination corresponding to the transportation modes mentioned in description
	  static std::size_t fromString(const char* description);
  };

}} // namespace tp::specs

#endif // H_TRANSP_MODES
