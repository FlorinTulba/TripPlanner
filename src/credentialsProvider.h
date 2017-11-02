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

#ifndef H_CREDENTIALS_PROVIDER
#define H_CREDENTIALS_PROVIDER

#include "credentialsBase.h"
#include "util.h"

#pragma warning ( push, 0 )

#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wexpansion-to-defined"
#include <boost/archive/binary_iarchive.hpp>
#pragma clang diagnostic pop
#include <boost/archive/binary_oarchive.hpp>

#pragma warning ( pop )

// namespace trip planner - specifications
namespace tp { namespace specs {

  /**
  Realization of IfCredentials.
  It checks for a certain file containing the credentials.

  If the file doesn`t exist, it prompts the user for those credentials.
  In this case, it attempts to write the provided values to a file.
  When the file cannot be created, it just issues a warning about that.

  Although the output is a binary file, the credentials appear in clear,
  so it`s not a secure solution.
  */
  class CredentialsProvider : public IfCredentials {
  protected:
	  std::string _user;
	  std::string _password;

  public:
	  /**
	  Using promptStream is helpful for Unit testing
	  */
	  CredentialsProvider(const std::string &credentialsFile,
                        std::istream &promptStream = std::cin);

	  const std::string& user() const override;
	  const std::string& password() const override;

	  template<class Archive>
	  void serialize(Archive &ar, const unsigned version) {
		  UNREFERENCED(version);
		  ar & _user & _password;
	  }
  };

}} // namespace tp::specs

#endif // H_CREDENTIALS_PROVIDER
