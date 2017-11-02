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

#include "CppUnitTest.h"
#include "jsonSource.h"
#include "place.h"
#include "transpModes.h"
#include "customDateTimeProcessor.h"
#include "util.h"

#include <boost/date_time/gregorian/parsers.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace tp::specs;
using namespace tp::var;
using namespace boost::filesystem;

namespace UnitTests {
	TEST_CLASS(JsonSource) {
	public:
		TEST_METHOD(JsonSource_CorrectFile_NormalResults) {
			Logger::WriteMessage(__FUNCTION__);

			try {
				// Force the test to always run as if on 2017-Jul-10
				nowReplacements.resize(5ULL, ptime(from_simple_string("2017-Jul-10"s)));

        tp::specs::JsonSource js(path("../../UnitTests/TestFiles/specsOk.json"));

				const auto &p4 = js.getPlace(4U);
        const auto &p4Loc = p4.gpsCoord();
        Assert::IsTrue(p4.shortDescr().empty());
        Assert::AreEqual(2ULL, p4.names().size());
        Assert::AreEqual(u8"∃y ∀x ¬(x ≺ y)"s, p4.names().front());
        Assert::AreEqual(u8"p4"s, p4.names().back());
        Assert::AreEqual(45.f, p4Loc.latitude().get(true), 1e-3f);
        Assert::AreEqual(90.f, p4Loc.longitude().get(true), 1e-3f);

				const auto &p8 =
          js.getPlace(GpsCoord<float>((float) 11.25_deg, (float) 22.5_deg));
				Assert::AreEqual(8U, p8.id());
        Assert::IsTrue(p8.shortDescr().empty());
        Assert::AreEqual(2ULL, p8.names().size());
        Assert::AreEqual(u8"Приве́т नमस्ते שָׁלוֹם"s, p8.names().front());
        Assert::AreEqual(u8"p8"s, p8.names().back());

        Assert::ExpectException<invalid_argument>([&js] {
          js.getPlace(0U); // No such place
        });

        Assert::ExpectException<invalid_argument>([&js] {
          js.getPlace(16U); // No such place
        });

        Assert::ExpectException<invalid_argument>([&js] {
          // No such place
          js.getPlace(GpsCoord<float>((float) 111.25_deg, (float) 2.5_deg));
        });

        vector<const IfPlace*> foundPlaces;
        js.getAllPlacesNamed(u8"p16", foundPlaces); // No such place
        Assert::IsTrue(foundPlaces.empty());

        Assert::ExpectException<invalid_argument>([&js] {
          js.getPlace(u8"p16"); // No such place
        });

        js.getAllPlacesNamed(u8"pp", foundPlaces);
        // Places with ids: 1,3,6,7, sorted by their descriptions:
        // "" for id3, "descr1" for id1, "descr6" for id6 and "descr7" for id7
        Assert::IsTrue(foundPlaces.size() == 4ULL);
        Assert::AreEqual(3U, foundPlaces[0ULL]->id());
        Assert::AreEqual(1U, foundPlaces[1ULL]->id());
        Assert::AreEqual(6U, foundPlaces[2ULL]->id());
        Assert::AreEqual(7U, foundPlaces[3ULL]->id());

        Assert::ExpectException<invalid_argument>([&js] {
          js.getPlace(u8"pp", u8"descr16"); // No such place
        });

        Assert::ExpectException<invalid_argument>([&js] {
					vector<unsigned> routes;
					js.routesForPlace(16U, routes); // No such place
				});

        vector<unsigned> routes, expected { 1U, 2U, 3U };
				js.routesForPlace(1U, routes);
				Assert::IsTrue(equal(CBOUNDS(routes), cbegin(expected)));

				js.routesForPlace(2U, routes);
				expected = { 1U };
				Assert::IsTrue(equal(CBOUNDS(routes), cbegin(expected)));

				js.routesForPlace(13U, routes);
				expected = { 2U, 4U };
				Assert::IsTrue(equal(CBOUNDS(routes), cbegin(expected)));

				js.routesForPlaces({ 1U, 13U }, routes);
				expected = { 1U, 2U, 3U, 4U };
				Assert::IsTrue(equal(CBOUNDS(routes), cbegin(expected)));

				js.routesForPlaces({ 1U, 2U }, routes);
				expected = { 1U, 2U, 3U };
				Assert::IsTrue(equal(CBOUNDS(routes), cbegin(expected)));

				js.routesForPlaces({ 2U, 13U }, routes);
				expected = { 1U, 2U, 4U };
				Assert::IsTrue(equal(CBOUNDS(routes), cbegin(expected)));

				Assert::ExpectException<domain_error>([&js] {
					js.routeSharedInfo(0U); // No such route
				});
				
				const auto &r1 = js.routeSharedInfo(1U);
				// "RouteId":1, "TM" : "Road", "ODW" : "0111110",
				// "UDYA" : "Jan-1|Jan-2|Jan-3|Apr-4|July-4|Jul-14|Dec-25|Dec-31",
				// "EF" : 3.5
				Assert::AreEqual(1U, r1.id());
				Assert::AreEqual(size_t(TranspModes::ROAD), r1.transpMode());
				Assert::AreEqual(0b0111110UL,
								 r1.customizableInfo().operationalDaysOfWeek()->to_ulong());

				const set<date> &udyaR1 =
					*r1.customizableInfo().unavailDaysForTheYearAhead();
				const auto endIt = cend(udyaR1);
				/*
				Unoperational days are updated at the start of each month for the year ahead.
				So, on the k-th day of a month, all previous k-1 days of that month
				can be ignored, as they remain in the past.
				
				Since 'today' was set as 2017-Jul-10, July-4 can be ignored (after the start of the month and before today),
				thus there remain only 7 relevant unoperational dates:

				2018-Jan-1, 2018-Jan-2, 2018-Jan-3, 2018-Apr-4
				2017-Jul-14, 2017-Dec-25, 2017-Dec-31
				*/
				Assert::AreEqual(7ULL, udyaR1.size());
				Assert::IsTrue(endIt != udyaR1.find(from_simple_string("2018-Jan-1"s)));
				Assert::IsTrue(endIt != udyaR1.find(from_simple_string("2018-Jan-2"s)));
				Assert::IsTrue(endIt != udyaR1.find(from_simple_string("2018-Jan-3"s)));
				Assert::IsTrue(endIt != udyaR1.find(from_simple_string("2018-Apr-4"s)));
				Assert::IsTrue(endIt == udyaR1.find(from_simple_string("2017-July-4"s)));
				Assert::IsTrue(endIt != udyaR1.find(from_simple_string("2017-July-14"s)));
				Assert::IsTrue(endIt != udyaR1.find(from_simple_string("2017-Dec-25"s)));
				Assert::IsTrue(endIt != udyaR1.find(from_simple_string("2017-Dec-31"s)));

				Assert::AreEqual(1.f, // ln(1+3.5*((e-1)/3.5)) == 1
								 r1.pricingEngine().normalFare(expm1f(1.f)/3.5f),
								 1e-3f);
				Assert::AreEqual(6ULL, r1.stopsCount());
				Assert::IsFalse(r1.containsStop(0U));
				Assert::IsFalse(r1.containsStop(7U));
				
				vector<float> expectedDistances { 90.5f, 70.2f, 93.4f, 115.9f, 208.6f };
				const vector<float> &returnedDistances = r1.distances();
				Assert::IsTrue(equal(CBOUNDS(expectedDistances),
									 CBOUNDS(returnedDistances), []
									 (float f1, float f2) {
					return fabs(f1 - f2) < 1e-3f;
				}));

				for(unsigned placeId = 1U; placeId <= 5U; ++placeId) {
					Assert::IsTrue(r1.containsStop(placeId));
					Assert::AreEqual(placeId, r1.nthStop(placeId - 1U, false));
					Assert::AreEqual(7U - placeId, r1.nthStop(placeId - 1U, true));
					Assert::AreEqual(expectedDistances[placeId - 1U],
									 r1.nthDistance(placeId - 1U, false),
									 1e-3f);
					Assert::AreEqual(expectedDistances[5U - placeId],
									 r1.nthDistance(placeId - 1U, true),
									 1e-3f);
				}
				
				Assert::IsTrue(r1.containsStop(6U));
				Assert::AreEqual(6U, r1.nthStop(5U, false));
				Assert::AreEqual(1U, r1.nthStop(5U, true));

				Assert::ExpectException<out_of_range>([&r1] {
					r1.nthStop(6U, true); // max index is 5
				});
				Assert::ExpectException<out_of_range>([&r1] {
					r1.nthStop(6U, false); // max index is 5
				});
				Assert::ExpectException<out_of_range>([&r1] {
					r1.nthDistance(5U, true); // max index is 4
				});
				Assert::ExpectException<out_of_range>([&r1] {
					r1.nthDistance(5U, false); // max index is 4
				});

				vector<unsigned> expectedStops { 1U, 2U, 3U, 4U, 5U, 6U };
				const vector<unsigned> &returnedStops = r1.traversedStops();
				Assert::IsTrue(equal(CBOUNDS(expectedStops),
									 CBOUNDS(returnedStops)));

				set<unsigned> expectedRAs { 0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U };
				const set<unsigned> &returnedRAs = r1.alternatives();
				Assert::IsTrue(equal(CBOUNDS(expectedRAs),
									 CBOUNDS(returnedRAs)));

				Assert::ExpectException<domain_error>([&js] {
					js.routeAlternative(50U); // Not that many routes
				});

				IRouteAlternative &ra5 = js.routeAlternative(5U);
				// "ESA" : 50, "ReturnTrip" : true, "ODW" : "0100000",
				// "UDYA" : "Jan-1|Jan-2|Jan-3|Mar-7|Apr-4|July-4|Jul-14|Dec-25|Dec-31",
				// "TT" : "0:30-4:30|5:0-7:15|7:30-9:0|9:15-10:20|10:30-12:0"}

				Assert::AreEqual(5U, ra5.id());
				Assert::AreEqual(1U, ra5.routeSharedInfo().id());
				Assert::IsTrue(ra5.returnTrip());

				Assert::AreEqual(0b0111110UL, // this was within the shared route information
								 ra5.routeSharedInfo().customizableInfo().
								 operationalDaysOfWeek()->to_ulong());
				Assert::AreEqual(0b0000010UL, ra5.operationalDaysOfWeek()->to_ulong());
				// in binary, the string "0100000" (Mondays only) gets reversed: 0000010
				Assert::IsTrue(ra5.operationalDaysOfWeek()->test(Monday));
				
				Assert::AreEqual(7ULL, // this was within the shared route information
								 ra5.routeSharedInfo().customizableInfo().
								 unavailDaysForTheYearAhead()->size());
				const set<date> &udyaRa5 = *ra5.unavailDaysForTheYearAhead();
				// "UDYA" : "Jan-1|Jan-2|Jan-3|Mar-7|Apr-4|July-4|Jul-14|Dec-25|Dec-31"
				// translates to:
				// 2018-Jan-1, 2018-Jan-2, 2018-Jan-3, 2018-Mar-7, 2018-Apr-4
				// 2017-Jul-14, 2017-Dec-25, 2017-Dec-31
				Assert::AreEqual(8ULL, udyaRa5.size());
				Assert::IsTrue(cend(udyaRa5) != udyaRa5.find(from_simple_string("2018-Mar-7"s)));
				
				Assert::AreEqual(50U, ra5.economySeatsCapacity());
				Assert::AreEqual(0U, ra5.businessSeatsCapacity());

				const vector<time_period> &returnedTimetable = ra5.timetable();
				Assert::AreEqual(5ULL, returnedTimetable.size());
				const time_period &firstInterval = returnedTimetable.front(),
					&lastInterval = returnedTimetable.back();
				const auto startFirstInterval = firstInterval.begin().time_of_day();
				Assert::AreEqual(0, startFirstInterval.hours());
				Assert::AreEqual(30, startFirstInterval.minutes());
				const auto endFirstInterval = firstInterval.last().time_of_day();
				Assert::AreEqual(4, endFirstInterval.hours());
				Assert::AreEqual(30, endFirstInterval.minutes());
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
			nowReplacements.clear(); // make sure other tests are not affected
		}

		TEST_METHOD(JsonSource_TabsInStrings_InvalidCodeSequence) {
			Logger::WriteMessage(__FUNCTION__);

			Assert::ExpectException<exception>([] {
				// Strings containing TABs
				const string str(R"({"root":"strings	with	tabs	are	not ok!"})");
        tp::specs::JsonSource js(str);
			});
		}

		TEST_METHOD(JsonSource_NotUniquePlaceTraits_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			Assert::ExpectException<domain_error>([] {
				// Duplicate place id-s
				const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0},
	{"id":1, "names":"p3", "lat":2, "long":0}
]}})");
        tp::specs::JsonSource js(str);
			});

			Assert::ExpectException<domain_error>([] {
				// Duplicate place names when no place descriptions available
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0},
	{"id":3, "names":"p2", "lat":2, "long":0}
]}})");
        tp::specs::JsonSource js(str);
			});

      Assert::ExpectException<domain_error>([] {
        // Duplicate (place alias, place description) pair
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"pp|p1", "descr" : "dd", "lat":0, "long":0},
	{"id":2, "names":"p2|pp", "descr" : "dd", "lat":1, "long":0}
]}})");
        tp::specs::JsonSource js(str);
      });

      Assert::ExpectException<domain_error>([] {
        // Duplicate GPS
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":0, "long":0}
]}})");
        tp::specs::JsonSource js(str);
      });
    }

    TEST_METHOD(JsonSource_LessThan2Places_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      // No section for places at all
      Assert::ExpectException<exception>([] {
        const string str(R"({"Scenario": {}})");
        tp::specs::JsonSource js(str);
      });

      // No place defined
      Assert::ExpectException<domain_error>([] {
        const string str(R"({"Scenario": { "Places" : [
]}})");
        tp::specs::JsonSource js(str);
      });

      // Single place defined
      Assert::ExpectException<domain_error>([] {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0}
]}})");
        tp::specs::JsonSource js(str);
      });

      // For >= 2 places shouldn't throw
      // domain_error("There must be at least 2 places!")
      try {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}
]}})");
        tp::specs::JsonSource js(str);

      } catch(exception &e) {
        // The stream is providing only the part concerning the places and
        // it should throw some exception about missing the rest of the information
        Assert::AreNotEqual("There must be at least 2 places!", e.what());
      }
    }

    TEST_METHOD(JsonSource_FoundDuplicateRouteSharedInfoId_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			// There are routes with the same id
			Assert::ExpectException<domain_error>([] {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":6, "dist" : 208.6}]},
		"Alternatives" : []},

	{"RouteId":1, "TM" : "Rail", "EF" : 4,
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":13, "dist" : 30.3}]},
		"Alternatives" : []}
]}})");
        tp::specs::JsonSource js(str);
			});
		}

    TEST_METHOD(JsonSource_NoRouteLinks_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      // The section Links is empty
      Assert::ExpectException<domain_error>([] {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
		"Route" : {"StartPlaceId":1, "Links" : []},
		"Alternatives" : []}
]}})");
        tp::specs::JsonSource js(str);
      });
    }

    TEST_METHOD(JsonSource_NoRouteAlternatives_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      // The section Alternatives is empty
      Assert::ExpectException<domain_error>([] {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
		"Route" : {"StartPlaceId":1, "Links" : [
      {"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : []}
]}})");
        tp::specs::JsonSource js(str);
      });
    }

    TEST_METHOD(JsonSource_UncoveredPlaces_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      // Not routes at all
      Assert::ExpectException<domain_error>([] {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": []}})");
        tp::specs::JsonSource js(str);
      });

      // Not enough routes (Places p0 and p3 are not covered)
      Assert::ExpectException<domain_error>([] {
        const string str(R"({"Scenario": { "Places" : [
  {"id":0, "names":"p0", "lat":0, "long":0},
	{"id":1, "names":"p1", "lat":1, "long":0},
	{"id":2, "names":"p2", "lat":2, "long":0},
	{"id":3, "names":"p3", "lat":3, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
		"Route" : {"StartPlaceId":1, "Links" : [
      {"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [{"ESA" : 20, "TT" : "7:30-9:0"}]}
]}})");
        tp::specs::JsonSource js(str);
      });
    }

    TEST_METHOD(JsonSource_UnknownRoutePlaces_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      // Unknown StartPlaceId
      // Fails to catch domain_error, as it was thrown
      // Same test performed outside the unit tests works with domain_error.
      Assert::ExpectException<exception>([] {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
		"Route" : {"StartPlaceId":3, "Links" : [
      {"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [{"ESA" : 20, "TT" : "7:30-9:0"}]}
]}})");
        tp::specs::JsonSource js(str);
      });

      // Unknown NextPlaceId
      // Fails to catch domain_error, as it was thrown
      // Same test performed outside the unit tests works with domain_error.
      Assert::ExpectException<exception>([] {
        const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0},
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
		"Route" : {"StartPlaceId":1, "Links" : [
      {"NextPlaceId":3, "dist" : 208.6}]},
		"Alternatives" : [{"ESA" : 20, "TT" : "7:30-9:0"}]}
]}})");
        tp::specs::JsonSource js(str);
      });
    }

    TEST_METHOD(JsonSource_WrongCustomizedOdw_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			// The alternative of the route has more operational days within a week:
			// 1111111 (customized) versus 0111110 (previously)
			Assert::ExpectException<domain_error>([] {
				const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
	"ODW": "0111110",
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [
			{"ESA" : 20, "TT" : "7:30-9:0",
			"ODW" : "1111111"}
]}]}})");
        tp::specs::JsonSource js(str);
			});

			// The alternative of the route has a new operational day within a week:
			// 1111100 (customized) versus 0111110 (previously)
			Assert::ExpectException<domain_error>([] {
				const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
	"ODW": "0111110",
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [
			{"ESA" : 20, "TT" : "7:30-9:0",
			"ODW" : "1111100"}
]}]}})");
        tp::specs::JsonSource js(str);
			});

			// Correct update of ODW for a route alternative
			// customized: 0111100 (Mon-Thu); previously: 0111110 (Mon-Fri)
			try {
				const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
	"ODW": "0111110",
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [
			{"ESA" : 20, "TT" : "7:30-9:0",
			"ODW" : "0111100"}
]}]}})");
        tp::specs::JsonSource js(str);
				const IRouteAlternative &ra0 = js.routeAlternative(0U);
				Assert::IsTrue(ra0.routeSharedInfo().customizableInfo().
							   operationalDaysOfWeek()->test(Friday));
				Assert::IsFalse(ra0.operationalDaysOfWeek()->test(Friday));

			} catch(domain_error &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}

		TEST_METHOD(JsonSource_WrongCustomizedUdya_Throws) {
			Logger::WriteMessage(__FUNCTION__);

      const ptime refMoment(from_simple_string("2017-Sep-16"));

      // Make sure the next 2 configurations of UDYA consider that 'today' is 2017-Sep-16
      nowReplacements.resize(2ULL, refMoment);
      
      // The alternative of the route has less unoperational days:
			// Empty set (customized) versus Jan-3|Apr-4|July-4 (previously)
			Assert::ExpectException<domain_error>([] {
				const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
	"UDYA": "Jan-3|Apr-4|July-4",
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [
			{"ESA" : 20, "TT" : "7:30-9:0",
			"UDYA" : ""}
]}]}})");
        tp::specs::JsonSource js(str);
			});

      // Make sure the next 2 configurations of UDYA consider that 'today' is 2017-Sep-16
      nowReplacements.resize(2ULL, refMoment);
      
      // The alternative of the route forgets about a previous unoperational day (Jan-3):
			// Apr-4|July-4|Aug-8 (customized) versus Jan-3|Apr-4|July-4 (previously)
			Assert::ExpectException<domain_error>([] {
				const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
	"UDYA": "Jan-3|Apr-4|July-4",
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [
			{"ESA" : 20, "TT" : "7:30-9:0",
			"UDYA" : "Apr-4|July-4|Aug-8"}
]}]}})");
        tp::specs::JsonSource js(str);
			});


			// Make sure the next 2 configurations of UDYA consider that 'today' is 2017-Sep-16
			nowReplacements.resize(2ULL, refMoment);

			// Correct update of UDYA for a route alternative (added Aug-8)
			// customized: Jan-3|Apr-4|July-4|Aug-8; previously: Jan-3|Apr-4|July-4
			try {
				const string str(R"({"Scenario": { "Places" : [
	{"id":1, "names":"p1", "lat":0, "long":0},
	{"id":2, "names":"p2", "lat":1, "long":0}],
"Routes": [
	{"RouteId":1, "TM" : "Road", "EF" : 3.5,
	"UDYA": "Jan-3|Apr-4|July-4",
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":2, "dist" : 208.6}]},
		"Alternatives" : [
			{"ESA" : 20, "TT" : "7:30-9:0",
			"UDYA" : "Jan-3|Apr-4|July-4|Aug-8"}
]}]}})");
        tp::specs::JsonSource js(str);
				const IRouteAlternative &ra0 = js.routeAlternative(0U);
				const set<date> &previously = *ra0.routeSharedInfo().customizableInfo().
					unavailDaysForTheYearAhead(),
					&customized = *ra0.unavailDaysForTheYearAhead();
				Assert::IsTrue(previously.size() == 3ULL);
				Assert::IsTrue(cend(previously) ==
							   previously.find(from_simple_string("2018-Aug-8")));
				Assert::IsTrue(customized.size() == 4ULL);
				Assert::IsTrue(cend(customized) !=
							   customized.find(from_simple_string("2018-Aug-8")));

			} catch(domain_error &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}

			nowReplacements.clear(); // Leave other tests unaffected
		}
	};
}
