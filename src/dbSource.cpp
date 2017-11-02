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

#include "dbSource.h"

using namespace std;

// namespace trip planner - specifications
namespace tp { namespace specs {

  DbSource::DbSource(const string &dbUrl, const string &dbName,
                     const IfCredentials &credentialsProvider) {}

  void DbSource::reload() {
    throw exception();
  }

  void DbSource::idsOfAllPlaces(vector<unsigned> &placeIds) const {
	  throw exception();
  }

  const IfPlace& DbSource::getPlace(unsigned id) const {
	  throw exception();
  }

  const IfPlace & DbSource::getPlace(const GpsCoord<float>& gps) const {
    throw exception();
  }

  const IfPlace & DbSource::getPlace(const string &knownAs,
                                     const string &shortDescr/* = u8""*/) const {
    throw exception();
  }

  void DbSource::getAllPlacesNamed(const string &name,
                                   vector<const IfPlace*> &places) const {
    throw exception();
  }

  void DbSource::idsOfAllRoutes(vector<unsigned>& routeSharedInfoIds) const {
    throw exception();
  }

  void DbSource::routesForPlace(unsigned placeId,
                                vector<unsigned> &routeSharedInfoIds) const {
	  throw exception();
  }

  void DbSource::routesForPlaces(const set<unsigned> &placeIds,
                                 vector<unsigned> &routeSharedInfoIds) const {
	  throw exception();
  }

  IRouteSharedInfo& DbSource::routeSharedInfo(unsigned routeSharedInfoId) const {
	  throw exception();
  }

  IRouteAlternative& DbSource::routeAlternative(unsigned raId) const {
	  throw exception();
  }

}} // namespace tp::specs
