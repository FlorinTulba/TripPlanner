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

#include "jsonSource.h"
#include "place.h"
#include "variantsBase.h"
#include "routeSharedInfo.h"
#include "routeAlternative.h"
#include "pricing.h"
#include "customDateTimeProcessor.h"
#include "transpModes.h"
#include "util.h"

#pragma warning ( push, 0 )

#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cassert>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#pragma warning ( pop )

using namespace std;
using namespace boost::property_tree;
using namespace boost::posix_time;
using namespace boost::gregorian;

// namespace trip planner - specifications
namespace tp { namespace specs {

  /**
  Given the array of pointers to places with the same name and
  sorted by their description, it searches the position of the place
  matching descr. When descr isn`t found, it stops on the first place with
  a description lexicographically larger than descr and reports that index.
  */
  static bool matchDescription(const vector<const IfPlace*> &sameNamePlaces,
                               const string &descr,
                               size_t &matchOrInsertPosition) {
    const auto itDescr = lower_bound(CBOUNDS(sameNamePlaces), descr,
                                     [] (const IfPlace *p,
                                         const string &refVal) {
      assert(nullptr != p);
      return p->shortDescr().compare(refVal) < 0;
    });

    matchOrInsertPosition = (size_t)distance(cbegin(sameNamePlaces), itDescr);
    
    return cend(sameNamePlaces) != itDescr &&
      (*itDescr)->shortDescr().compare(descr) == 0;
  }

  /// Manager of the data read from the Json file / stream
  struct JsonSource::DataManager {
    /// Correlation between a place and the routes passing through it
    struct PlaceData {
      Place info; ///< the details of the place
      set<unsigned> coveringRoutes; ///< the routes passing through the place

      PlaceData(const Place &p) : info(p) {}
      PlaceData(Place &&p) : info(move(p)) {}
      PlaceData(const PlaceData&) = default;
      PlaceData(PlaceData&&) = default;

      PlaceData& operator=(const PlaceData&) = default;
      PlaceData& operator=(PlaceData&&) = default;
    };

    /// Complete information for a place plus the routes passing through it
	  map<unsigned, PlaceData> placeDataById;

    /// Provides the pointer to the information for a place at a given location
    map<GpsCoord<float>, IfPlace*> placeByGps;

    /// Provides the pointers to the places with a given name, sorted by their description
    map<string, vector<const IfPlace*>> placesByName;

	  /// The routes; Didn't use vector, since some routes might be removed,
	  /// thus, the remaining indices might not be a continuous sequence
	  map<unsigned, RouteSharedInfo> rsiById;

	  /// The counter that assigns unique id-s to route alternatives
	  unsigned nextRouteAlternativeId = 0U;

	  /// All known routes with all their alternatives
	  /// Didn't use vector, since some alternatives might be removed,
	  /// thus, the remaining indices might not be a continuous sequence
	  map<unsigned, RouteAlternative> transpAlternatives;

	  /// Add this place unless it`s not new. Throws when not new
	  void newPlace(Place &&place) {
		  const auto itId = placeDataById.find(place.id());
		  if(placeDataById.cend() != itId) {
			  ostringstream oss;
			  oss<<"The id of the provided place is not unique:\n"
				  <<"\tPrevious place: "<<itId->second.info.toString()
				  <<" ; New place: "<<place.toString();
			  throw domain_error(oss.str());
		  }
      Place &p = placeDataById.emplace(place.id(), PlaceData(move(place))).
        first->second.info;

      const auto itGps = placeByGps.find(p.gpsCoord());
      if(placeByGps.cend() != itGps) {
        ostringstream oss;
        oss<<"The location of the provided place is not unique:\n"
          <<"\tPrevious place: "<<itGps->second->toString()
          <<" ; New place: "<<p.toString();
        throw domain_error(oss.str());
      }
      placeByGps.emplace(p.gpsCoord(), &p);

      for(const string &placeName : p.names()) {
        vector<const IfPlace*> &sameNamePlaces = placesByName[placeName];
        size_t matchOrInsertPosition;
        if(matchDescription(sameNamePlaces, p.shortDescr(),
                            matchOrInsertPosition)) {
          ostringstream oss;
          oss<<"Found 2 places with same (alias, description) combination:\n"
            <<"\tPrevious place: "
            <<sameNamePlaces[matchOrInsertPosition]->toString()
            <<" ; New place: "<<p.toString();
          throw domain_error(oss.str());
        }
        
        // Insert p at matchOrInsertPosition (shifting the rest to the right)
        size_t idx = sameNamePlaces.size() + 1ULL;
        sameNamePlaces.resize(idx);
        while(--idx > matchOrInsertPosition)
          sameNamePlaces[idx] = sameNamePlaces[idx-1];
        sameNamePlaces[matchOrInsertPosition] = &p;
      }
	  }

