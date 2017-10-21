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
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace UnitTests {
	TEST_CLASS(JsonSource) {
	public:
		TEST_METHOD(JsonSource_CorrectFile_NormalResults) {
			Logger::WriteMessage(__FUNCTION__);

			try {
				::JsonSource js("../../UnitTests/TestFiles/specsOk.json");

				const auto &p4 = js.getPlace(4);
				Assert::AreEqual("p4", p4.name().c_str());

				const auto &p8 = js.getPlace(UniquePlace("p8"));
				Assert::AreEqual(8U, p8.id());

				Assert::ExpectException<invalid_argument>([&js] {
					js.getPlace(16U); // No such place
				});

				Assert::ExpectException<invalid_argument>([&js] {
					js.getPlace(UniquePlace("p16")); // No such place
				});

				Assert::ExpectException<invalid_argument>([&js] {
					set<unsigned> bundles;
					js.bundlesForPlace(16U, bundles); // No such place
				});

				Assert::ExpectException<domain_error>([&js] {
					set<unsigned> bundles;
					js.bundlesForPlace(0U, bundles); // No bundle covers this place
				});

				set<unsigned> bundles, expected { 1U, 2U, 3U };
				js.bundlesForPlace(1U, bundles);
				Assert::IsTrue(equal(CBOUNDS(bundles), cbegin(expected)));

				js.bundlesForPlace(2U, bundles);
				expected = { 1U };
				Assert::IsTrue(equal(CBOUNDS(bundles), cbegin(expected)));

				js.bundlesForPlace(13U, bundles);
				expected = { 2U, 4U };
				Assert::IsTrue(equal(CBOUNDS(bundles), cbegin(expected)));

				js.bundlesForPlaces({ 1U, 13U }, bundles);
				expected = { 1U, 2U, 3U, 4U };
				Assert::IsTrue(equal(CBOUNDS(bundles), cbegin(expected)));

				js.bundlesForPlaces({ 1U, 2U }, bundles);
				expected = { 1U, 2U, 3U };
				Assert::IsTrue(equal(CBOUNDS(bundles), cbegin(expected)));

				js.bundlesForPlaces({ 2U, 13U }, bundles);
				expected = { 1U, 2U, 4U };
				Assert::IsTrue(equal(CBOUNDS(bundles), cbegin(expected)));

				Assert::ExpectException<domain_error>([&js] {
					js.getRoutesBundle(0U); // No such bundle
				});
				
				const auto &b1 = js.getRoutesBundle(1U);
				Assert::AreEqual(1U, b1.id());
				Assert::AreEqual(size_t(TranspModes::ROAD), b1.transpMode());
				Assert::AreEqual(0b0111110UL, b1.operationalDaysOfWeek()->to_ulong());
				Assert::AreEqual(8ULL, b1.unavailDaysForTheYearAhead()->size());
				Assert::AreEqual(1.f, // ln(1+3.5*((e-1)/3.5)) == 1
								 b1.pricingEngine().normalFare(expm1f(1.f)/3.5f),
								 1e-3f);
				Assert::AreEqual(6ULL, b1.stopsCount());
				Assert::IsFalse(b1.containsStop(0U));
				Assert::IsFalse(b1.containsStop(7U));
				
				vector<float> expectedDistances { 90.5f, 70.2f, 93.4f, 115.9f, 208.6f };
				const vector<float> &returnedDistances = b1.distances();
				Assert::IsTrue(equal(CBOUNDS(expectedDistances),
									 CBOUNDS(returnedDistances), []
									 (float f1, float f2) {
					return fabs(f1 - f2) < 1e-3f;
				}));

				for(unsigned placeId = 1U; placeId <= 5U; ++placeId) {
					Assert::IsTrue(b1.containsStop(placeId));
					Assert::AreEqual(placeId, b1.nthStop(placeId - 1U, false));
					Assert::AreEqual(7U - placeId, b1.nthStop(placeId - 1U, true));
					Assert::AreEqual(expectedDistances[placeId - 1U],
									 b1.nthDistance(placeId - 1U, false),
									 1e-3f);
					Assert::AreEqual(expectedDistances[5U - placeId],
									 b1.nthDistance(placeId - 1U, true),
									 1e-3f);
				}
				
				Assert::IsTrue(b1.containsStop(6U));
				Assert::AreEqual(6U, b1.nthStop(5U, false));
				Assert::AreEqual(1U, b1.nthStop(5U, true));

				Assert::ExpectException<out_of_range>([&b1] {
					b1.nthStop(6U, true); // max index is 5
				});
				Assert::ExpectException<out_of_range>([&b1] {
					b1.nthStop(6U, false); // max index is 5
				});
				Assert::ExpectException<out_of_range>([&b1] {
					b1.nthDistance(5U, true); // max index is 4
				});
				Assert::ExpectException<out_of_range>([&b1] {
					b1.nthDistance(5U, false); // max index is 4
				});

				vector<unsigned> expectedStops { 1U, 2U, 3U, 4U, 5U, 6U };
				const vector<unsigned> &returnedStops = b1.traversedStops();
				Assert::IsTrue(equal(CBOUNDS(expectedStops),
									 CBOUNDS(returnedStops)));

				set<unsigned> expectedRAs { 0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U };
				const set<unsigned> &returnedRAs = b1.alternatives();
				Assert::IsTrue(equal(CBOUNDS(expectedRAs),
									 CBOUNDS(returnedRAs)));

				Assert::ExpectException<domain_error>([&js] {
					js.getRouteAlternative(50U); // Not that many routes
				});

				IRouteAlternative &ra5 = js.getRouteAlternative(5U);
				Assert::AreEqual(5U, ra5.id());
				Assert::AreEqual(1U, ra5.bundleId());
				Assert::IsTrue(ra5.returnTrip());
				Assert::AreEqual(0b0111110UL, ra5.operationalDaysOfWeek()->to_ulong());
				Assert::AreEqual(8ULL, ra5.unavailDaysForTheYearAhead()->size());
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
		}

		TEST_METHOD(JsonSource_TabsInStrings_InvalidCodeSequence) {
			Logger::WriteMessage(__FUNCTION__);

			Assert::ExpectException<exception>([] {
				// Strings containing TABs
				istringstream iss(R"({"root":"strings	with	tabs	are	not ok!"})");
				::JsonSource js(move(iss));
			});
		}

		TEST_METHOD(JsonSource_NotUniquePlaceTraits_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			Assert::ExpectException<domain_error>([] {
				// Duplicate place id-s
				istringstream iss(R"({"Scenario": { "Places" : [
	{"id":1, "name":"p1"},
	{"id":2, "name":"p2"},
	{"id":1, "name":"p3"}
]}})");
				::JsonSource js(move(iss));
			});

			Assert::ExpectException<domain_error>([] {
				// Duplicate place names
				istringstream iss(R"({"Scenario": { "Places" : [
	{"id":1, "name":"p1"},
	{"id":2, "name":"p2"},
	{"id":3, "name":"p2"}
]}})");
				::JsonSource js(move(iss));
			});
		}

		TEST_METHOD(JsonSource_LessThan2Places_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			// No section for places at all
			Assert::ExpectException<exception>([] {
				istringstream iss(R"({"Scenario": {}})");
				::JsonSource js(move(iss));
			});

			// No place defined
			Assert::ExpectException<domain_error>([] {
				istringstream iss(R"({"Scenario": { "Places" : [
]}})");
				::JsonSource js(move(iss));
			});

			// Single place defined
			Assert::ExpectException<domain_error>([] {
				istringstream iss(R"({"Scenario": { "Places" : [
	{"id":1, "name":"p1"}
]}})");
				::JsonSource js(move(iss));
			});

			// For >= 2 places shouldn't throw
			// domain_error("There must be at least 2 places!")
			try {
				istringstream iss(R"({"Scenario": { "Places" : [
	{"id":1, "name":"p1"},
	{"id":2, "name":"p2"}
]}})");
				::JsonSource js(move(iss));

			} catch(exception &e) {
				// The stream is providing only the part concerning the places and
				// it should throw some exception about missing the rest of the information
				Assert::AreNotEqual("There must be at least 2 places!", e.what());
			}
		}

		TEST_METHOD(JsonSource_FoundDuplicateBundleId_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			// There are bundle with the same id
			Assert::ExpectException<domain_error>([] {
				istringstream iss(R"({"Scenario": { "Places" : [
	{"id":1, "name":"p1"},
	{"id":2, "name":"p2"}],
"Routes": [
	{"bundleId":1, "TM" : "Road", "EF" : 3.5,
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":6, "dist" : 208.6}]},
		"Alternatives" : []},

	{"bundleId":1, "TM" : "Rail", "EF" : 4,
		"Route" :
			{"StartPlaceId":1, "Links": [
			{"NextPlaceId":13, "dist" : 30.3}]},
		"Alternatives" : []}
]}})");
				::JsonSource js(move(iss));
			});
		}
	};
}
