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

#include "credentialsProvider.h"

#pragma warning ( push, 0 )

#include <fstream>

#pragma warning ( pop )

using namespace std;
using namespace boost::archive;

static void promptStreamExc() {
	static const char err[] = "Fatal - Invalid prompt stream status / data!";
  cerr<<err<<endl;
	throw runtime_error(err);
}

// namespace trip planner - specifications
namespace tp { namespace specs {

  CredentialsProvider::CredentialsProvider(const string &credentialsFile,
                                           istream &promptStream/* = cin*/) {
	  ifstream ifs(credentialsFile, ios::binary);
	  if(ifs) { // read the credentials from the file
		  try {
			  binary_iarchive ia(ifs);
			  ia>>*this;
			  return;
		  } catch(exception &e) {
			  cerr<<"Error - Detected problem while processing the credentials file `"
				  <<credentialsFile<<"`: "<<e.what()<<endl;
		  }
	  } 
	
	  // Prompt the user for the credentials
	  cout<<"Please provide the credentials for using the database of this project!"
      <<endl;
	  for(;;) {
		  cout<<"User=";
		  if(!getline(promptStream, _user))
			  promptStreamExc();
		
		  if(!_user.empty())
			  break;
		  cerr<<"Error - The username cannot be empty!"<<endl;
	  }
	  cout<<"Password=";
	  if(!getline(promptStream, _password))
		  promptStreamExc();

	  // Try writing the provided credentials to the file
	  try {
		  ofstream ofs(credentialsFile, ios::binary);
		  binary_oarchive oa(ofs);
		  oa<<*this;
	  } catch(exception &e) {
		  cout<<"Warning - Detected problem while writing the credentials file `"
			  <<credentialsFile<<"`: "<<e.what()<<endl;
	  }
  }

  const string& CredentialsProvider::user() const {
	  return _user;
  }

  const string& CredentialsProvider::password() const {
	  return _password;
  }

}} // namespace tp::specs
