
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

/*! \file algebra/nsvpolynomialring.h
 *  \brief A single variable polynomial ring object, implemented sparsely.
 */

#ifndef __SVPolynomialRing_H
#ifndef __DOXYGEN
#define __SVPolynomialRing_H
#endif

#include <sstream>
#include <map>
#include <vector> 
#include <cstdlib>
#include <list>

#include "regina-core.h"
#include "maths/integer.h"
#include "utilities/ptrutils.h"

namespace regina {

/**
 * \weakgroup algebra
 * @{
 */

/**
 * This is a class for dealing with elements of a single-variable polynomial 
 * ring, implemented sparsely.  The template class must be of a "ring" type, 
 * meaning T must contain:
 *  1) A copy constructor. 
 *  2) An assignment "=" operator. 
 *  3) An output operation std::string T::stringValue()
 *  4) Operations +, -, +=, -=, ==, <, abs()
 *  5) T casts unsigned integers 
 *  6) Present implementation also assumes the ring is without zero divisors.
 *
 * \testpart
 *
 * \todo allow for rational coefficients. Template ring.
 *
 * @author Ryan Budney
 */
// TODO: currently width returns zero for both a zero polynomial and a monomial. 
//       like kt^j.  We should migrate to monomials being width 1, but this will
//       require fixing old code. 
template <class T>
class NSVPolynomialRing {
    public:
	static const NSVPolynomialRing<T> zero;
        static const NSVPolynomialRing<T> one;
        static const NSVPolynomialRing<T> pvar;
    private:
       // sparse storage of coefficients
       std::map< signed long, T* > cof;

    public:
        /**
         * Creates an element of the polynomial ring, zero by default. 
         */
        NSVPolynomialRing();

        /**
         * Creates an element of the polynomial ring, of the form at^k 
         */
        NSVPolynomialRing(const T &a, signed long k);

        /**
         * Creates a constant polynomial of the form "a", needed to allow for
         * expressions like p==0 where p is an NSVPolynomialRing object.
         */
        NSVPolynomialRing(signed long a);

        /**
         * Creates a polynomial of the form:
         *
         *  t^{m-n}+...+t^{m-dn} if d>0,  -t^m-t^{m+n}-...-t^{m-(d+1)n} if d<0
         *
         *  These polynomials are useful for situations where one can divide 
         *  two integer polynomials, ie if n=dm+r with 0<=r<|m|, then 
         *  t^m-1 = (NSVPolynomialRing(n,m,d))*(t^n-1) + (t^r-1)
         */
        NSVPolynomialRing( signed long n, signed long m, signed long d );

        /**
         * Destructor.
         */
        virtual ~NSVPolynomialRing();

        /**
         * Creates a polynomial that is a clone of the given
         * polynomial.
         *
         * @param cloneMe the permutation to clone.
         */
        NSVPolynomialRing(const NSVPolynomialRing<T>& cloneMe);

        /**
         * Sets this polynomial to the given polynomial.
         */
        NSVPolynomialRing<T>& operator = (const NSVPolynomialRing<T>& cloneMe);

        /**
         * Set a coefficient.
         */
        void setCoefficient (signed long i, const T& c);

        /**
         * Returns the product of two polynomials.
         */
        NSVPolynomialRing<T> operator * (const NSVPolynomialRing<T>& q) const;

        /**
         * Scalar multiplication of a polynomial.
         */
        template <class F>
        friend NSVPolynomialRing<F> operator * (const F &k, 
               const NSVPolynomialRing<F>& q);

        /**
         * Returns the sum of two polynomials.
         */
        NSVPolynomialRing<T> operator + (const NSVPolynomialRing<T>& q) const;

        /**
         * Returns the difference of two polynomials.
        */
        NSVPolynomialRing<T> operator - (const NSVPolynomialRing<T>& q) const;