	  /// Extracts the data about the unique places (Mandatory section)
	  void extractPlaces(const ptree &tree) {
		  for(const auto &placeSubtree : tree.get_child("Scenario.Places")) {
			  const auto &place = placeSubtree.second;
			  const unsigned id = place.get<unsigned>("id");
        const string names = place.get<string>("names");
        const string descr =
          place.get_optional<string>("descr").get_value_or(u8"");
        const radians<float>
          lat = radians<float>::fromDegrees(place.get<float>("lat")),
          lon = radians<float>::fromDegrees(place.get<float>("long"));
        newPlace(Place(id, GpsCoord<float>(lat, lon), names, descr));
		  }
		  if(placeDataById.size() < 2ULL)
			  // The exact message of this exception is checked by Unit Tests. 
			  throw domain_error("There must be at least 2 places!");
	  }

	  /// Extracts the stops and the distance between all consecutive ones
	  void extractStopsAndDistances(const ptree &tree, RouteSharedInfo &rsi) {
		  assert(rsi.stopsCount() == 0ULL);
		  unsigned placeId = tree.get<unsigned>("StartPlaceId");
      try {
        placeDataById.at(placeId). // throws out_of_range for unrecognized place id
          coveringRoutes.insert(rsi.id());
        rsi.setFirstPlace(placeId);
        for(const auto &nextStopSubtree : tree.get_child("Links")) {
          const auto &nextStopAndDist = nextStopSubtree.second;
          placeId = nextStopAndDist.get<unsigned>("NextPlaceId");
          placeDataById.at(placeId). // throws out_of_range for unrecognized place id
            coveringRoutes.insert(rsi.id());
          const float dist = nextStopAndDist.get<float>("dist");
          rsi.setNextStop(placeId, dist);
        }
      } catch(out_of_range&) {
        ostringstream oss;
        oss<<__func__<<" detected that route with RouteId = "<<rsi.id()
          <<" refers to an unknown place id: "<<placeId;
        throw domain_error(oss.str());
      }

      if(rsi.stopsCount() < 2ULL) {
        ostringstream oss;
        oss<<__func__<<" detected that route with RouteId = "<<rsi.id()
          <<" has no links (Links is empty)!";
        throw domain_error(oss.str());
      }
	  }

	  /// Gets the timetable and other details about each alternative of the given route
	  void extractRouteAlternatives(const ptree &alternativesTree,
                                  RouteSharedInfo &rsi) {
		  for(const auto &alternativeInfoSubtree : alternativesTree) {
			  const auto &alternativeInfo = alternativeInfoSubtree.second;
			  const unsigned esa = alternativeInfo.get<unsigned>("ESA");
			  const unsigned bsa =
				  alternativeInfo.get_optional<unsigned>("BSA").get_value_or(0U);
			  const bool returnTrip =
				  alternativeInfo.get_optional<bool>("ReturnTrip").get_value_or(false);
			  const string timetable = alternativeInfo.get<string>("TT");
			  RouteAlternative &ra = transpAlternatives.emplace(
				  make_pair(nextRouteAlternativeId,
                    RouteAlternative(nextRouteAlternativeId,
                                     rsi, esa, bsa,
                                     timetable,
                                     returnTrip))).first->second;

			  auto odw = alternativeInfo.get_optional<string>("ODW");
			  if(odw) ra.updateOperationalDaysOfWeek(*odw);
			  const auto udya = alternativeInfo.get_optional<string>("UDYA");
			  if(udya) ra.updateUnavailDaysForTheYearAhead(*udya);

			  rsi.addAlternative(ra.id());
			  ++nextRouteAlternativeId;
		  }

      if(rsi.alternatives().size() == 0ULL) {
        ostringstream oss;
        oss<<__func__<<" detected that route with RouteId = "<<rsi.id()
          <<" has no alternatives (Alternatives is empty)!";
        throw domain_error(oss.str());
      }
	  }

