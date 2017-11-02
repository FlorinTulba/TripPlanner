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
#include "place.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace tp::specs;

namespace UnitTests {
  TEST_CLASS(PlaceRelated) {
  public:
    TEST_METHOD(Gps_InvalidLatOrLong_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      Assert::ExpectException<invalid_argument>([] {
        GpsCoord<long double> g(-90.0006_deg, 0.0_deg); // lat<-90 deg
      });

      Assert::ExpectException<invalid_argument>([] {
        GpsCoord<long double> g(90.0006_deg, 0.0_deg); // lat>90 deg
      });

      Assert::ExpectException<invalid_argument>([] {
        GpsCoord<long double> g(0.0_deg, -180.0006_deg); // long<-180 deg
      });

      Assert::ExpectException<invalid_argument>([] {
        GpsCoord<long double> g(0.0_deg, 180.0006_deg); // long>180 deg
      });
    }

    TEST_METHOD(Gps_ValidValues_ok) {
      Logger::WriteMessage(__FUNCTION__);

      try {
        GpsCoord<long double> g1(-90.0_deg, -180.0_deg);
        GpsCoord<long double> g2(90.0_deg, 180.0_deg);
      } catch(exception &e) {
        Logger::WriteMessage(e.what());
        Assert::Fail();
      }
    }

    TEST_METHOD(Gps_CompareCoords_ExpectedResults) {
      Logger::WriteMessage(__FUNCTION__);

      try {
        GpsCoord<long double> g1(0.0_deg, 0.0_deg);
        Assert::IsFalse(g1<g1);

        GpsCoord<long double> g2(0.01_deg, 0.0_deg);
        GpsCoord<long double> g3(0.0_deg, 0.01_deg);

        Assert::IsTrue(g1<g2);
        Assert::IsFalse(g2<g1);
        Assert::IsTrue(g1<g3);
        Assert::IsFalse(g3<g1);
        Assert::IsTrue(g3<g2);
        Assert::IsFalse(g2<g3);

      } catch(exception &e) {
        Logger::WriteMessage(e.what());
        Assert::Fail();
      }
    }

    TEST_METHOD(Gps_GeodesicDist_ExpectedResults) {
      Logger::WriteMessage(__FUNCTION__);

      try {
        GpsCoord<long double>
          // Earth's poles:  
          g1(-90.0_deg, 0.0_deg),         // South
          g11(-90.0_deg, 12.3456789_deg), // South
          g2(90.0_deg, 0.0_deg),          // North

          // point on Equator
          g3(0.0_deg, 12.3456789_deg),

          // 2 identical points
          g4(12.3456789_deg, -180.0_deg),
          g41(12.3456789_deg, 180.0_deg),

          // 2 opposite points on the Equator
          g5(0.0_deg, 0.0_deg),
          g6(0.0_deg, 180.0_deg),
          
          // latitude 45S
          g7(-45.0_deg, 23.456789_deg);

        Assert::AreEqual(0., (double)g1.distanceTo(g11), 1e-4); // both are South
        Assert::AreEqual(0., (double)g4.distanceTo(g41), 1e-4); // same points

        const double earthDiam = 2. * (double)Pi * 6'371.008'8,
          
          // half Earth's diameter
          g1_g2 = (double)g1.distanceTo(g2),
          g2_g1 = (double)g2.distanceTo(g1),
          g5_g6 = (double)g5.distanceTo(g6),
          g6_g5 = (double)g6.distanceTo(g5),

          // a quarter of Earth's diameter
          g1_g3 = (double)g1.distanceTo(g3),
          g3_g2 = (double)g3.distanceTo(g2),
          
          // an eighth of Earth's diameter
          g7_g11 = (double)g7.distanceTo(g11);

        Assert::IsTrue(g1_g2 == g2_g1);
        Assert::IsTrue(g1_g3 == g3_g2);
        Assert::IsTrue(g5_g6 == g6_g5);

        // Pi * Earth's radius (half Earth's diameter)
        Assert::AreEqual(earthDiam/2., g1_g2, 1e-4);
        Assert::AreEqual(earthDiam/2., g5_g6, 1e-4);

        // Pi/2 * Earth's radius (a quarter of Earth's diameter)
        Assert::AreEqual(earthDiam/4., g1_g3, 1e-4);

        // Pi/4 * Earth's radius (an eighth of Earth's diameter)
        Assert::AreEqual(earthDiam/8., g7_g11, 1e-4);

      } catch(exception &e) {
        Logger::WriteMessage(e.what());
        Assert::Fail();
      }
    }

    TEST_METHOD(Place_InvalidNames_Throws) {
      Logger::WriteMessage(__FUNCTION__);

      Assert::ExpectException<invalid_argument>([] {
        Place p(1U, GpsCoord<float>(radians<float>(0.f), radians<float>(0.f)),
                u8" \t\t \t "); // empty name (and description)
      });

      Assert::ExpectException<invalid_argument>([] {
        Place p(1U, GpsCoord<float>(radians<float>(0.f), radians<float>(0.f)),
                u8" \t\t \t |\tpp \t\t"); // first name is empty
      });

      Assert::ExpectException<invalid_argument>([] {
        Place p(1U, GpsCoord<float>(radians<float>(0.f), radians<float>(0.f)),
                u8" \t\t pp \t |\tpp \t\t"); // both names are pp
      });
    }

    TEST_METHOD(Place_CorrectValues_ExpectedFields) {
      Logger::WriteMessage(__FUNCTION__);
      try {
        Place p(1U,
                GpsCoord<float>(radians<float>::fromDegrees(12.3456789f),
                                radians<float>::fromDegrees(123.456789f)),
                u8" \t\t pp1 \t |\tpp2 \t\t|pp3\t ", // names: pp1, pp2, pp3
                u8" \t\t descr \t "); // description: descr

        Assert::AreEqual(1U, p.id());
        const auto &pLoc = p.gpsCoord();
        Assert::AreEqual(12.3456789f, pLoc.latitude().get(true), 1e-4f);
        Assert::AreEqual(123.456789f, pLoc.longitude().get(true), 1e-4f);
        Assert::AreEqual(u8"descr"s, p.shortDescr());
        const auto &pNames = p.names();
        Assert::AreEqual(3ULL, pNames.size());
        Assert::AreEqual(u8"pp1"s, pNames[0ULL]);
        Assert::AreEqual(u8"pp2"s, pNames[1ULL]);
        Assert::AreEqual(u8"pp3"s, pNames[2ULL]);

      } catch(exception &e) {
        Logger::WriteMessage(e.what());
        Assert::Fail();
      }
    }
  };
}
