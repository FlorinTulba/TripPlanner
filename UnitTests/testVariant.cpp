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
#include "variant.h"
#include "connection.h"
#include "customDateTimeProcessor.h"
#include "transpModes.h"

#include <stdexcept>

#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::posix_time;
using namespace tp::var;
using namespace tp::specs;
using namespace tp::queries;

namespace UnitTests {
	TEST_CLASS(Variant) {
		const ptime
			now = nowUTC(),
			after2Hours = now + hours(2),
			after3Hours = now + hours(3),
			after4Hours = now + hours(4),
			after7Hours = now + hours(7),
			after9Hours = now + hours(9);

	public:
		TEST_METHOD(Variant_InvalidNewConnection_Throws) {
			Logger::WriteMessage(__FUNCTION__);

      tp::queries::Variant v;

			// appending null connection
			Assert::ExpectException<invalid_argument>([&v] {
				v.appendConnection(nullptr);
			});

			try {
				v.appendConnection(make_unique<Connection>(0ULL, 1ULL,
														   time_period(now,
																			   after3Hours),
														   TranspModes::AIR,
														   123.f, 321.f));
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}

			// The new connection starts at place index 2 instead of 1, as it should
			Assert::ExpectException<invalid_argument>([&v, this] {
				v.appendConnection(make_unique<Connection>(2ULL, 3ULL,
														   time_period(after4Hours,
																			   after7Hours),
														   TranspModes::AIR,
														   123.f, 321.f));
			});

			// The new connection is missed, since the traveler
			// reaches the common location too late:
			// after3Hours, while the new connection leaves after2Hours
			Assert::ExpectException<invalid_argument>([&v, this] {
				v.appendConnection(make_unique<Connection>(1ULL, 2ULL,
														   time_period(after2Hours,
																			   after7Hours),
														   TranspModes::AIR,
														   123.f, 321.f));
			});
		}

		TEST_METHOD(Variant_SeveralNewConnections_Throws) {
			Logger::WriteMessage(__FUNCTION__);

      tp::queries::Variant v;

			try {
				v.appendConnection(make_unique<Connection>(0ULL, 1ULL,
														   time_period(now,
																			   after2Hours),
														   TranspModes::AIR,
														   123.f, 321.f));
				v.appendConnection(make_unique<Connection>(1ULL, 2ULL,
														   time_period(after3Hours,
																			   after4Hours),
														   TranspModes::RAIL,
														   123.f, 321.f));
				v.appendConnection(make_unique<Connection>(2ULL, 3ULL,
														   time_period(after7Hours,
																			   after9Hours),
														   TranspModes::WATER,
														   123.f, 321.f));
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}

			Assert::AreEqual(0ULL, v.from());
			Assert::AreEqual(3ULL, v.to());
			Assert::IsTrue(now == v.begin());
			Assert::IsTrue(after9Hours == v.end());
			Assert::AreEqual(9, v.duration().hours());
			Assert::AreEqual(3.f * 123.f, v.price(), 1e-3f);
			Assert::AreEqual(3.f * 321.f, v.distance(), 1e-3f);
			Assert::AreEqual(
				size_t(TranspModes::AIR | TranspModes::RAIL | TranspModes::WATER),
				v.transpModes());
		}
	};
}
