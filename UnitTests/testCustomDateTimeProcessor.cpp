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
#include "customDateTimeProcessor.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/parsers.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace tp::var;

namespace UnitTests {
	TEST_CLASS(DurationAndTimeFormatter) {
	public:
		TEST_METHOD(DurationFormatter_VariousDurations_ExpectedFormatting) {
			Logger::WriteMessage(__FUNCTION__);

			Assert::IsTrue(0 == formatDuration(minutes(0)).
						   compare("0 minutes"));
			Assert::IsTrue(0 == formatDuration(minutes(1)).
						   compare("1 minute"));
			Assert::IsTrue(0 == formatDuration(hours(1)).
						   compare("1 hour and 0 minutes"));
			Assert::IsTrue(0 == formatDuration(hours(1)+minutes(1)).
						   compare("1 hour and 1 minute"));
			Assert::IsTrue(0 == formatDuration(hours(2)).
						   compare("2 hours and 0 minutes"));
			Assert::IsTrue(0 == formatDuration(hours(23)+minutes(59)).
						   compare("23 hours and 59 minutes"));
			Assert::IsTrue(0 == formatDuration(hours(24)).
						   compare("1 day, 0 hours and 0 minutes"));
			Assert::IsTrue(0 == formatDuration(hours(48)-minutes(1)).
						   compare("1 day, 23 hours and 59 minutes"));
			Assert::IsTrue(0 == formatDuration(hours(48)).
						   compare("2 days, 0 hours and 0 minutes"));
			Assert::IsTrue(0 == formatDuration(hours(49)+minutes(1)).
						   compare("2 days, 1 hour and 1 minute"));
		}

		TEST_METHOD(TimeFormatter_VariousTimes_ExpectedFormatting) {
			Logger::WriteMessage(__FUNCTION__);

			Assert::IsTrue(0 == formatTimePoint(
				ptime(from_simple_string("2017-Oct-17"s), duration_from_string("7:4"s))).
				compare("Tue Oct-17-2017 07:04"));
			Assert::IsTrue(0 == formatTimePoint(
				ptime(from_simple_string("2017-Jan-25"s), duration_from_string("22:50"s))).
				compare("Wed Jan-25-2017 22:50"));
		}
	};
}
