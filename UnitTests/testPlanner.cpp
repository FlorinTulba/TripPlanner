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
#include "planner.h"
#include "jsonSource.h"
#include "customDateTimeProcessor.h"

#include <stdexcept>

#include <boost/date_time/gregorian/parsers.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::filesystem;
using namespace tp;
using namespace tp::var;
using namespace tp::specs;

namespace UnitTests {
	TEST_CLASS(Planner) {
    const ptime refMoment = ptime(from_simple_string("2017-Sep-16"s));

    /// Exposes protected methods from TripPlanner as public 
    class DerivedTripPlanner : public TripPlanner {
    public:
      DerivedTripPlanner(unique_ptr<InfoSource> infoSrc_) :
        TripPlanner(move(infoSrc_)) {}

      using TripPlanner::infoSrc;
      using TripPlanner::pickPlace;
    };

  public:
		TEST_METHOD(Planner_SearchPathBetweenSamePlaces_Throws) {
			Logger::WriteMessage(__FUNCTION__);

      // Make sure the next 100 configurations of UDYA consider that 'today' is 2017-Sep-16
      nowReplacements.resize(100ULL, refMoment);

      try {
				TripPlanner tp(make_unique<JsonSource>(
          path("../../UnitTests/TestFiles/specsOk.json")));
        
        // Both ends have the same name
        Assert::ExpectException<invalid_argument>( [&tp] {
          tp.search("abc"s, "abc"s, 100ULL);
        });
      
        // The trip ends are aliases of the same place
        Assert::ExpectException<invalid_argument>( [&tp] {
          tp.search("Приве́т नमस्ते שָׁלוֹם"s, "p8"s, 100ULL);
        });
      } catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}

      nowReplacements.clear(); // don't influence other tests
		}

		TEST_METHOD(Planner_SearchFor0Results_Throws) {
			Logger::WriteMessage(__FUNCTION__);

      // Make sure the next 100 configurations of UDYA consider that 'today' is 2017-Sep-16
      nowReplacements.resize(100ULL, refMoment);

      try {
				TripPlanner tp(make_unique<JsonSource>(
          path("../../UnitTests/TestFiles/specsOk.json")));
        Assert::ExpectException<invalid_argument>( [&tp] {
          tp.search("abc"s, "def"s, 0ULL);
        });
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}

      nowReplacements.clear(); // don't influence other tests
    }

    TEST_METHOD(Planner_SearchWhenBlockedDataAccess_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      // Make sure the next 100 configurations of UDYA consider that 'today' is 2017-Sep-16
      nowReplacements.resize(100ULL, refMoment);

      try {
        TripPlanner tp(make_unique<JsonSource>(
          path("../../UnitTests/TestFiles/specsOk.json")));

        tp.allowDataAccess(false);

        Assert::ExpectException<runtime_error>( [&tp] {
          tp.search("abc"s, "def"s, 1ULL);
        });

        tp.allowDataAccess(true);

      } catch(exception &e) {
        Logger::WriteMessage(e.what());
        Assert::Fail();
      }

      nowReplacements.clear(); // don't influence other tests
    }

