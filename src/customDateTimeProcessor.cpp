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

#include "customDateTimeProcessor.h"
#include "util.h"

#pragma warning ( push, 0 )

#include <sstream>
#include <iomanip>
#include <cassert>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/parsers.hpp>

#pragma warning ( pop )

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

// namespace trip planner - various
namespace tp { inline namespace var {

#if (defined(_MSC_VER) || defined(__clang__)) && !defined(__GNUC__)
  template vector<ptime>; // forced template instantiation
#endif // (_MSC_VER || __clang__) && !__GNUC__
  vector<ptime> nowReplacements;

  ptime nowUTC() {
    while(!nowReplacements.empty()) {
      ptime result = nowReplacements.back();
      nowReplacements.pop_back();
      return result; // leaves the rest for next calls to nowUTC()
    }

    // If nowReplacements was/gets empty, return the actual current moment
    return second_clock::universal_time();
  }

  string formatTimePoint(const ptime &mom) {
    const auto timePart = mom.time_of_day();
    const auto datePart = mom.date();
    const auto ymd = datePart.year_month_day();
    ostringstream oss;
    oss<<datePart.day_of_week().as_short_string()<<' '
      <<ymd.month.as_short_string()<<'-'
      <<ymd.day<<'-'
      <<ymd.year<<' '
      <<setw(2)<<setfill('0')<<timePart.hours()<<':'
      <<setw(2)<<setfill('0')<<timePart.minutes();
    return oss.str();
  }

  string formatDuration(const time_duration& dur) {
    const auto asHours = dur.hours();
    const div_t days_hours = div(asHours, 24);
    const auto mins = dur.minutes();

    ostringstream oss;
    bool daysPartPresent = false;
    if(days_hours.quot > 0) {
      daysPartPresent = true;
      oss<<days_hours.quot<<" day";
      if(days_hours.quot > 1)
        oss<<'s';
      oss<<", ";
    }
    if(days_hours.rem > 0 || daysPartPresent) {
      oss<<days_hours.rem<<" hour";
      if(days_hours.rem != 1)
        oss<<'s';
      oss<<" and ";
    }
    oss<<mins<<" minute";
    if(mins != 1)
      oss<<'s';

    return oss.str();
  }

  void updateUnavailDaysForTheYearAhead(const string &udyaStr,
                                        set<date>& udyaSet) {
    // Dates delimited by '|' among 0 or more space-like symbols
    const vector<string> providedDays = tokenize(udyaStr, R"(\s*\|\s*)");

    const date today = nowUTC().date();
    const greg_year_month_day todayYMD = today.year_month_day();
    const date startOfCurrentMonth =
      today - days(todayYMD.day.as_number());

    ostringstream oss;
    oss<<todayYMD.year<<"-";
    const string thisYearPrefix(oss.str());
    oss.str("");

    udyaSet.clear();
    for(const string &mentionedDay : providedDays) {
      oss<<thisYearPrefix<<mentionedDay;
      date unavailDay(from_simple_string(oss.str()));
      oss.str("");
      bool relevantDate = true;
      if(unavailDay < today) {
        // all dates before the start of the month are projected 1 year ahead
        if(unavailDay < startOfCurrentMonth)
          unavailDay += years(1);
        else // dates between today and the start of the month can be ignored (they remain in the past)
          relevantDate = false;
      }
      if(relevantDate)
        udyaSet.insert(unavailDay);
    }
  }

}} // namespace tp::var
