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

#ifndef H_CREDENTIALS_BASE
#define H_CREDENTIALS_BASE

#pragma warning ( push, 0 )

#include <string>

#pragma warning ( pop )

 // namespace trip planner - specifications
namespace tp { namespace specs {

  /**
  Interface for providing the user and password for a database.
  Allows selecting an implementation as secure as necessary.
  */
  struct IfCredentials /*abstract*/ {
	  virtual ~IfCredentials() /*= 0*/ {}

	  virtual const std::string& user() const = 0;
	  virtual const std::string& password() const = 0;
  };

}} // namespace tp::specs

#endif // H_CREDENTIALS_BASE
