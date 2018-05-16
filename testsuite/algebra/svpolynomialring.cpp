
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Test Suite                                                            *
 *                                                                        *
 *  Copyright (c) 1999-2013, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  As an exception, when this program is distributed through (i) the     *
 *  App Store by Apple Inc.; (ii) the Mac App Store by Apple Inc.; or     *
 *  (iii) Google Play by Google Inc., then that store may impose any      *
 *  digital rights management, device limits and/or redistribution        *
 *  restrictions that are required by its terms of service.               *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include <sstream>
#include <cppunit/extensions/HelperMacros.h>
#include "algebra/svpolynomialring.h"
#include "maths/integer.h"
#include "testsuite/algebra/testalgebra.h"

using regina::Integer;
using regina::SVPolynomialRing;

class SVPolynomialRingTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SVPolynomialRingTest);

    CPPUNIT_TEST(additive_struc_test);
    CPPUNIT_TEST(multiplicative_struc_test);
    CPPUNIT_TEST(ring_struc_test);
    CPPUNIT_TEST(degree_and_width_test);
    CPPUNIT_TEST(bogus_terms);

    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {}
        void tearDown() {}

	void additive_struc_test() {
	  for (unsigned long k=0; k<20; k++)
	   {
	   regina::SVPolynomialRing< regina::Integer > p1, p2, p3;
	   for (unsigned long i=0; i<10; i++)
	    {
		p1 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(7), 3*i );
		p2 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(4), 4*((signed long)i-5) );
		p3 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(3), 5*i );
	    }
	   if ( (p1+p2)+p3 != p1+(p2+p3) ) CPPUNIT_FAIL("Addition not associative.");
	   if ( p1 + SVPolynomialRing< regina::Integer >::zero != p1 ) CPPUNIT_FAIL("Zero does not exist (1).");
	   if ( SVPolynomialRing< regina::Integer >::zero + p1 != p1 ) CPPUNIT_FAIL("Zero does not exist (2).");
	   if ( (p1 - p1) != SVPolynomialRing< regina::Integer >::zero ) CPPUNIT_FAIL("No additive inverse.");
	   if ( p1 + p2 != p2 + p1 ) CPPUNIT_FAIL("Addition not commutative.");
	   }
	 // + is assoc, id, inverses
	}
	void multiplicative_struc_test() {
	  for (unsigned long k=0; k<20; k++)
	   {
	   regina::SVPolynomialRing< regina::Integer > p1, p2, p3;
	   for (unsigned long i=0; i<10; i++)
	    {
		p1 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(7)-64, 3*i );
		p2 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(4)-8, 4*((signed long)i-5) );
		p3 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(3)-4, 5*i );
	    }
	   if ( (p1*p2)*p3 != p1*(p2*p3) ) CPPUNIT_FAIL("Multiplication not associative.");
	   if ( p1 * SVPolynomialRing< regina::Integer >::one != p1 ) CPPUNIT_FAIL("One does not exist (1).");
	   if ( SVPolynomialRing< regina::Integer >::one * p1 != p1 ) CPPUNIT_FAIL("One does not exist (2).");
	   if ( p1 * p2 != p2 * p1 ) CPPUNIT_FAIL("Multiplication not commutative.");
	   }
	 // * is assoc, id
	}
	void ring_struc_test() {
	  for (unsigned long k=0; k<20; k++)
	   {
	   regina::SVPolynomialRing< regina::Integer > p1, p2, p3;
	   for (unsigned long i=0; i<10; i++)
	    {
		p1 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(7)-64, 3*i );
		p2 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(4)-8, 4*((signed long)i-5) );
		p3 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(3)-4, 5*i );
	    }
	   if ( (p1+p2)*p3 != (p1*p3) + (p2*p3) ) CPPUNIT_FAIL("Multiplication and addition do not distribute (1).");
	   if ( p1*(p2+p3) != (p1*p2) + (p1*p3) ) CPPUNIT_FAIL("Multiplication and addition do not distribute (2).");
	   }
	 // distrib
	}
       void degree_and_width_test() {
 	  for (unsigned long k=0; k<20; k++)
	   {
	   regina::SVPolynomialRing< regina::Integer > p1, p2;
	   for (unsigned long i=0; i<10; i++)
	    {
		p1 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(7)-64, 3*i );
		p2 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(4)-8, 4*((signed long)i-5) );
            }
           if (abs(p1.degree())+abs(p2.degree()) < abs((p1*p2).degree()) ) CPPUNIT_FAIL("Degree under multiplication error. "+
                                                p1.toString()+"*"+p2.toString()+" and "+(p1*p2).toString());
           if (p1.width()+p2.width() != (p1*p2).width() ) CPPUNIT_FAIL("Width under multiplication error. "+
                                                p1.toString()+"*"+p2.toString()+" and "+(p1*p2).toString());
           }
          }
       void bogus_terms() {
	  for (unsigned long k=0; k<20; k++)
	   {
	   regina::SVPolynomialRing< regina::Integer > p1, p2, p3;
	   for (unsigned long i=0; i<10; i++)
	    {
		p1 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(7)-64, 3*i );
		p2 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(4)-8, 4*((signed long)i-5) );
		p3 += SVPolynomialRing< regina::Integer >( Integer::randomBinary(3)-4, 5*i );
	    }
           if (p1.toString(false) != p1.toString(true)) CPPUNIT_FAIL("Bogus zero init.");
           if (p2.toString(false) != p2.toString(true)) CPPUNIT_FAIL("Bogus zero init.");
           if (p3.toString(false) != p3.toString(true)) CPPUNIT_FAIL("Bogus zero init.");
           if ((p1*p2).toString(false) != (p1*p2).toString(true)) CPPUNIT_FAIL("Bogus zero *.");
           if ((p2*p3).toString(false) != (p2*p3).toString(true)) CPPUNIT_FAIL("Bogus zero *.");
           if ((p3*p1).toString(false) != (p3*p1).toString(true)) CPPUNIT_FAIL("Bogus zero *.");
           if ((p1+p2).toString(false) != (p1+p2).toString(true)) CPPUNIT_FAIL("Bogus zero +.");
           if ((p2+p3).toString(false) != (p2+p3).toString(true)) CPPUNIT_FAIL("Bogus zero +.");
           if ((p3+p1).toString(false) != (p3+p1).toString(true)) CPPUNIT_FAIL("Bogus zero +.");
           if ((p1-p2).toString(false) != (p1-p2).toString(true)) CPPUNIT_FAIL("Bogus zero -.");
           if ((p2-p3).toString(false) != (p2-p3).toString(true)) CPPUNIT_FAIL("Bogus zero -.");
           if ((p3-p1).toString(false) != (p3-p1).toString(true)) CPPUNIT_FAIL("Bogus zero -.");
           p1 += p2; p2 += p3; 
           if (p1.toString(false) != p1.toString(true)) CPPUNIT_FAIL("Bogus zero +=.");
           if (p2.toString(false) != p2.toString(true)) CPPUNIT_FAIL("Bogus zero +=.");
           p1 -= p2; p2 -= p3; 
           if (p1.toString(false) != p1.toString(true)) CPPUNIT_FAIL("Bogus zero -=.");
           if (p2.toString(false) != p2.toString(true)) CPPUNIT_FAIL("Bogus zero -=.");
	   }
          }
};

void addSVPolynomialRing(CppUnit::TextUi::TestRunner& runner) {
    runner.addTest(SVPolynomialRingTest::suite());
}