    TEST_METHOD(Planner_PickPlaceIssues_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      ostringstream oss; // capturing output from pickPlace

      // Make sure the next 100 configurations of UDYA consider that 'today' is 2017-Sep-16
      nowReplacements.resize(100ULL, refMoment);

      try {
        DerivedTripPlanner dtp(make_unique<JsonSource>(
          path("../../UnitTests/TestFiles/specsOk.json")));

        // There's no "abc" place
        Assert::ExpectException<invalid_argument>( [&dtp, &oss] {
          dtp.pickPlace(u8"abc"s, cin, oss);
        });
        Assert::AreEqual(u8""s, oss.str());

        // There are 4 places named pp with id-s: 3,1,6,7 - sorted by description:
        // "", "descr1", "descr6", "descr7"
        const IfPlace
          &p3 = dtp.infoSrc->getPlace(3U),
          &p1 = dtp.infoSrc->getPlace(1U),
          &p6 = dtp.infoSrc->getPlace(6U),
          &p7 = dtp.infoSrc->getPlace(7U);

        // End of stream instead of specifying which place id to use
        Assert::ExpectException<runtime_error>( [&dtp, &oss] {
          dtp.pickPlace(u8"pp"s, istringstream(""), oss);
        });
        Assert::AreEqual(u8"There are several places with the name `pp`:\n"
                         "\t"s + p3.toString() + "\n"
                         "\t"s + p1.toString() + "\n"
                         "\t"s + p6.toString() + "\n"
                         "\t"s + p7.toString() + "\n"
                         "\n"
                         "Please enter the id of the desired place: "s,
                         oss.str());
        oss.str("");

        // Enter an empty line, another with whitespaces,
        // then enter 4 and 5 - which are not relevant,
        // then enter 'a' - invalid id value
        Assert::ExpectException<runtime_error>( [&dtp, &oss] {
          dtp.pickPlace(u8"pp"s,
                        istringstream("\n \t\n4\n5\na\n"),
                        oss);
        });
        Assert::AreEqual(u8"There are several places with the name `pp`:\n"
                         "\t"s + p3.toString() + "\n"
                         "\t"s + p1.toString() + "\n"
                         "\t"s + p6.toString() + "\n"
                         "\t"s + p7.toString() + "\n"
                         "\n"
                         "Please enter the id of the desired place: "
                         "\tEmpty line isn't a valid input. Retrying ...\n"
                         "Please enter the id of the desired place: "
                         "\t` \t` isn't a valid id. Retrying ...\n"
                         "Please enter the id of the desired place: "
                         "\tProvided id (4) isn't among the options. Retrying ...\n"
                         "Please enter the id of the desired place: "
                         "\tProvided id (5) isn't among the options. Retrying ...\n"
                         "Please enter the id of the desired place: "
                         "\t`a` isn't a valid id. Retrying ...\n"
                         "Please enter the id of the desired place: "s,
                         oss.str());
        oss.str("");

      } catch(exception &e) {
        Logger::WriteMessage(e.what());
        Assert::Fail();
      }

      nowReplacements.clear(); // don't influence other tests
    }

    TEST_METHOD(Planner_CorrectPickPlaceUse_ExpectedResults) {
      Logger::WriteMessage(__FUNCTION__);

      ostringstream oss; // capturing output from pickPlace

      // Make sure the next 100 configurations of UDYA consider that 'today' is 2017-Sep-16
      nowReplacements.resize(100ULL, refMoment);

      try {
        DerivedTripPlanner dtp(make_unique<JsonSource>(
          path("../../UnitTests/TestFiles/specsOk.json")));
        
        // There's a single place named 👨‍👨‍👧‍👧 with id 10
        Assert::AreEqual(10U,
                         dtp.pickPlace(u8"👨‍👨‍👧‍👧"s,
                                       istringstream(""),
                                       oss));
        Assert::AreEqual(u8""s, oss.str());

        // There are 4 places named pp with id-s: 3,1,6,7 - sorted by description:
        // "", "descr1", "descr6", "descr7"
        const IfPlace
          &p3 = dtp.infoSrc->getPlace(3U),
          &p1 = dtp.infoSrc->getPlace(1U),
          &p6 = dtp.infoSrc->getPlace(6U),
          &p7 = dtp.infoSrc->getPlace(7U);

        // Enter 5 - which is not relevant, then enter correct choice 6
        const unsigned chosenId =
          dtp.pickPlace(u8"pp"s,
                        istringstream("5\n6\n"),
                        oss);
        Assert::AreEqual(6U, chosenId);
        Assert::AreEqual(u8"There are several places with the name `pp`:\n"
                         "\t"s + p3.toString() + "\n"
                         "\t"s + p1.toString() + "\n"
                         "\t"s + p6.toString() + "\n"
                         "\t"s + p7.toString() + "\n"
                         "\n"
                         "Please enter the id of the desired place: "
                         "\tProvided id (5) isn't among the options. Retrying ...\n"
                         "Please enter the id of the desired place: "s,
                         oss.str());

      } catch(exception &e) {
        Logger::WriteMessage(e.what());
        Assert::Fail();
      }

      nowReplacements.clear(); // don't influence other tests
    }
  };
}