        /**
         * Decrement operator. 
         */
        NSVPolynomialRing<T>& operator -=(const NSVPolynomialRing<T>& q);

        /**
         * Increment operator. 
         */
        NSVPolynomialRing<T>& operator +=(const NSVPolynomialRing<T>& q);

        /**
         * Negation operator. 
         */
        NSVPolynomialRing<T> operator -() const;

        /**
         * Returns the the coefficient of t^i for this polynomial.
         */
        const T& operator[](signed long i) const;

        /**
         * Determines if two polynomials are equal.
         */
        bool operator == (const NSVPolynomialRing<T>& other) const;

        /**
         * Determines if two polynomials are not equal.
         */
        bool operator != (const NSVPolynomialRing<T>& other) const;

        /** 
         * Evaluate polynomial
         */
        T eval(const T &input) const;

        /**
         * Determines if this polynomial is equal to the multiplicative 
         * identity element.
         */
        bool isIdentity() const;

        /**
         * Determines if this polynomial is equal to the additive identity
         * element.
         */
        bool isZero() const;

        /** 
         * Determines if the polynomial is symmetric, i.e. up to a 
         * multiple of t^k,  Check if it's true that p(t)=p^(t^-1)
         */
        bool isSymmetric() const; 

        /**
         * Returns the number of sign changes in coefficients of the polynomial
         * P(t) - number of sign changes of P(-t). This is the number of + 
         * roots - number of - roots, provided all real. 
         */
        long int descartesNo() const;

        /**
         * The width is the difference between the exponents of the largest and
         * smallest degree non-zero terms.  
         */
        unsigned long width() const;

        /**
         * The degree returns the exponent of the term whose exponent has the
         * maximum absolute value.  
         */
        signed long degree() const;

        /**
         * Returns the degree in the sense of Pauer-Unterkircher, the number 
         * of non-zero terms in the polynomial.
         */
        unsigned long PU_degree() const;

        /**
         * Returns the leading term of the Pauer-Unterkircher sense, the 
         * coefficient of the highest-degree term.
         */
        T PU_leadTerm() const;

        /**
         *  Returns the lowest-degree (most negative degree) term.
         */
        std::pair<signed long, T> firstTerm() const;

        /**
         *  Returns the highest-degree (most positive degree) term.
         */
        std::pair<signed long, T> lastTerm() const;

        /**
         * All terms of the polynomial
         */
        const std::map< signed long, T* >& allTerms() const;

        /**
         * Returns a string representation of this polynomial.
         * <tt>a + bt + ct^2 + ... </tt>
         *
         * suppressZero is a debugging flag. Set to false to check for any
         *  0t^n terms.
         */
        std::string toString(bool suppressZero=true) const;

        /**
         * Stream version of toString()
         */
        void writeTextShort(std::ostream& out) const;

        /**
         * Returns TeX string formatting of toString()
         */
        std::ostream& writeTeX(std::ostream &out) const;

