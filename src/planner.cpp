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

#include "planner.h"
#include "constraints.h"
#include "results.h"
#include "place.h"

using namespace std;

namespace tp { // trip planner
  using namespace specs;
  using namespace queries;

  static const TimeConstraints defaultConstraints;

  /// Handle class for building the map`s graph and resolving queries
  class TripPlanner::GraphMap {
  protected:
    /// The provider of places, routes and schedules
	  InfoSource &infoSrc;

  public:
    /// Builds the map`s graph
	  GraphMap(InfoSource &infoSrc_) : infoSrc(infoSrc_) {
		  vector<unsigned> placeIds, routeSharedInfoIds;
		  infoSrc.idsOfAllPlaces(placeIds);
		  infoSrc.idsOfAllRoutes(routeSharedInfoIds);

		  for(unsigned placeId : placeIds) {
			  // TODO: add vertex placeId to the graph
		  }

		  for(unsigned rsiId : routeSharedInfoIds) {
			  IRouteSharedInfo &rsi = infoSrc.routeSharedInfo(rsiId);
			  const vector<unsigned> &stops = rsi.traversedStops();
			  const size_t stopsCountM1 = stops.size() - 1ULL;
			  for(unsigned raId : rsi.alternatives()) {
				  IRouteAlternative &ra = infoSrc.routeAlternative(raId);
				  if(ra.returnTrip()) {
					  for(size_t i = stopsCountM1; i > 0ULL; --i) {
						  // TODO: add edge from stops[i] to stop[i-1]
						  // The edge should contain only:
						  // - raId - to provide access to the route information 
						  // - (stopsCountM1-i) - to know the index of this stop when traversing the route
					  }
				  } else {
					  for(size_t i = 0ULL; i < stopsCountM1; ++i) {
						  // TODO: add edge from stops[i] to stop[i+1]
						  // The edge should contain only:
						  // - raId - to provide access to the route information 
						  // - i - to know the index of this stop when traversing the route
					  }
				  }
			  }
		  }
	  }

    GraphMap(const GraphMap&) = delete;
    GraphMap(GraphMap&&) = delete;
    void operator=(const GraphMap&) = delete;
    void operator=(GraphMap&&) = delete;
    
    /**
	  Searches for itinerary variants between the 2 places.

	  @param idFrom id of the starting location
	  @param idTo id of the destination location
	  @param maxCountPerCategory maximum number of variants
	  for each considered category (price, distance, duration, soonest at destination)
	  @param timeConstraints the imposed periods when to leave and when to arrive

	  @return the found variants for the trip if the places can be connected; nullptr otherwise
	  */
	  unique_ptr<IResults> search(const unsigned idFrom,
                                const unsigned idTo,
                                size_t maxCountPerCategory,
                                const ITimeConstraints &timeConstraints) const {
		  unique_ptr<IResults> results = make_unique<Results>();
		  return results;
	  }
  };

  void TripPlanner::reset() {
    if(nullptr != g) {
      delete g;
      g = nullptr;
      infoSrc->reload();
    }

    g = new GraphMap(*infoSrc);
  }

  unsigned TripPlanner::pickPlace(const string &name,
                                  istream &promptStream/* = cin*/,
                                  ostream &outStream/* = cout*/) const {
    vector<const IfPlace*> possiblePlaces;
    infoSrc->getAllPlacesNamed(name, possiblePlaces);
    if(possiblePlaces.empty())
      throw invalid_argument(string(__func__) + " couldn't find place: "s + name);

    if(possiblePlaces.size() == 1ULL)
      return possiblePlaces.front()->id();

    set<unsigned> possibleIds;
    outStream<<"There are several places with the name `"<<name<<"`:"<<endl;
    for(const IfPlace *p : possiblePlaces) {
      possibleIds.insert(p->id());
      outStream<<'\t'<<p->toString()<<endl;
    }
    outStream<<endl;

    const unsigned invalidId = *possibleIds.crbegin() + 1U; // largest relevant id + 1
    unsigned id;
    for(;;) { // loop until reading a valid id, or until reaching EOS
      for(;;) { // loop until reading an id (valid or not), or until reaching EOS
        outStream<<"Please enter the id of the desired place: ";
        string line;
        if(!getline(promptStream, line))
          throw runtime_error("Couldn't read chosen place id!");
        
        if(line.empty()) {
          outStream<<"\tEmpty line isn't a valid input. Retrying ..."<<endl;
          continue;
        }

        istringstream iss(line);
        id = invalidId;
        if(iss>>id)
          break;

        outStream<<"\t`"<<line<<"` isn't a valid id. Retrying ..."<<endl;
      }

      if(possibleIds.find(id) != cend(possibleIds))
        return id;

      outStream<<"\tProvided id ("<<id<<") isn't among the options. "
        "Retrying ..."<<endl;
    }
  }

  TripPlanner::TripPlanner(unique_ptr<InfoSource> infoSrc_) :
		  infoSrc(move(infoSrc_)) {
	  if(nullptr == infoSrc)
		  throw invalid_argument(string(__func__) + " expects non-null parameter!");
    
    dataAccess.lock(); // block data access until g is built
    allowDataAccess(); // build g and then allow data access
  }

  TripPlanner::~TripPlanner() {
    if(nullptr != g) {
      delete g;
      g = nullptr;
    }

    dataAccess.unlock();
  }

  void TripPlanner::allowDataAccess(bool allowed/* = true*/) {
    if(!allowed) {
      // waits until all ongoing searches have finished
      dataAccess.lock();

      // Now the admin can perform database maintenance.
      // When he/she finishes, allowDataAccess(true) should be called

    } else {
      reset(); // updates g
      dataAccess.unlock();
    }
  }

  unique_ptr<IResults> 
	  TripPlanner::search(const string &fromPlace,
                        const string &toPlace,
                        size_t maxCountPerCategory,
                        const ITimeConstraints *timeConstraints
                          /* = nullptr*/) const {
	  if(fromPlace.compare(toPlace) == 0 || maxCountPerCategory == 0ULL) 
      throw invalid_argument(string(__func__) + " should be called with "
                             "fromPlace != toPlace and maxCountPerCategory > 0!");

    shared_lock<shared_timed_mutex> sharedDataAccess(dataAccess, 50ms);
    if(!sharedDataAccess.owns_lock())
      throw runtime_error(string(__func__) + " couldn't obtain data access!");

	  const ITimeConstraints &constraints =
		  (nullptr != timeConstraints) ? *timeConstraints : defaultConstraints;

    const unsigned idFrom = pickPlace(fromPlace), idTo = pickPlace(toPlace);
    if(idFrom == idTo) {
      ostringstream oss;
      oss<<__func__<<" should be called with fromPlace != toPlace, but `"
        <<fromPlace<<"` and `"<<toPlace<<"` are aliases for the same place!";
      throw invalid_argument(oss.str());
    }

	  return g->search(idFrom, idTo, maxCountPerCategory, constraints);
  }

} // namespace tp
