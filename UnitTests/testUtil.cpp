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
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTests {
	TEST_CLASS(TrimAndTokenizerTests) {
		static inline bool compareStr(const string &s1,
							   const string &s2) {
			return s1.compare(s2) == 0;
		}

		static inline bool compareStrVectors(const vector<string> &v1,
											 const vector<string> &v2) {
			return v1.size() == v2.size() &&
				equal(CBOUNDS(v1), cbegin(v2), compareStr);
		}

	public:
		TEST_METHOD(TokenizerTests_VariousInput_ExpectedOutput) {
			Logger::WriteMessage(__FUNCTION__);

			// Chars separated by 1 or more space-like symbols
			string inp = "a \tb\t \tc\t\t d   \t e f";
			vector<string> expect = {"a", "b", "c", "d", "e", "f"},
				produced = tokenize(inp);
			Assert::IsTrue(compareStrVectors(expect, produced));

			// Chars separated by a '-' among 0 or more space-like symbols
			inp = "a \t-   \t b-c -\t\td-e-f";
			produced = tokenize(inp, R"(\s*-\s*)");
			Assert::IsTrue(compareStrVectors(expect, produced));

			// Chars separated by a '|' among 0 or more space-like symbols
			inp = "a \t|   \t b|c |\t\td|e|f"; produced = tokenize(inp, R"(\s*\|\s*)");
			Assert::IsTrue(compareStrVectors(expect, produced));

			// The same input - looking for '-' when there are none
			produced = tokenize(inp, "-"); expect = {inp};
			Assert::IsTrue(compareStrVectors(expect, produced));

			// Empty input
			inp = ""; produced = tokenize(inp); expect.clear();
			Assert::IsTrue(compareStrVectors(expect, produced));
		}

    TEST_METHOD(TrimTests_VariousInput_ExpectedOutput) {
      Logger::WriteMessage(__FUNCTION__);

      struct ExpectedForInput {
        string input;
        string trimOut, ltrimOut, rtrimOut;
      };

      const vector<ExpectedForInput> cases {
        {"", "", "", ""},

        {"   ", "", "", ""},

        {"   \n\t ", "", "", ""},

        {"   \n\t a b c", "a b c",
        "a b c", "   \n\t a b c"},

        {"a b c   \n\t ", "a b c",
        "a b c   \n\t ", "a b c"},

        {"   \n\t a b c   \n\t ", "a b c",
        "a b c   \n\t ", "   \n\t a b c"},
        
        {u8"   \n\t ∃y ∀x ¬(x ≺ y)   \n\t ", u8"∃y ∀x ¬(x ≺ y)",
        u8"∃y ∀x ¬(x ≺ y)   \n\t ", u8"   \n\t ∃y ∀x ¬(x ≺ y)"},
        
        {u8"   \n\t 吴兆㈲㜸㐰   \n\t ", u8"吴兆㈲㜸㐰",
        u8"吴兆㈲㜸㐰   \n\t ", u8"   \n\t 吴兆㈲㜸㐰"},
        
        {u8"   \n\t  Tîrşolţ  \n\t ", u8"Tîrşolţ",
        u8"Tîrşolţ  \n\t ", u8"   \n\t  Tîrşolţ"},
        
        {u8"   \n\t  Temesvár  \n\t ", u8"Temesvár",
        u8"Temesvár  \n\t ", u8"   \n\t  Temesvár"},
        
        {u8"   \n\t  🐨  \n\t ", u8"🐨",
        u8"🐨  \n\t ", u8"   \n\t  🐨"},
        
        {u8"   \n\t  👨‍👨‍👧‍👧  \n\t ", u8"👨‍👨‍👧‍👧",
        u8"👨‍👨‍👧‍👧  \n\t ", u8"   \n\t  👨‍👨‍👧‍👧"},
        
        {u8"   \n\t ю́   \n\t ", u8"ю́",
        u8"ю́   \n\t ", u8"   \n\t ю́"},
        
        {u8"   \n\t  Ω  \n\t ", u8"Ω",
        u8"Ω  \n\t ", u8"   \n\t  Ω"},
        
        {u8"   \n\t Приве́т नमस שָׁלוֹםे שָׁ   \n\t ", u8"Приве́т नमस שָׁלוֹםे שָׁ",
        u8"Приве́т नमस שָׁלוֹםे שָׁ   \n\t ", u8"   \n\t Приве́т नमस שָׁלוֹםे שָׁ"}
      };

      for(const ExpectedForInput &c: cases) {
        Assert::AreEqual(c.trimOut, trim(c.input));
        Assert::AreEqual(c.ltrimOut, ltrim(c.input));
        Assert::AreEqual(c.rtrimOut, rtrim(c.input));
      }
    }
  };

  TEST_CLASS(ConversionsTests) {
  public:
    TEST_METHOD(ConversionsTests_VariousInput_ExpectedOutput) {
      Logger::WriteMessage(__FUNCTION__);

      // miles <-> km
      Assert::AreEqual(1.6093, milesToKm(1.), 1e-3);
      Assert::AreEqual(0.6213, kmToMiles(1.), 1e-3);

      // radians <-> degrees
      Assert::AreEqual(180., radians<double>((double)Pi).get(true), 1e-3);
      Assert::AreEqual((double)Pi, 180.0_deg, 1e-3);
    }
  };
}