        /**
         * string version of writeTeX.
         */
        std::string texString() const;
};

/**
 * Produces a Groebner basis for the ideal.  Set laurentPoly=false to work in 
 * Z[t] rather than Z[t^\pm]. Implements Groebner basis algorithms adapted for
 * Laurent polynomial rings from Pauer and Unterkircher Groebner Basis for 
 * Ideals in Laurent Polynomial Rings and their Application to Systems of 
 * Difference Equations.  AAECC 9, 271--291 (1999). 
 */
REGINA_API void reduceIdeal(  std::list< NSVPolynomialRing< NLargeInteger > > 
 &ideal, bool laurentPoly=true );
/**
 * Given an element elt of NSVPolynomialRing, this algorithm checks to see if
 * it reduces to 0 by taking remainders via division by elements of ideal. 
 * laurentPoly allows one to assume in Laurent polynomial ring. Set it to false
 * if you want to be in Z[t]. 
 */
REGINA_API bool reduceByIdeal( const std::list< NSVPolynomialRing< 
        NLargeInteger > > &ideal, 
        NSVPolynomialRing< NLargeInteger > &elt, bool laurentPoly=true );
/**
 *  Some kind of ordering < on ideals.  Useful? 
 */
REGINA_API bool ideal_comparison( const NSVPolynomialRing< NLargeInteger > 
 &first, const NSVPolynomialRing< NLargeInteger > &second);
/**
 * Check if idealA is contained in idealB. Assumes you've run them through 
 * reduceIdeal -- that they have their Groebner basis. 
 */
REGINA_API bool isSubIdeal( 
 const std::list< NSVPolynomialRing< NLargeInteger > > &idealA,  
 const std::list< NSVPolynomialRing< NLargeInteger > > &idealB );

/**
 * Checks to see if elements of the ideal can be expressed in terms of 
 * others, if so erases them. 
 */
REGINA_API void elementaryReductions( 
 std::list< NSVPolynomialRing< NLargeInteger > > &ideal );

/**
 *  Computes the GCD of elements in input, output is a vector such that the sum
 *  over i, input[i]*outputG[i] == GCD.  We assume output is initialized to the
 *  size of input.  Sum over i, input[i]*outputN[i] == 0 (for a non-trivial 
 *  outputN) Assumes input.size()>=1. If input.size()==1, outputN will be zero. 
 */
REGINA_API NLargeInteger gcd( const std::vector< NLargeInteger > &input,
 std::vector< NLargeInteger > &outputG, std::vector< NLargeInteger > &outputN );

REGINA_API inline void prettifyPolynomial( 
 NSVPolynomialRing< NLargeInteger > &poly );

/*@}*/

// Inline functions for NSVPolynomialRing

// zero element -- nothing to do
template <class T>
inline NSVPolynomialRing<T>::NSVPolynomialRing() {}

// monomial constructor
template <class T>
inline NSVPolynomialRing<T>::NSVPolynomialRing(const T &a, signed long k)
{ if (a != T::zero) cof.insert(std::pair<signed long, T*>( k, new T(a) ) ); }

// cast signed longs.
template <class T>
inline NSVPolynomialRing<T>::NSVPolynomialRing(signed long a)
{ if (a != 0) cof.insert(std::pair<signed long, T*>( 0, new T(a) ) ); }

// destructor
template <class T>
inline NSVPolynomialRing<T>::~NSVPolynomialRing()
{
 typename std::map< signed long, T* >::iterator ci;
 for (ci = cof.begin(); ci != cof.end(); ci++)
	delete ci->second;
 cof.clear();
}

// copy constructor
template <class T>
inline NSVPolynomialRing<T>::NSVPolynomialRing(
 const NSVPolynomialRing<T>& cloneMe)
{
 typename std::map< signed long, T* >::const_iterator ci;
 for (ci = cloneMe.cof.begin(); ci != cloneMe.cof.end(); ci++) cof.insert( 
  std::pair< signed long, T* >( ci->first, clonePtr(ci->second) ) );
}

// assignment
template <class T>
inline NSVPolynomialRing<T>& NSVPolynomialRing<T>::operator = (
 const NSVPolynomialRing<T>& cloneMe)
{
 // deallocate everything
 typename std::map< signed long, T* >::iterator ci;
 for (ci = cof.begin(); ci != cof.end(); ci++) 
	delete ci->second;
 cof.clear();
 // copy everything
 ci = cof.begin();
 typename std::map< signed long, T* >::const_iterator Ci;
 for (Ci = cloneMe.cof.begin(); Ci != cloneMe.cof.end(); Ci++)
	ci = cof.insert(ci, std::pair< signed long, T* >
    ( Ci->first, clonePtr(Ci->second) ));
 return (*this);
}

template <class T>
inline const std::map< signed long, T* >& NSVPolynomialRing<T>::allTerms() const
{ return cof; }


template <class T>
inline const T& NSVPolynomialRing<T>::operator[](signed long i) const
{
 // is t^i in cof?  if so return coefficient, if not, return 0.
 typename std::map< signed long, T* >::const_iterator p;
 p = cof.find(i);
 if (p != cof.end()) return (*p->second);
 else return T::zero;
}

template <class T>
inline bool NSVPolynomialRing<T>::operator == 
 (const NSVPolynomialRing<T>& other) const
{
 // verify equal...
 if (cof.size() != other.cof.size()) return false;
 typename std::map< signed long, T* >::const_iterator p1, p2;
 p1 = cof.begin(); p2 = other.cof.begin();
 while (p1 != cof.end() )
  {
   if ( p1->first != p2->first ) return false;
   if ( (*p1->second) != (*p2->second) ) return false;
   p1++; p2++;
  }
 return true;
}

template <class T>
inline bool NSVPolynomialRing<T>::operator !=
  (const NSVPolynomialRing<T>& other) const
{ return ( !((*this)==other) ); }

template <class T>
inline bool NSVPolynomialRing<T>::isIdentity() const
{
 // check only nonzero coeff is t^0 with coeff 1.
 if (cof.size() != 1) return false;
 if (cof.begin()->first != 0) return false;
 if (*(cof.begin()->second) != T::one) return false;
 return true;
}

template <class T>
inline bool NSVPolynomialRing<T>::isZero() const
{ return cof.empty(); }

template <class T>
inline unsigned long NSVPolynomialRing<T>::width() const
{
 // find the first and last elements of the list "cof", return
 // difference of exponents.
 if (cof.empty()) return 0;
 typename std::map<signed long, T*>::const_iterator i(cof.begin());
 typename std::map<signed long, T*>::const_reverse_iterator j(cof.rbegin());
 return ( (*j).first - (*i).first );
}

template <class T>
inline signed long NSVPolynomialRing<T>::degree() const
{
 // find first and last elements of the list "cof", return max of abs
 //  of exponents.
 if (cof.empty()) return 0;
 typename std::map<signed long, T*>::const_iterator i(cof.begin());
 typename std::map<signed long, T*>::const_reverse_iterator j(cof.rbegin());
 return ( ( abs((*j).first) > abs((*i).first) ) ? (*j).first : (*i).first );
}

template <class T>
inline unsigned long NSVPolynomialRing<T>::PU_degree() const
{ return cof.size(); }

template <class T>
inline T NSVPolynomialRing<T>::PU_leadTerm() const
{ typename std::map<signed long, T*>::const_reverse_iterator j(cof.rbegin()); 
  return ( *(j->second) ); }

template <class T>
inline std::pair<signed long, T> NSVPolynomialRing<T>::firstTerm() const
{ typename std::map<signed long, T*>::const_iterator j(cof.begin()); 
  return ( std::pair< signed long, T >(j->first, *(j->second) ) ); }

template <class T>
inline std::pair< signed long, T> NSVPolynomialRing<T>::lastTerm() const
{ typename std::map<signed long, T*>::const_reverse_iterator j(cof.rbegin()); 
  return ( std::pair< signed long, T >(j->first, *(j->second) ) ); }


template <class T>
inline NSVPolynomialRing<T> operator * 
 (const T &k, const NSVPolynomialRing<T>& q)
{
 NSVPolynomialRing<T> retval;
 if (k != T::zero)
  {
  typename std::map< signed long, T* >::iterator ci;
  for (ci = q.cof.begin(); ci != q.cof.end(); ci++)
	retval.cof.insert( std::pair< signed long, T* >
    ( ci->first, new T( (*ci->second)*k ) ) );
  }
 return retval;
}

template <class T>
inline std::ostream& operator << (std::ostream& out, 
 const NSVPolynomialRing<T>& p)
{ p.writeTextShort(out); return out; }

template <class T>
const NSVPolynomialRing<T> NSVPolynomialRing<T>::zero;

template <class T>
const NSVPolynomialRing<T> NSVPolynomialRing<T>::one(  T(1), 0 );

template <class T>
const NSVPolynomialRing<T> NSVPolynomialRing<T>::pvar( T(1), 1 );

template <class T>
inline void NSVPolynomialRing<T>::setCoefficient (signed long i, const T & c) 
{
 if (c==T::zero)
  {
   typename std::map< signed long, T* >::iterator it(cof.find(i)); 
   // look for t^i
   if (it != cof.end()) cof.erase(it); return;
  }
 T* P(new T(c));
 std::pair< typename std::map< signed long, T* >::iterator, bool > res = 
  cof.insert(std::pair< signed long, T* >( i, P ) );
 if (res.second == false) { (*res.first->second) = (*P); delete P; }
}

template <class T>
inline NSVPolynomialRing<T> NSVPolynomialRing<T>::operator * 
 (const NSVPolynomialRing<T>& q) const
{
 // There's a faster way to do polynomial multiplication using the FFT. See
 // http://www.cs.iastate.edu/~cs577/handouts/polymultiply.pdf
 // Fateman 2005 indicates nobody has implemented such algorithms in any
 // major package, and that the asymptotic advantage only appears for extremely
 // large polynomials, a problem being that roots of unity are required so they
 // seem to think arbitrary precision complex numbers are required, which is 
 // slow.  Can we avoid this using exact arithmetic? 
 typename std::map< signed long, T* >::const_iterator I, J;
 NSVPolynomialRing<T> retval;

 for (I = cof.begin(); I!=cof.end(); I++)
  for (J=q.cof.begin(); J!=q.cof.end(); J++)
  {
   // lets multiply *I->second and *J->second
   T* P(new NLargeInteger( (*I->second)*(*J->second) ) );
   std::pair< typename std::map< signed long, T* >::iterator, bool > res = 
    retval.cof.insert( std::pair< signed long, T* > (
    I->first+J->first, P ) );
   if (res.second == false) { (*res.first->second) += (*P); delete P; }
  }

 //  now run through find zero coefficients and deallocate.
 typename std::map< signed long, T* >::iterator K;
 for (K = retval.cof.begin(); K!=retval.cof.end(); K++)
   while ( (*K->second) == NLargeInteger(0) ) 
    { delete K->second; retval.cof.erase(K++);  
      if (K==retval.cof.end()) continue; }
 return retval;
}

template <class T>
inline NSVPolynomialRing<T> NSVPolynomialRing<T>::operator + 
        (const NSVPolynomialRing<T>& q) const
{
 NSVPolynomialRing<T> retval;
 // two iterators, one for this->cof, one for q.cof, start at beginning
 // for both, smallest we add, if only one then that's okay. We increment
 // smallest, repeat...
 typename std::map< signed long, T* >::const_iterator i,j;
 i = cof.begin(); j = q.cof.begin();
 while ( (i != cof.end()) || (j != q.cof.end()) )
 {
  if (i == cof.end())
   { // only j relevant
     retval.cof.insert( std::pair< signed long, T* >
        (j->first, new T(*j->second) ) );
     j++; }
  else if (j == q.cof.end())
   { // only i relevant
     retval.cof.insert( std::pair< signed long, T* >
        (i->first, new T(*i->second) ) );
     i++; }
  else
   {if ( i->first < j->first )
     { retval.cof.insert( std::pair< signed long, T* >
        (i->first, new T(*i->second) ) );
       i++; }
    else if ( i->first > j->first )
     { retval.cof.insert( std::pair< signed long, T* >
            (j->first, new T(*j->second) ) );
       j++; }
    else
     { T* temp(new T( (*i->second) + (*j->second) ) );
       if ( (*temp) != T::zero )
       retval.cof.insert( std::pair< signed long, T* >
            (i->first, temp ) );
	else delete temp;
       i++; j++; } 
   }
 }
 return retval;
}

template <class T>
inline NSVPolynomialRing<T> NSVPolynomialRing<T>::operator -
  (const NSVPolynomialRing<T>& q) const
{
 NSVPolynomialRing<T> retval;
 // two iterators, one for this->cof, one for q.cof, start at beginning for
 // both, smallest we add, if only one then that's okay. We increment 
 // smallest, repeat...
 typename std::map< signed long, T* >::const_iterator i,j;
 i = cof.begin(); j = q.cof.begin();
 while ( (i != cof.end()) || (j != q.cof.end()) )
 {
  if (i == cof.end())
   { // only j relevant
     retval.cof.insert( std::pair< signed long, T* >(j->first, 
            new T(-(*j->second) ) ) );
     j++; }
  else if (j == q.cof.end())
   { // only i relevant
     retval.cof.insert( std::pair< signed long, T* >(i->first, 
                new T(*i->second) ) );
     i++; }
  else
   {if ( i->first < j->first )
     { retval.cof.insert( std::pair< signed long, T* >(i->first, 
                new T(*i->second) ) );
       i++; }
    else if ( i->first > j->first )
     { retval.cof.insert( std::pair< signed long, T* >(j->first, 
            new T(-(*j->second) ) ) );
       j++; }
    else
     { T* temp(new NLargeInteger( (*i->second) - (*j->second) ) );
       if ( (*temp) != T::zero )
       retval.cof.insert( std::pair< signed long, T* >(i->first, temp ) );
	else delete temp;
       i++; j++; } 
   }
 }
 return retval;
}

template <class T>
inline NSVPolynomialRing<T>& NSVPolynomialRing<T>::operator -=(const 
                                            NSVPolynomialRing<T>& q)
{ // todo: redo using insert w/iterator
 typename std::map< signed long, T* >::iterator i;
 typename std::map< signed long, T* >::const_iterator j;
 i = cof.begin(); j = q.cof.begin();
 while ( (i != cof.end()) || (j != q.cof.end()) )
  { if (i == cof.end()) { cof.insert( std::pair< signed long, T* >
                                (j->first, new T(-(*j->second) ) ) ); j++; }
    else if (j == q.cof.end()) i++;  
    else if ( i->first < j->first ) i++;
    else if ( i->first > j->first ) { cof.insert( std::pair< signed long, T* >
                                (j->first, new T(-(*j->second) ) ) ); j++; }
    else
      { (*i->second) -= (*j->second); 
        if (*i->second == T::zero) 
    // we have to deallocate the pointer, remove *i from cof, 
    //  and move i and j up the cof list.
          { delete i->second; cof.erase(i++); j++; } 
        else { i++; j++; }
      }
  }

 return (*this);
}

template <class T>
inline NSVPolynomialRing<T>& NSVPolynomialRing<T>::operator +=(const 
        NSVPolynomialRing<T>& q)
{ // todo: redo using insert w/iterator
 typename std::map< signed long, T* >::iterator i;
 typename std::map< signed long, T* >::const_iterator j;
 i = cof.begin(); j = q.cof.begin();
 while ( (i != cof.end()) || (j != q.cof.end()) )
  { if (i == cof.end()) { cof.insert( std::pair< signed long, T* >
                            (j->first, new T( (*j->second) ) ) ); j++; }
    else if (j == q.cof.end()) i++; 
    else if ( i->first < j->first ) i++; 
    else if ( i->first > j->first ) { cof.insert( std::pair< signed long, T* >
                                (j->first, new T( (*j->second) ) ) ); j++; }
    else
      { (*i->second) += (*j->second); 
        if (*i->second == T::zero) 
   // we have to deallocate the pointer, remove *i from cof, 
   // and move i and j up the cof list.
          { delete i->second; cof.erase(i++); j++; } 
        else { i++; j++; }
      }
  }
 return (*this);
}

template <class T>
inline NSVPolynomialRing<T> NSVPolynomialRing<T>::operator -() const
{
 NSVPolynomialRing<T> retval(*this);
 typename std::map< signed long, T* >::iterator i;
 for (i = retval.cof.begin(); i!=retval.cof.end(); i++)
  i->second->negate();
 return retval;
}


template <class T>
inline long int NSVPolynomialRing<T>::descartesNo() const
{
 long int retval = 0;
 bool signP(false), signN(false); // keeps track of signs L to R for 
  //                                 P(t) and P(-t) respectively.
 typename std::map< signed long, T* >::const_iterator i;

 for (i = cof.begin(); i!=cof.end(); i++)
  {
   bool NsignP = ( (*i->second) > 0 ? true : false );
   bool NsignN = NsignP; if ( i->first % 2 == 1 ) NsignN = !NsignN; 

   if (i != cof.begin()) // compare
    {
     if (signP != NsignP) retval++;
     if (signN != NsignN) retval--;
    }
   signP = NsignP; signN = NsignN;
  }
return retval;
}