	  /// Extracts the routes (Mandatory section)
	  void extractRoutes(const ptree &tree) {
		  for(const auto &routesSubtree : tree.get_child("Scenario.Routes")) {
			  const auto &rsiProvider = routesSubtree.second;
			  const unsigned routeSharedInfoId = rsiProvider.get<unsigned>("RouteId");

			  const auto it = rsiById.find(routeSharedInfoId);
			  if(cend(rsiById) != it)
				  throw domain_error(string(__func__) +
                             " detected 2 routes with same id: "s +
                             to_string(routeSharedInfoId));

			  const size_t transpMode = TranspModes::fromString(
				  rsiProvider.get<string>("TM").c_str());

			  // The ticket price rules for all route alternatives of this route
			  unique_ptr<ITicketPriceCalculator> pricingEng =
				  make_unique<TicketPriceCalculator>(
					  rsiProvider.get<float>("EF"),
					  rsiProvider.get_optional<float>("BF").get_value_or(0.f),
					  rsiProvider.get_optional<float>("LFF").get_value_or(1.f),
					  rsiProvider.get_optional<float>("HFF").get_value_or(1.f));

			  const string odw = rsiProvider.get_optional<string>("ODW").
				  get_value_or("1111111");

			  const string udya = rsiProvider.get_optional<string>("UDYA").
				  get_value_or("");
			  RouteSharedInfo &rsi = rsiById.emplace(
				  make_pair(routeSharedInfoId, RouteSharedInfo(routeSharedInfoId,
                                                       transpMode,
                                                       move(pricingEng),
                                                       udya,
                                                       odw))).first->second;

			  extractStopsAndDistances(rsiProvider.get_child("Route"), rsi);
			  extractRouteAlternatives(rsiProvider.get_child("Alternatives"), rsi);
		  }

      const auto uncoveredPlace = [] (const pair<unsigned, PlaceData> &pd) {
        return pd.second.coveringRoutes.empty();
      };

      auto uncoveredPlaceIt = find_if(CBOUNDS(placeDataById), uncoveredPlace);
      if(cend(placeDataById) != uncoveredPlaceIt) {
        ostringstream oss;
        oss<<__func__<<" detected that following places are not covered by "
          "any route: `"<<uncoveredPlaceIt->second.info.toString()<<'`';
        while(cend(placeDataById) != 
              (uncoveredPlaceIt = find_if(next(uncoveredPlaceIt),
                                          cend(placeDataById),
                                          uncoveredPlace)))
          oss<<" `"<<uncoveredPlaceIt->second.info.toString()<<'`';
        throw domain_error(oss.str());
      }
	  }
  };

  void JsonSource::cleanup() {
	  if(nullptr != dm) {
		  delete dm;
		  dm = nullptr;
	  }
  }

  void JsonSource::reload(istringstream &&jsonStream) {
    cleanup();
	  ptree tree;
	  try {
		  read_json(jsonStream, tree); // Parses the entire json stream.
		  // To avoid "invalid code sequence" exceptions,
		  // make sure to not use TAB-s within string values!!!!

      dm = new DataManager;
		  dm->extractPlaces(tree);
		  dm->extractRoutes(tree);

	  } catch(exception &e) {
		  cerr<<"Error - Detected an error in the json stream: "
			  <<e.what()<<endl;
		  cleanup();
		  throw;
	  }
  }

  JsonSource::JsonSource(const string &jsonContent_) :
      jsonContent(&jsonContent_) {
    reload();
  }

  JsonSource::JsonSource(const boost::filesystem::path &jsonFile_) :
      jsonFile(jsonFile_) {
    reload();
  }

  JsonSource::~JsonSource() {
	  cleanup();
  }

