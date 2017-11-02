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

#ifndef H_CONNECTION
#define H_CONNECTION

#include "variantsBase.h"

#pragma warning ( push, 0 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"

#include <boost/date_time/posix_time/time_period.hpp>

#pragma clang diagnostic pop
#pragma warning ( pop )

// namespace trip planner - queries
namespace tp { namespace queries {

  /// Realization of IConnection
  class Connection : public IConnection {
  protected:
	  size_t _from;	///< index of the starting point (local / global)
	  size_t _to;		///< index of the destination point (local / global)
	  boost::posix_time::time_period _interval;	///< travel period
	  size_t _transpModes;	///< a transportation mode
	  float _price;	///< price of the ticket(s) between the connected locations
	  float _distance;///< distance between the connected locations in the given configuration

  public:
	  /// Builds the connection between from_ and to_, during interval_
	  /// for price_ $ corresponding to dist km.
	  /// transpMode is among the enum from TranspModes.
	  Connection(size_t from_, size_t to_,
               const boost::posix_time::time_period &interval_,
               int transpModes_, float price_, float dist);

	  /// The start of the interval (first permitted value)
	  boost::posix_time::ptime begin() const override;

	  /// The end of the interval (last permitted value)
	  boost::posix_time::ptime end() const override;

	  /// Trip duration for the connected locations.
	  /// Use formatDuration from util.h to display this value
	  boost::posix_time::time_duration duration() const override;

	  /// @return the index of the starting point (local / global)
	  size_t from() const override;

	  /// @return the index of the destination point (local / global)
	  size_t to() const override;

	  /// Price of the ticket(s) between the connected locations
	  float price() const override;

	  /// Distance between the connected locations in the given configuration
	  float distance() const override;

	  /// Utilized transportation modes. Use TranspModes::toString to display it.
	  /// @return overlapping bitmasks for every specific mode
	  size_t transpModes() const override;
  };

}} // namespace tp::queries

#endif // H_CONNECTION
