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

#include "planner.h"
#include "jsonSource.h"

using namespace std;
using namespace boost::filesystem;
using namespace tp;
using namespace tp::specs;
using namespace tp::queries;

int main(/*int argc, const char* argv[]*/) {
  /*
  cout<<"Initial locale: "<<setlocale(LC_ALL, nullptr)<<endl;
  cout<<"Setting locale: "<<setlocale(LC_ALL, "utf-8")<<endl;
  cin.imbue(locale()); cout.imbue(locale());
  */
  TripPlanner tp(make_unique<JsonSource>(path("input.json")));
  string from, to;
  for(;;) {
    try {
      cout<<"Enter the name of the departure place: ";
      if(!getline(cin, from))break;
      cout<<"Enter the destination name: ";
      if(!getline(cin, to))break;
      tp.search(from, to, 4ULL);
    } catch(exception &e) {
      cerr<<e.what()<<endl;
    }
  }
  /*
  const unique_ptr<IResults> results = tp.search("pp", "p13", 4ULL);
  if(nullptr == results)
  cout<<"There were no results!"<<endl;
  else
  cout<<*results<<endl;
  */
}
