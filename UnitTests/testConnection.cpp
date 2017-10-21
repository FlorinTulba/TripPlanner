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
#include "connection.h"
#include "customDateTimeProcessor.h"
#include "transpModes.h"

#include <stdexcept>
#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::posix_time;

namespace UnitTests {
	TEST_CLASS(Connection) {
	public:
		TEST_METHOD(Connection_InvalidCtorParams_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			const ptime now = nowUTC(),
				after2Hours = now + hours(2);

			// Identical from and to
			Assert::ExpectException<invalid_argument>([&] {
				::Connection c(7ULL, 7ULL, time_period(now, after2Hours),
							   TranspModes::AIR, 123.f, 321.f);
			});

			// Empty interval
			Assert::ExpectException<invalid_argument>([&] {
				::Connection c(7ULL, 8ULL, time_period(now, now-time_duration::unit()),
							   TranspModes::RAIL, 123.f, 321.f);
			});

			// Invalid transportation mode
			Assert::ExpectException<invalid_argument>([&] {
				::Connection c(7ULL, 8ULL, time_period(now, after2Hours),
							   TranspModes::RAIL | TranspModes::ROAD,
							   123.f, 321.f);
			});

			// Negative or 0 price
			Assert::ExpectException<invalid_argument>([&] {
				::Connection c(7ULL, 9ULL, time_period(now, after2Hours),
							   TranspModes::ROAD, -1.f, 321.f);
			});
			Assert::ExpectException<invalid_argument>([&] {
				::Connection c(7ULL, 9ULL, time_period(now, after2Hours),
							   TranspModes::WATER, 0.f, 321.f);
			});

			// Negative or 0 distance
			Assert::ExpectException<invalid_argument>([&] {
				::Connection c(7ULL, 9ULL, time_period(now, after2Hours),
							   TranspModes::AIR, 123.f, -1.f);
			});
			Assert::ExpectException<invalid_argument>([&] {
				::Connection c(7ULL, 9ULL, time_period(now, after2Hours),
							   TranspModes::RAIL, 123.f, 0.f);
			});
		}

		TEST_METHOD(Connection_correctCtorParams_ExpectedDuration) {
			Logger::WriteMessage(__FUNCTION__);

			const ptime now = nowUTC(),
				after2Hours = now + hours(2);

			try {
				::Connection c(7ULL, 9ULL, time_period(now, after2Hours),
							   TranspModes::WATER, 123.f, 321.f);
				Assert::AreEqual(2, c.duration().hours());
				Assert::AreEqual((size_t)TranspModes::WATER, c.transpModes());
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}
	};
}
