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
#include "../pricing.h"

#include <stdexcept>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTests {
	TEST_CLASS(TicketPriceCalculator) {
	public:
		TEST_METHOD(TicketPriceCalculator_InvalidCtorParams_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			// Invalid kEconomy
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(-1.f); });
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(0.f); });

			// Invalid kBusiness
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(0.01f, -0.01f); });
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(0.01f, 0.001f); });

			// Invalid lowFareFactor
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(3.4f, 6.5f, -0.01f); });
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(3.4f, 6.5f, 0.f); });
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(3.4f, 6.5f, 1.001f); });

			// Invalid lowFareFactor
			Assert::ExpectException<invalid_argument>(
				[] {::TicketPriceCalculator tpc(3.4f, 6.5f, 0.7f, 0.99f); });
		}

		TEST_METHOD(TicketPriceCalculator_CorectCtorParams_DoesntThrow) {
			Logger::WriteMessage(__FUNCTION__);
			try {
				{::TicketPriceCalculator tpc(0.01f);}
				{::TicketPriceCalculator tpc(3.4f, 3.4001f);}
				{::TicketPriceCalculator tpc(3.4f, 6.5f, 1.f, 1.001f);}
			} catch(invalid_argument &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}

		TEST_METHOD(TicketPriceCalculator_invalidParamFareQueries_Throws) {
			Logger::WriteMessage(__FUNCTION__);
			try {
				::TicketPriceCalculator tpc(1.f);

				// Invalid tripDistance
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.normalFare(-0.01f); });
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.normalFare(0.f, false); });
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.airplaneFare(-0.01f, 0.5f, 0.5f, false); });
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.airplaneFare(0.f, 0.5f, 0.5f); });

				// Invalid urgency / occupancy
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.airplaneFare(300.4f, -0.001f, 0.5f); });
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.airplaneFare(300.4f, 1.001f, 0.5f, false); });
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.airplaneFare(300.4f, 0.5f, -0.001f); });
				Assert::ExpectException<invalid_argument>(
					[&tpc] {tpc.airplaneFare(300.4f, 0.5f, 1.001f, false); });
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}

		TEST_METHOD(TicketPriceCalculator_correctFareQueries_ExpectedPrices) {
			Logger::WriteMessage(__FUNCTION__);
			const float lowFareFactor = 0.75f,
				highFareFactor = 2.f;
			try {
				::TicketPriceCalculator tpc(1.f, // kEconomy
											0.f, // no business class
											lowFareFactor,
											highFareFactor);
				
				// fares for business class tickets when there is no business class results in a 0 price
				Assert::AreEqual(0.f, tpc.normalFare(1000000.f, false), 1e-3f);
				Assert::AreEqual(0.f, tpc.airplaneFare(1000000.f, 0.f, 0.f, false), 1e-3f);
				Assert::AreEqual(0.f, tpc.airplaneFare(1000000.f, 0.5f, 0.5f, false), 1e-3f);
				Assert::AreEqual(0.f, tpc.airplaneFare(1000000.f, 1.f, 1.f, false), 1e-3f);

				// normal fare for k=1 and a distance of (e-1) is ln(1+1*(e-1)) = ln(e) = 1
				Assert::AreEqual(1.f, tpc.normalFare(expm1f(1.f)), 1e-3f);

				// When the urgency and occupancy are both 0, the price should be normalFare * lowFareFactor
				Assert::AreEqual(lowFareFactor, tpc.airplaneFare(expm1f(1.f), 0.f, 0.f), 1e-3f);

				// When the urgency or occupancy is 1, the price should be normalFare * highFareFactor
				Assert::AreEqual(highFareFactor, tpc.airplaneFare(expm1f(1.f), 0.f, 1.f), 1e-3f);
				Assert::AreEqual(highFareFactor, tpc.airplaneFare(expm1f(1.f), 1.f, 0.f), 1e-3f);

			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}
	};
}
