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
#include "routesBundle.h"
#include "routeAlternative.h"
#include "pricing.h"
#include "customDateTimeProcessor.h"
#include "transpModes.h"
#include "util.h"

#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cassert>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;
using namespace boost::property_tree;
using namespace boost::posix_time;
using namespace boost::gregorian;

/// Manager of the data read from the Json file / stream
struct JsonSource::DataManager {
	map<unsigned, Place> placeWithId;
	map<UniquePlace, Place*> placeWithUniqueTraits;

	/// The bundles; Didn't use vector, since some bundles might be removed,
	/// thus, the remaining indices might not be a continuous sequence
	map<unsigned, RoutesBundle> bundleById;

	/// The bundle (id-s) whose routes include a stop at a given place (id)
	map<unsigned, set<unsigned>> bundlesForPlace;

	unsigned nextRouteAlternativeId = 0U;

	/// All known routes with all their alternatives
	/// Didn't use vector, since some alternatives might be removed,
	/// thus, the remaining indices might not be a continuous sequence
	map<unsigned, RouteAlternative> transpAlternatives;

	/// Add this place unless it`s not new. Throws when not new
	void newPlace(Place &&place) {
		const auto itId = placeWithId.find(place.id());
		if(placeWithId.cend() != itId) {
			ostringstream oss;
			oss<<"The id of the provided place is not unique:\n"
				<<"\tPrevious place: "<<itId->second.toString()
				<<" ; New place: "<<place.toString();
			throw domain_error(oss.str());
		}
		const auto itTraits = placeWithUniqueTraits.find(place);
		if(placeWithUniqueTraits.cend() != itTraits) {
			ostringstream oss;
			oss<<"The traits of the provided place are not unique:\n"
				<<"\tPrevious place: "<<itTraits->second->toString()
				<<" ; New place: "<<place.toString();
			throw domain_error(oss.str());
		}

		Place &p = placeWithId.emplace(place.id(), move(place)).first->second;
		placeWithUniqueTraits.emplace(p, &p);
	}

	/// Extracts the data about the unique places (Mandatory section)
	void extractPlaces(const ptree &tree) {
		for(const auto &placeSubtree : tree.get_child("Scenario.Places")) {
			const auto &place = placeSubtree.second;
			const unsigned id = place.get<unsigned>("id");
			const string name = place.get<string>("name");
			newPlace(Place(id, name));
		}
		if(placeWithId.size() < 2ULL)
			// The exact message of this exception is checked by Unit Tests. 
			throw domain_error("There must be at least 2 places!");
	}

	/// Extracts the stops and the distance between each consecutive ones
	void extractStopsAndDistances(const ptree &tree, RoutesBundle &rb) {
		assert(rb.stopsCount() == 0ULL);
		unsigned placeId = tree.get<unsigned>("StartPlaceId");
		rb.setFirstPlace(placeId);
		bundlesForPlace[placeId].insert(rb.id());
		for(const auto &nextStopSubtree : tree.get_child("Links")) {
			const auto &nextStopAndDist = nextStopSubtree.second;
			placeId = nextStopAndDist.get<unsigned>("NextPlaceId");
			const float dist = nextStopAndDist.get<float>("dist");
			rb.setNextStop(placeId, dist);
			bundlesForPlace[placeId].insert(rb.id());
		}
		assert(rb.stopsCount() >= 2ULL);
	}

	/// Gets the timetable and other details about each alternative of the given route
	/// No route alternative is an accepted case!
	void extractRouteAlternatives(const ptree &alternativesTree,
								  RoutesBundle &rb) {
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
										   rb, esa, bsa,
										   timetable,
										   returnTrip))).first->second;

			auto odw = alternativeInfo.get_optional<string>("ODW");
			if(odw) ra.updateOperationalDaysOfWeek(*odw);
			const auto udya = alternativeInfo.get_optional<string>("UDYA");
			if(udya) ra.updateUnavailDaysForTheYearAhead(*udya);

			rb.addAlternative(ra.id());
			++nextRouteAlternativeId;
		}
	}

	/// Extracts the routes (Mandatory section). No routes is allowed!
	void extractRoutes(const ptree &tree) {
		for(const auto &bundleSubtree : tree.get_child("Scenario.Routes")) {
			const auto &bundle = bundleSubtree.second;
			const unsigned bundleId = bundle.get<unsigned>("bundleId");

			const auto it = bundleById.find(bundleId);
			if(cend(bundleById) != it) {
				ostringstream oss;
				oss<<__FUNCTION__ " detected 2 bundles with same id: "<<bundleId;
				throw domain_error(oss.str());
			}

			const size_t transpMode = TranspModes::fromString(
				bundle.get<string>("TM").c_str());

			// The ticket price rules for all routes within this bundle
			unique_ptr<ITicketPriceCalculator> pricingEng =
				make_unique<TicketPriceCalculator>(
					bundle.get<float>("EF"),
					bundle.get_optional<float>("BF").get_value_or(0.f),
					bundle.get_optional<float>("LFF").get_value_or(1.f),
					bundle.get_optional<float>("HFF").get_value_or(1.f));

			const string odw = bundle.get_optional<string>("ODW").get_value_or("1111111");

			RoutesBundle &rb = bundleById.emplace(
				make_pair(bundleId, RoutesBundle(bundleId,
												transpMode,
												move(pricingEng),
												odw))).first->second;

			rb.updateUnavailDaysForTheYearAhead(
				bundle.get_optional<string>("UDYA").get_value_or(""));

			extractStopsAndDistances(bundle.get_child("Route"), rb);
			extractRouteAlternatives(bundle.get_child("Alternatives"), rb);
		}
	}
};