  void JsonSource::reload() {
    string jsonString;

    if(nullptr == jsonContent) { // reload from file
      ifstream ifs(jsonFile.string());
      jsonString.assign(
        istreambuf_iterator<char>(ifs),
        {} // end istreambuf_iterator
      );

    }  else jsonString = *jsonContent; // reload from (updated) string
    
    reload(istringstream(jsonString));
  }

  void JsonSource::idsOfAllPlaces(vector<unsigned> &placeIds) const {
    placeIds.clear(); placeIds.reserve(dm->placeDataById.size());
	  for(const auto &mapPair : dm->placeDataById)
		  placeIds.push_back(mapPair.first);
  }

  const IfPlace& JsonSource::getPlace(unsigned id) const try {
    return dm->placeDataById.at(id).info;
  } catch(out_of_range&) {
    throw invalid_argument("Error - Cannot locate place with index: "s +
                           to_string(id));
  }

  const IfPlace& JsonSource::getPlace(const GpsCoord<float>& gps) const try {
    return *dm->placeByGps.at(gps);
  } catch(out_of_range&) {
    throw invalid_argument("Error - Cannot locate place located at: "s +
                           gps.toString());
  }

  const IfPlace& JsonSource::getPlace(const string &knownAs,
                                      const string &shortDescr/* = u8""*/) const {
    const auto itName = dm->placesByName.find(knownAs);
    if(itName == cend(dm->placesByName))
      throw invalid_argument(string(__func__) + " couldn't recognize place "s +
                             knownAs);
    
    const vector<const IfPlace*> &foundPlaces = itName->second;
    
    size_t matchPos;
    if(!matchDescription(foundPlaces, shortDescr, matchPos)) {
      ostringstream oss;
      oss<<__func__<<" couldn't match description `"<<shortDescr
        <<"` among the places named `"<<knownAs<<'`';
      throw invalid_argument(oss.str());
    }

    return *foundPlaces[matchPos];
  }

  void JsonSource::getAllPlacesNamed(const string &name,
                                     vector<const IfPlace*> &places) const try {
    places = dm->placesByName.at(name);
  } catch(out_of_range&) {
    places.clear();
  }

  void JsonSource::idsOfAllRoutes(vector<unsigned> &routeSharedInfoIds) const {
    routeSharedInfoIds.clear(); routeSharedInfoIds.reserve(dm->rsiById.size());
    for(const auto &rsiPair : dm->rsiById)
      routeSharedInfoIds.push_back(rsiPair.first);
  }

  void JsonSource::routesForPlace(unsigned placeId,
                                  vector<unsigned> &routeSharedInfoIds) const {
	  getPlace(placeId);
    assert(cend(dm->placeDataById) != dm->placeDataById.find(placeId));
    const set<unsigned> &coveringRoutes =
      dm->placeDataById.at(placeId).coveringRoutes;
    routeSharedInfoIds.assign(CBOUNDS(coveringRoutes));
  }

  void JsonSource::routesForPlaces(const set<unsigned> &placeIds,
                                   vector<unsigned> &routeSharedInfoIds) const {
	  routeSharedInfoIds.clear();
	  for(const unsigned placeId : placeIds) {
      vector<unsigned> routesForNewPlace, temp;
		  routesForPlace(placeId, routesForNewPlace);
		  set_union(CBOUNDS(routesForNewPlace),
                CBOUNDS(routeSharedInfoIds),
                back_inserter(temp));
		  routeSharedInfoIds = move(temp);
	  }
  }

  IRouteSharedInfo&
      JsonSource::routeSharedInfo(unsigned routeSharedInfoId) const try {
	  return dm->rsiById.at(routeSharedInfoId);
  } catch(out_of_range&) {
    throw domain_error(string(__func__) + " couldn't find any route with id "s +
                       to_string(routeSharedInfoId));
  }

  IRouteAlternative& JsonSource::routeAlternative(unsigned raId) const try {
	  return dm->transpAlternatives.at(raId);
  } catch(out_of_range&) {
    throw domain_error(string(__func__) +
                       " couldn't find route alternative with id "s +
                       to_string(raId));
  }

}} // namespace tp::specs