  /**
   * Creates a polynomial of the form:
   *
   *  t^{m-n}+...+t^{m-dn} if d>0,      -t^m-t^{m+n}-...-t^{m-(d+1)n} if d<0
   *
   *  these polynomials are useful for situations where one can divide two 
   *  integer polynomials, ie if n=dm+r with 0<=r<|m|, then 
   *   t^m-1 = (NSVPolynomialRing(n,m,d))*(t^n-1) + (t^r-1)
   */
template <class T>
inline NSVPolynomialRing<T>::NSVPolynomialRing( signed long n, signed long m, 
                                                signed long d )
{
typename std::map< signed long, T*>::iterator i = cof.begin();
signed long exp=m;
if (d>0) for (unsigned long j=0; j<d; j++)
 {
  exp -= n;
  T* P(new T(T::one));
  i = cof.insert( i, std::pair<signed long, T*> (exp, P) );
 }
else for (unsigned long j=0; j<abs(d); j++)
 {
  T* P(new T(-T::one));
  i = cof.insert( i, std::pair<signed long, T*> (exp, P) );
  exp += n;
 }
}

template <class T>
inline std::string NSVPolynomialRing<T>::toString(bool suppressZero) const
{
// run through cof, assemble into string
std::string retval; std::stringstream ss;
typename std::map< signed long, T* >::const_iterator p;
bool outputSomething = false; 
// keep track of whether or not we've output anything
for (p = cof.begin(); p != cof.end(); p++)
 {
  T mag( p->second->abs() );  bool pos( ((*p->second) > 0) ? true : false );  
    signed long exp( p->first );
  ss.str("");  ss<<p->first;
  // ensure sensible output, eg:  t^(-5) + 5 - t + 3t^2 + t^3 - t^(12), etc...
  if ( (mag != 0) || (suppressZero==false) ) // don't output 0t^n
   { 
   if (outputSomething) retval += ( pos ? "+" : "-" ); else 
   if (!pos) retval += "-";
   outputSomething = true;
   if ( (exp==0) || (mag != 1) ) retval += mag.stringValue();
   if (exp == 1) retval += "t"; else 
   if ( (exp<0) || (exp>9) ) retval += "t^(" + ss.str() + ")"; else
    if (exp != 0) retval += "t^" + ss.str();
   }
 }
if (outputSomething == false) retval = "0";
return retval;
}

template <class T>
inline void NSVPolynomialRing<T>::writeTextShort(std::ostream& out) const
{ out<<toString(); }

template <class T>
inline std::ostream& NSVPolynomialRing<T>::writeTeX(std::ostream &out) const
{ out<<texString(); return out; }

template <class T>
inline std::string NSVPolynomialRing<T>::texString() const
{
// run through cof, assemble into string
std::string retval; std::stringstream ss;
typename std::map< signed long, T* >::const_iterator p;
bool outputSomething = false; 
// keep track of whether or not we've output anything
for (p = cof.begin(); p != cof.end(); p++)
 {
  T mag( p->second->abs() );  
  bool pos( ((*p->second) > 0) ? true : false );  signed long exp( p->first );
  ss.str("");  ss<<p->first;
  // ensure sensible output, eg:  t^{-5} + 5 - t + 3t^2 + t^3 - t^{12}, etc...
  if (mag != 0) // don't output 0t^n
   { 
   if (outputSomething) retval += ( pos ? "+" : "-" ); else 
   if (!pos) retval += "-";
   outputSomething = true;
   if ( (exp==0) || (mag != 1) ) retval += mag.stringValue();
   if (exp == 1) retval += "t"; else 
   if ( (exp<0) || (exp>9) ) retval += "t^{" + ss.str() + "}"; else
    if (exp != 0) retval += "t^" + ss.str();
   }
 }
if (outputSomething == false) retval = "0";
return retval;
}

/** 
 * Given a Laurent polynomial, it ensures via multiplication by a unit 
 * \pm 1 t^k that if it is non-zero, the smallest-degree non-zero term is
 * the constant term, and if it evaluates to a non-zero number, that number
 * is positive. 
 */
REGINA_API inline void prettifyPolynomial( 
 NSVPolynomialRing< NLargeInteger > &poly )
{
 // step 1: check if polynomial is zero
  if (poly.isZero()) return;

 // step 2: if not, multiply by t^k to ensure smallest degree term is t^0
  std::pair<signed long, NLargeInteger> firstAPterm(poly.firstTerm());
  std::pair<signed long, NLargeInteger> lastAPterm( poly.lastTerm() );
  NSVPolynomialRing< NLargeInteger > trans 
   ( NLargeInteger::one, -firstAPterm.first );  
  poly = poly * trans;
 // step 3: multiply by -1 if polynomial evaluates to negative at t=1. 
  NLargeInteger evalone( poly.eval(NLargeInteger::one) );
  if (evalone < 0) poly=(NSVPolynomialRing<NLargeInteger>(-1))*poly;
}

/** 
 * Evaluate polynomial.  It must be an actual polynomial for this to work, 
 * as we do not assume the class T has inverses.  If the polynomial has terms 
 * of degree less than 0, routine returns 0. 
 */
template <class T>
REGINA_API inline T NSVPolynomialRing<T>::eval(const T &input) const
{
 if (firstTerm().first<0) return T::zero; 
 // compute input^n recursively, building up sum from left to right. 
 T retval(T::zero);
 //        std::map< signed long, T* > cof;
 typename std::map< signed long, T* >::const_iterator cofi;
 unsigned long k=0;  T inputk(T::one);
 for (cofi = cof.begin(); cofi != cof.end(); cofi++)
  {
   // determine gap between cofi->first and k, multiply
   // appropriate amount to get inputk
   for (unsigned long i=k; i<cofi->first; i++)
    { inputk = inputk * input; }
   k=cofi->first; 
   retval = retval + (*cofi->second) * inputk;
  }
 return retval;
}

template <class T>
REGINA_API inline bool NSVPolynomialRing<T>::isSymmetric() const
{
 // use a forward and backwards iterator and check they agree at every 
 // step until they pass each other
 typename std::map< signed long, T* >::const_iterator cofF; 
 typename std::map< signed long, T* >::const_reverse_iterator cofB;
 cofF = cof.begin();  cofB = cof.rbegin();
 signed long gap = cofF->first + cofB->first; 
  // this should stay constant through
  // incrementation if poly is symmetric
 while ( cofF->first > cofB->first )
  {
   if (cofF->second != cofB->second) return false; 
   cofF++; cofB++;
   if (cofF->first + cofB->first != gap) return false; 
  }
 return true;
}

} // namespace regina

#endif

