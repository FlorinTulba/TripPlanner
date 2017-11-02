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
#include "constraints.h"

#include <stdexcept>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::posix_time;
using namespace tp::var;
using namespace tp::queries;

namespace UnitTests {
	TEST_CLASS(TimeConstraints) {
		static time_duration years(int y) {
			static const time_duration oneYear(366 * 24, 0, 0);
			return oneYear * y;
		}

	public:
		TEST_METHOD(TimeConstraints_InvalidCtorParams_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			// First interval starts after the start of the second (default) interval
			Assert::ExpectException<invalid_argument>([] {
				const auto m = nowUTC() + years(2);
				::TimeConstraints tc(time_period(m, m));
			});

			// First interval ends after the end of the second interval
			Assert::ExpectException<invalid_argument>([] {
				const auto now = nowUTC();
				::TimeConstraints tc(time_period(now, years(2)),
									 time_period(now, years(1)));
			});
		}

		TEST_METHOD(TimeConstraints_correctCtorParams_DoesntThrow) {
			Logger::WriteMessage(__FUNCTION__);

			try {
				const auto now = nowUTC();
				time_period p1(now, seconds(1)), 
					p2(now+hours(2), now+hours(3));
				auto p3 = time_period(nowUTC(), years(1));
				::TimeConstraints
					tc1,
					tc2(time_period(nowUTC(), years(1))),
					tc3(move(p3)),
					tc4(p1, time_period(nowUTC(), years(1))),
					tc5(p1, p2);
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}
	};
}