void JsonSource::cleanup() {
	if(dm) {
		delete dm;
		dm = nullptr;
	}
}

JsonSource::JsonSource(istringstream &&jsonStream) :
		dm(new DataManager) {
	ptree tree;
	try {
		read_json(jsonStream, tree); // Parses the entire json stream.
		// To avoid "invalid code sequence" exceptions,
		// make sure to not use TAB-s within string values!!!!

		dm->extractPlaces(tree);
		dm->extractRoutes(tree);
	} catch(exception &e) {
		cerr<<"Error - Detected an error in the json stream: "
			<<e.what()<<endl;
		cleanup();
		throw;
	}
}

JsonSource::JsonSource(const std::string &jsonFile) :
	JsonSource(istringstream(string(istreambuf_iterator<char>(ifstream(jsonFile)),
									{}))) {}

JsonSource::~JsonSource() {
	cleanup();
}

IfPlace& JsonSource::getPlace(unsigned id) const {
	const auto itId = dm->placeWithId.find(id);
	if(itId != dm->placeWithId.cend())
		return itId->second;

	ostringstream oss;
	oss<<"Error - Cannot locate place with index: "<<id;
	throw invalid_argument(oss.str());
}

IfPlace& JsonSource::getPlace(const IfUniquePlace &uniquePlaceTraits) const {
	const auto itTraits = dm->placeWithUniqueTraits.find(uniquePlaceTraits);
	if(itTraits != dm->placeWithUniqueTraits.cend())
		return *itTraits->second;

	ostringstream oss;
	oss<<"Error - Cannot locate place with traits: "<<uniquePlaceTraits.toString();
	throw invalid_argument(oss.str());
}

void JsonSource::bundlesForPlace(unsigned placeId,
								 set<unsigned> &bundleIds) const {
	IfPlace &p = getPlace(placeId);
	const auto it = dm->bundlesForPlace.find(placeId);
	if(cend(dm->bundlesForPlace) == it) {
		ostringstream oss;
		oss<<__FUNCTION__ " couldn't find any bundle for "<<p.toString();
		throw domain_error(oss.str());
	}

	bundleIds = it->second;
}

void JsonSource::bundlesForPlaces(const set<unsigned> &placeIds,
								  set<unsigned> &bundleIds) const {
	bundleIds.clear();
	for(const unsigned placeId : placeIds) {
		set<unsigned> bundlesForNewPlace, temp;
		bundlesForPlace(placeId, bundlesForNewPlace);
		set_union(CBOUNDS(bundlesForNewPlace),
				  CBOUNDS(bundleIds),
				  inserter(temp, begin(temp)));
		bundleIds = move(temp);
	}
}

IRoutesBundle& JsonSource::getRoutesBundle(unsigned bundleId) const {
	const auto it = dm->bundleById.find(bundleId);
	if(cend(dm->bundleById) == it) {
		ostringstream oss;
		oss<<__FUNCTION__ " couldn't find any bundle with id "<<bundleId;
		throw domain_error(oss.str());
	}

	return it->second;
}

IRouteAlternative& JsonSource::getRouteAlternative(unsigned raId) const {
	const auto it = dm->transpAlternatives.find(raId);
	if(cend(dm->transpAlternatives) == it) {
		ostringstream oss;
		oss<<__FUNCTION__ " couldn't find route alternative with id "<<raId;
		throw domain_error(oss.str());
	}

	return it->second;
}
