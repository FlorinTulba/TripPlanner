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
#include "credentialsProvider.h"
#include "boost/filesystem/operations.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace boost::filesystem;

namespace UnitTests {
	TEST_CLASS(CredentialsProvider) {
	public:
		TEST_METHOD(CredentialsProvider_InvalidFilePathAndPrompt_Throws) {
			Logger::WriteMessage(__FUNCTION__);

			Assert::ExpectException<runtime_error>([] {
				istringstream iss("\n\n");
        tp::specs::CredentialsProvider cp("", iss);
			});

			Assert::ExpectException<runtime_error>([] {
				istringstream iss("testUser\n");
        tp::specs::CredentialsProvider cp("", iss);
			});

			Assert::ExpectException<runtime_error>([] {
				istringstream iss("testUser");
        tp::specs::CredentialsProvider cp("", iss);
			});
		}

		TEST_METHOD(CredentialsProvider_InvalidFilePath_PromptAndWarning) {
			Logger::WriteMessage(__FUNCTION__);

			istringstream iss("testUser\ntestPassword\n");
			try {
        tp::specs::CredentialsProvider cp("", iss);
				Assert::AreEqual("testUser", cp.user().c_str());
				Assert::AreEqual("testPassword", cp.password().c_str());
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}

		TEST_METHOD(CredentialsProvider_ValidFile_CorrectData) {
			Logger::WriteMessage(__FUNCTION__);
			// The file provides testUser and testPassword as values

			istringstream iss("wrong\nwrong\n"); // prepare a stream with wrong values
			try {
        tp::specs::CredentialsProvider cp("../../UnitTests/TestFiles/credentialsOk.bin", iss);
				Assert::AreEqual("testUser", cp.user().c_str());
				Assert::AreEqual("testPassword", cp.password().c_str());
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}

		TEST_METHOD(CredentialsProvider_InvalidReadOnlyFile_CorrectDataAndFileUnchanged) {
			Logger::WriteMessage(__FUNCTION__);

			// The '\n' from the start forces prompting multiple times for the username
			istringstream iss("\n\n\ntestUser\ntestPassword\n");
			try {
        tp::specs::CredentialsProvider cp("../../UnitTests/TestFiles/credentialsNotOk.bin", iss);
				Assert::AreEqual("testUser", cp.user().c_str());
				Assert::AreEqual("testPassword", cp.password().c_str());
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}
		}

		TEST_METHOD(CredentialsProvider_InvalidFile_CorrectDataFileReplaced) {
			Logger::WriteMessage(__FUNCTION__);

			// clone a read-only file that was not ok; make sure the clone is writeable
			const path original("../../UnitTests/TestFiles/credentialsNotOk.bin"),
				clone("../../UnitTests/TestFiles/credentialsNotOkClone.bin"),
				correct("../../UnitTests/TestFiles/credentialsOk.bin");
			copy_file(original, clone, copy_option::overwrite_if_exists);
			permissions(clone, add_perms|owner_write|group_write|others_write);

			// Verify that 'credentialsNotOkClone.bin' and 'credentialsOk.bin' have different sizes
			Assert::AreNotEqual(file_size(correct), file_size(clone));

			istringstream iss("testUser\ntestPassword\n");
			try {
        tp::specs::CredentialsProvider cp("../../UnitTests/TestFiles/credentialsNotOkClone.bin", iss);
				Assert::AreEqual("testUser", cp.user().c_str());
				Assert::AreEqual("testPassword", cp.password().c_str());
			} catch(exception &e) {
				Logger::WriteMessage(e.what());
				Assert::Fail();
			}

			// Verify that 'credentialsNotOkClone.bin' was updated
			// and has same size as 'credentialsOk.bin'
			Assert::AreEqual(file_size(correct), file_size(clone));

			// Erase the cloned file
			Assert::IsTrue(remove(clone));
		}
	};
}
