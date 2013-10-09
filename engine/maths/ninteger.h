
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

#ifndef __NINTEGER_H
#ifndef __DOXYGEN
#define __NINTEGER_H
#endif

/*! \file maths/ninteger.h
 *  \brief Provides arbitrary-precision and fixed-precision integer types.
 */

#include <boost/static_assert.hpp>
#include <climits>
#include <gmp.h>
#include <iostream>
#include <string>
#include "regina-core.h"
#include "utilities/intutils.h"

/**
 * \hideinitializer
 *
 * An internal copy of the GMP signed comparison optimisations.
 * This macro should not be used outside this class.
 *
 * By making our own copy of such optimisation macros we can use
 * C++-style casts instead of C-style casts and avoid noisy compiler
 * warnings.  I'd love a better way of doing this.
 */
#ifdef __GNUC__
    #define mpz_cmp_si_cpp(z, si) \
        (__builtin_constant_p(si) && (si) == 0 ? mpz_sgn(z) : \
        __builtin_constant_p(si) && (si) > 0 ? _mpz_cmp_ui(z, \
            static_cast<unsigned long>(si)) : \
        _mpz_cmp_si(z, si))
#else
    #define mpz_cmp_si_cpp(z, si) _mpz_cmp_si(z, si)
#endif

namespace regina {

/**
 * \weakgroup maths
 * @{
 */

template <int bytes>
class NNativeInteger;

/**
 * Internal base classes for use with NIntegerBase, templated on whether we
 * should support infinity as an allowed value.
 *
 * See the NIntegerBase class notes for details.
 */
template <bool supportInfinity>
struct InfinityBase;

#ifndef __DOXYGEN
/**
 * An internal base class inherited by NLargeInteger, which provides
 * support for infinity as an allowed value.
 */
template <>
struct InfinityBase<true> {
    bool infinite_;
        /**< Does this integer represent infinity? */

    /**
     * Default constructor that sets this integer to be finite.
     */
    inline InfinityBase() : infinite_(false) {
    }
};

/**
 * An empty internal base class inherited by NInteger, which does not
 * support infinity as an allowed value.
 */
template <>
struct InfinityBase<false> {
};
#endif // __DOXYGEN

/**
 * Represents an arbitrary precision integer.
 * Calculations are always guaranteed to be exact, regardless of how
 * large the integers become.
 *
 * The current implementation uses fast native integer arithmetic wherever
 * possible, whilst always testing for potential overflow.  If a potential
 * overflow is detected, this class switches to using the GNU multiple
 * precision arithmetic library (libgmp) instead.
 *
 * This class takes a single boolean argument \a supportInfinity.
 * If this is \c true, then this class will support infinity as an allowed
 * value.  If this is \c false (the default), then infinity is not supported,
 * and any attempt to work with infinity will lead to undefined behaviour.
 * Supporting infinity is more flexible, but also comes with a slight
 * performance cost (very roughly estimated at around 10%-20%).
 *
 * For the purposes of comparison, infinity is
 * considered larger than any other integer but equal to itself.
 *
 * All routines in this class, including random number generation, are
 * thread-safe.
 *
 * The opportunistic use of native arithmetic where possible was inspired by
 * the (much more complex and powerful) lazy exact arithmetic in CGAL.
 * Thanks to Menelaos Karavelas for encouraging me to take another look at
 * these ideas.
 *
 * \ifacespython Both variants of this template are available through Python.
 * For \a supportInfinity = \c false (the default), simply use NIntegerBase.
 * For \a supportInfinity = \c true, use NLargeInteger.
 *
 * \testfull At present, the only routines not thoroughly tested in the
 * test suite are legendre() and the random number generation routines.
 */
template <bool supportInfinity = false>
class REGINA_API NIntegerBase : private InfinityBase<supportInfinity> {
    public:
        static const NIntegerBase<supportInfinity> zero;
            /**< Globally available zero. */
        static const NIntegerBase<supportInfinity> one;
            /**< Globally available one. */
        static const NIntegerBase<supportInfinity> infinity;
            /**< Globally available infinity.
                 This is only defined if \a supportInfinity is \c true.
                 Any attempt to use it when \a supportInfinity is \c false
                 should generate a linker error. */

    private:
        long small_;
            /**< Contains the native representation of this integer, if
                 we are still using native representations (i.e., if
                 large_ is null).  If we are using GMP large integer
                 representations, or if this integer is infinity, then this
                 native integer is ignored (and may be set to anything). */
        mpz_ptr large_;
            /**< 0 if we are using native representations, or a pointer to the
                 full GMP large integer if we are now using these instead.
                 We require that, whenever this pointer is non-null, the
                 corresponding GMP large integer is initialised.
                 If this integer is infinity then large_ must be null. */

    public:
        /**
         * Initialises this integer to zero.
         */
        NIntegerBase();
        /**
         * Initialises this integer to the given value.
         *
         * \ifacespython In Python, the only native-integer constructor
         * is NIntegerBase(long).
         *
         * @param value the new value of this integer.
         */
        NIntegerBase(int value);
        /**
         * Initialises this integer to the given value.
         *
         * \ifacespython In Python, the only native-integer constructor
         * is NIntegerBase(long).
         *
         * @param value the new value of this integer.
         */
        NIntegerBase(unsigned value);
        /**
         * Initialises this integer to the given value.
         *
         * \ifacespython In Python, this is the only native-integer
         * constructor available.
         *
         * @param value the new value of this integer.
         */
        NIntegerBase(long value);
        /**
         * Initialises this integer to the given value.
         *
         * \ifacespython In Python, the only native-integer constructor
         * is NIntegerBase(long).
         *
         * @param value the new value of this integer.
         */
        NIntegerBase(unsigned long value);
        /**
         * Initialises this integer to the given value.
         *
         * @param value the new value of this integer.
         */
        NIntegerBase(const NIntegerBase<supportInfinity>& value);
        /**
         * Initialises this integer to the given value.
         *
         * This constructor is marked as explicit in the hope of
         * avoiding accidental (and unintentional) mixing of template
         * parameters.
         *
         * \pre The given integer is not infinite.
         *
         * @param value the new value of this integer.
         */
        explicit NIntegerBase(const NIntegerBase<! supportInfinity>& value);
        /**
         * Initialises this integer to the given value.
         *
         * This constructor is marked as explicit in the hope of
         * avoiding accidental (and unintentional) mixing of integer classes.
         *
         * \pre If \a bytes is larger than sizeof(long), then
         * \a bytes is a strict \e multiple of sizeof(long).  For
         * instance, if longs are 8 bytes then you can use \a bytes=16
         * but not \a bytes=12.  This restriction is enforced through a
         * compile-time assertion, but may be lifted in future versions
         * of Regina.
         *
         * \ifacespython Not present.
         *
         * @param value the new value of this integer.
         */
        template <int bytes>
        explicit NIntegerBase(const NNativeInteger<bytes>& value);
        /**
         * Initialises this integer to the given value which is
         * represented as a string of digits in a given base.
         *
         * If not specified, the base defaults to 10.
         * If the given base is zero, the base will be automatically
         * determined.  If the given string begins with \c 0x or \c 0X,
         * the base will be assumed to be 16.  Otherwise, if the string
         * begins with \c 0, the base will be assumed to be 8.
         * Otherwise it will be taken as base 10.
         *
         * Whitespace may be present at the beginning or the end
         * of the given string, and will simply be ignored.
         *
         * Error detection is possible by passing a non-null boolean
         * pointer as the third parameter to this constructor.
         *
         * For finer details on how the string parsing works, see
         * strtol() from the standard C library (on which this method
         * is based).
         *
         * \pre The given base is zero, or is between 2 and 36 inclusive.
         * \pre The given string represents a finite integer
         * in the given base, with optional whitespace beforehand.
         *
         * \ifacespython The final parameter \a valid is not present.
         *
         * @param value the new value of this integer, represented as a string
         * of digits in base \a base.
         * @param base the base in which \a value is given.
         * @param valid if this pointer is not null, the boolean referenced
         * will be set to \c true if the entire given string was a valid
         * large integer representation and \c false otherwise.
         */
        NIntegerBase(const char* value, int base = 10, bool* valid = 0);
        /**
         * Initialises this integer to the given value which is
         * represented as a string of digits in a given base.
         *
         * If not specified, the base defaults to 10.
         * If the given base is zero, the base will be automatically
         * determined.  If the given string begins with \c 0x or \c 0X,
         * the base will be assumed to be 16.  Otherwise, if the string
         * begins with \c 0, the base will be assumed to be 8.
         * Otherwise it will be taken as base 10.
         *
         * Whitespace may be present at the beginning or the end
         * of the given string, and will simply be ignored.
         *
         * Error detection is possible by passing a non-null boolean
         * pointer as the third parameter to this constructor.
         *
         * For finer details on how the string parsing works, see
         * strtol() from the standard C library (on which this method
         * is based).
         *
         * \pre The given base is zero, or is between 2 and 36 inclusive.
         * \pre The given string represents an integer
         * in the given base, with optional whitespace beforehand.
         *
         * \ifacespython The final parameter \a valid is not present.
         *
         * @param value the new value of this integer, represented as a string
         * of digits in base \a base.
         * @param base the base in which \a value is given.
         * @param valid if this pointer is not null, the boolean referenced
         * will be set to \c true if the entire given string was a valid
         * large integer representation and \c false otherwise.
         */
        NIntegerBase(const std::string& value, int base = 10,
                bool* valid = 0);
        /**
         * Destroys this integer.
         */
        ~NIntegerBase();

        /**
         * Returns whether we are currently working with a native C/C++
         * long, or whether we have switched to GMP large integer arithmetic
         * for this integer.
         *
         * If this integer is infinite, this routine will return \c false.
         *
         * @return \c true if and only if we are still using a native
         * C/C++ long.
         */
        bool isNative() const;

        /**
         * Returns whether or not this integer is zero.
         *
         * This is micro-optimised to be faster than simply testing
         * whether (*this) == 0.
         *
         * @return \c true if and only if this integer is zero.
         */
        bool isZero() const;

        /**
         * Returns the sign of this integer.
         *
         * In this routine, infinity is considered to have sign +1.
         *
         * @return +1, -1 or 0 according to whether this integer is
         * positive, negative or zero.
         */
        int sign() const;

        /**
         * Returns whether this integer is infinity.
         *
         * @return \c true if and only if this integer is infinity.
         */
        bool isInfinite() const;

        /**
         * Sets this integer to be infinity.
         *
         * If the template parameter \a supportInfinity is \c false,
         * this routine safely does nothing.
         */
        inline void makeInfinite();

        /**
         * Returns the value of this integer as a long.
         *
         * It is the programmer's reponsibility to ensure that this integer
         * is within the required range.  If this integer is too large or
         * small to fit into a long, then the result will be undefined.
         *
         * Note that, assuming the value is within the required range,
         * this routine will give correct results regardless of whether the
         * underlying representation is a native or large integer.
         *
         * \pre This integer is not infinity.
         *
         * @return the value of this integer.
         */
        long longValue() const;
        /**
         * Returns the value of this integer as a native integer of some
         * fixed byte length.
         *
         * It is the programmer's reponsibility to ensure that this integer
         * is within the required range.  If this integer is too large or
         * small to fit into the return type, then the result will be undefined.
         *
         * Note that, assuming the value is within the required range,
         * this routine will give correct results regardless of whether the
         * underlying representation is a native or large integer.
         *
         * \pre If \a bytes is larger than sizeof(long), then
         * \a bytes is a strict \e multiple of sizeof(long).  For
         * instance, if longs are 8 bytes then you can use this
         * routine with \a bytes=16 but not \a bytes=12.
         * This restriction is enforced through a compile-time assertion,
         * but may be lifted in future versions of Regina.
         *
         * \pre This integer is not infinity.
         *
         * \ifacespython Not present.
         *
         * @return the value of this integer.
         */
        template <int bytes>
        typename IntOfSize<bytes>::type nativeValue() const;
        /**
         * Returns the value of this integer as a string in the given
         * base.  If not specified, the base defaults to 10.
         *
         * If this integer is infinity, the string returned will be
         * \c inf.
         *
         * \pre The given base is between 2 and 36 inclusive.
         *
         * @return the value of this integer as a newly allocated
         * string.
         */
        std::string stringValue(int base = 10) const;

        /**
         * Sets this integer to the given value.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator =(const NIntegerBase& value);
        /**
         * Sets this integer to the given value.
         *
         * \pre The given integer is not infinite.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator = (const NIntegerBase<! supportInfinity>& value);
        /**
         * Sets this integer to the given value.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator =(int value);
        /**
         * Sets this integer to the given value.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator =(unsigned value);
        /**
         * Sets this integer to the given value.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator =(long value);
        /**
         * Sets this integer to the given value.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator =(unsigned long value);
        /**
         * Sets this integer to the given value which is
         * represented as a string of digits in base 10.
         *
         * Whitespace may be present at the beginning or end of the given
         * string and will simply be ignored.
         *
         * \pre The given string represents an integer
         * in base 10, with optional whitespace added.
         *
         * @param value the new value of this integer, represented as a string
         * of digits in base 10.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator =(const char* value);
        /**
         * Sets this integer to the given value which is
         * represented as a string of digits in base 10.
         *
         * Whitespace may be present at the beginning or end of the given
         * string and will simply be ignored.
         *
         * \pre The given string represents an integer
         * in base 10, with optional whitespace added.
         *
         * @param value the new value of this integer, represented as a string
         * of digits in base 10.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator =(const std::string& value);
        /**
         * Swaps the values of this and the given integer.
         *
         * @param other the integer whose value will be swapped with
         * this.
         */
        void swap(NIntegerBase& other);

        /**
         * Determines if this is equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * equal.
         */
        bool operator ==(const NIntegerBase& rhs) const;
        /**
         * Determines if this is equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * equal.
         */
        bool operator ==(const NIntegerBase<! supportInfinity>& rhs) const;
        /**
         * Determines if this is equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * equal.
         */
        bool operator ==(long rhs) const;
        /**
         * Determines if this is not equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * not equal.
         */
        bool operator !=(const NIntegerBase& rhs) const;
        /**
         * Determines if this is not equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * not equal.
         */
        bool operator !=(const NIntegerBase<! supportInfinity>& rhs) const;
        /**
         * Determines if this is not equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * not equal.
         */
        bool operator !=(long rhs) const;
        /**
         * Determines if this is less than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than the given
         * integer.
         */
        bool operator <(const NIntegerBase& rhs) const;
        /**
         * Determines if this is less than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than the given
         * integer.
         */
        bool operator <(long rhs) const;
        /**
         * Determines if this is greater than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than the given
         * integer.
         */
        bool operator >(const NIntegerBase& rhs) const;
        /**
         * Determines if this is greater than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than the given
         * integer.
         */
        bool operator >(long rhs) const;
        /**
         * Determines if this is less than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than or equal to
         * the given integer.
         */
        bool operator <=(const NIntegerBase& rhs) const;
        /**
         * Determines if this is less than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than or equal to
         * the given integer.
         */
        bool operator <=(long rhs) const;
        /**
         * Determines if this is greater than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than or equal
         * to the given integer.
         */
        bool operator >=(const NIntegerBase& rhs) const;
        /**
         * Determines if this is greater than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than or equal
         * to the given integer.
         */
        bool operator >=(long rhs) const;

        /**
         * The preincrement operator.
         * This operator increments this integer by one, and returns a
         * reference to the integer \e after the increment.
         *
         * \ifacespython Not available.
         *
         * @return a reference to this integer after the increment.
         */
        NIntegerBase& operator ++();

        /**
         * The postincrement operator.
         * This operator increments this integer by one, and returns a
         * copy of the integer \e before the increment.
         *
         * \ifacespython Not available.
         *
         * @return a copy of this integer before the
         * increment took place.
         */
        NIntegerBase operator ++(int);

        /**
         * The predecrement operator.
         * This operator decrements this integer by one, and returns a
         * reference to the integer \e after the decrement.
         *
         * \ifacespython Not available.
         *
         * @return a reference to this integer after the decrement.
         */
        NIntegerBase& operator --();

        /**
         * The postdecrement operator.
         * This operator decrements this integer by one, and returns a
         * copy of the integer \e before the decrement.
         *
         * \ifacespython Not available.
         *
         * @return a copy of this integer before the
         * decrement took place.
         */
        NIntegerBase operator --(int);

        /**
         * Adds this to the given integer and returns the result.
         * This integer is not changed.
         *
         * If either term of the sum is infinite, the result will be
         * infinity.
         *
         * @param other the integer to add to this integer.
         * @return the sum \a this plus \a other.
         */
        NIntegerBase operator +(const NIntegerBase& other) const;
        /**
         * Adds this to the given integer and returns the result.
         * This integer is not changed.
         *
         * If either term of the sum is infinite, the result will be
         * infinity.
         *
         * @param other the integer to add to this integer.
         * @return the sum \a this plus \a other.
         */
        NIntegerBase operator +(long other) const;
        /**
         * Subtracts the given integer from this and returns the result.
         * This integer is not changed.
         *
         * If either term of the difference is infinite, the result will be
         * infinity.
         *
         * @param other the integer to subtract from this integer.
         * @return the difference \a this minus \a other.
         */
        NIntegerBase operator -(const NIntegerBase& other) const;
        /**
         * Subtracts the given integer from this and returns the result.
         * This integer is not changed.
         *
         * If either term of the difference is infinite, the result will be
         * infinity.
         *
         * @param other the integer to subtract from this integer.
         * @return the difference \a this minus \a other.
         */
        NIntegerBase operator -(long other) const;
        /**
         * Multiplies this by the given integer and returns the
         * result.
         * This integer is not changed.
         *
         * If either factor of the product is infinite, the result will be
         * infinity.
         *
         * @param other the integer to multiply by this integer.
         * @return the product \a this times \a other.
         */
        NIntegerBase operator *(const NIntegerBase& other) const;
        /**
         * Multiplies this by the given integer and returns the
         * result.
         * This integer is not changed.
         *
         * If either factor of the product is infinite, the result will be
         * infinity.
         *
         * @param other the integer to multiply by this integer.
         * @return the product \a this times \a other.
         */
        NIntegerBase operator *(long other) const;
        /**
         * Divides this by the given integer and returns the result.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is not changed.
         *
         * If \a other is known to divide this integer exactly,
         * divExact() should be used instead.
         *
         * Infinity divided by anything will return infinity.
         * Anything finite divided by infinity will return zero.
         * Anything finite divided by zero will return infinity.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre If this class does not support infinity, then
         * \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NIntegerBase operator /(const NIntegerBase& other) const;
        /**
         * Divides this by the given integer and returns the result.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is not changed.
         *
         * If \a other is known to divide this integer exactly,
         * divExact() should be used instead.
         *
         * Infinity divided by anything will return infinity.
         * Anything finite divided by zero will return infinity.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre If this class does not support infinity, then
         * \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NIntegerBase operator /(long other) const;
        /**
         * Divides this by the given integer and returns the result.
         * This can only be used when the given integer divides into
         * this exactly, and for large integers can be much faster than
         * ordinary division.  This integer is not changed.
         *
         * \pre The given integer divides exactly into
         * this integer, i.e. \a this divided by \a other is an integer.
         * \pre \a other is not zero.
         * \pre Neither this nor \a other is infinite.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NIntegerBase divExact(const NIntegerBase& other) const;
        /**
         * Divides this by the given integer and returns the result.
         * This can only be used when the given integer divides into
         * this exactly, and for large integers can be much faster than
         * ordinary division.  This integer is not changed.
         *
         * \pre The given integer divides exactly into
         * this integer, i.e. \a this divided by \a other is an integer.
         * \pre \a other is not zero.
         * \pre This integer is not infinite.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NIntegerBase divExact(long other) const;
        /**
         * Determines the remainder when this integer is divided by the
         * given integer.  If non-zero, the result will have the same sign
         * as this integer.
         * This integer is not changed.
         *
         * For a division routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         * \pre Neither this nor \a other is infinite.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer to divide this by.
         * @return the remainder \a this modulo \a other.
         */
        NIntegerBase operator %(const NIntegerBase& other) const;
        /**
         * Determines the remainder when this integer is divided by the
         * given integer.  If non-zero, the result will have the same sign
         * as this integer.
         * This integer is not changed.
         *
         * For a division routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         * \pre This integer is not infinite.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer to divide this by.
         * @return the remainder \a this modulo \a other.
         */
        NIntegerBase operator %(long other) const;

        /**
         * Uses the division algorithm to obtain a quotient and
         * remainder when dividing by the given integer.
         *
         * Suppose this integer is \a n and we pass the divisor \a d.
         * The <em>division algorithm</em> describes the result of
         * dividing \a n by \a d; in particular, it expresses
         * <tt>n = qd + r</tt>, where \a q is the quotient and
         * \a r is the remainder.
         *
         * The division algorithm is precise about which values of \a q
         * and \a r are chosen; in particular it chooses the unique \a r
         * in the range <tt>0 <= r < |d|</tt>.
         *
         * Note that this differs from other division routines in this
         * class, in that it always rounds to give a non-negative remainder.
         * Thus NIntegerBase(-7).divisionAlg(3) gives quotient -3 and
         * remainder 2, whereas (-7)/3 gives quotient -2 and (-7)\%3 gives
         * remainder -1.
         *
         * The two results are passed back to the caller as follows:
         * The quotient \a q is passed back as the return value of the
         * function, and the remainder \a r is stored in the reference
         * argument \a r.
         *
         * In the special case where the given divisor is 0 (not
         * allowed by the usual division algorithm), this routine selects
         * quotient 0 and remainder \a n.
         *
         * \pre Neither this nor the divisor are infinite.
         *
         * \ifacespython The argument \a remainder is missing; instead both
         * the quotient and remainder are passed back through the return
         * value of the function.  Specifically, this function returns a
         * (\a quotient, \a remainder) pair.
         *
         * @param divisor the divisor \a d.
         * @param remainder used to store the remainder \a r when the
         * functon returns.  The initial value of this argument is ignored.
         * @return the quotient \a q.
         *
         * @author Ryan Budney & B.B.
         */
        NIntegerBase<supportInfinity> divisionAlg(
                const NIntegerBase<supportInfinity>& divisor,
                NIntegerBase<supportInfinity>& remainder) const;

        /**
         * Determines the negative of this integer.
         * This integer is not changed.
         *
         * Negative infinity will return infinity.
         *
         * @return the negative of this integer.
         */
        NIntegerBase operator -() const;

        /**
         * Adds the given integer to this.
         * This integer is changed to reflect the result.
         *
         * If either term of the sum is infinite, the result will be
         * infinity.
         *
         * @param other the integer to add to this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator +=(const NIntegerBase& other);
        /**
         * Adds the given integer to this.
         * This integer is changed to reflect the result.
         *
         * If either term of the sum is infinite, the result will be
         * infinity.
         *
         * @param other the integer to add to this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator +=(long other);
        /**
         * Subtracts the given integer from this.
         * This integer is changed to reflect the result.
         *
         * If either term of the difference is infinite, the result will be
         * infinity.
         *
         * @param other the integer to subtract from this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator -=(const NIntegerBase& other);
        /**
         * Subtracts the given integer from this.
         * This integer is changed to reflect the result.
         *
         * If either term of the difference is infinite, the result will be
         * infinity.
         *
         * @param other the integer to subtract from this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator -=(long other);
        /**
         * Multiplies the given integer by this.
         * This integer is changed to reflect the result.
         *
         * If either factor of the product is infinite, the result will be
         * infinity.
         *
         * @param other the integer to multiply with this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator *=(const NIntegerBase& other);
        /**
         * Multiplies the given integer by this.
         * This integer is changed to reflect the result.
         *
         * If either factor of the product is infinite, the result will be
         * infinity.
         *
         * @param other the integer to multiply with this integer.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator *=(long other);
        /**
         * Divides this by the given integer.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is changed to reflect the result.
         *
         * If \a other is known to divide this integer exactly,
         * divByExact() should be used instead.
         *
         * Infinity divided by anything will return infinity.
         * Anything finite divided by infinity will return zero.
         * Anything finite divided by zero will return infinity.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre If this class does not support infinity, then
         * \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator /=(const NIntegerBase& other);
        /**
         * Divides this by the given integer.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is changed to reflect the result.
         *
         * If \a other is known to divide this integer exactly,
         * divByExact() should be used instead.
         *
         * Infinity divided by anything will return infinity.
         * Anything finite divided by zero will return infinity.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre If this class does not support infinity, then
         * \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator /=(long other);
        /**
         * Divides this by the given integer.
         * This can only be used when the given integer divides into
         * this exactly, and for large integers this is much faster than
         * ordinary division.  This integer is changed to reflect the result.
         *
         * \pre The given integer divides exactly into
         * this integer, i.e. \a this divided by \a other is an integer.
         * \pre \a other is not zero.
         * \pre Neither this nor \a other is infinite.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& divByExact(const NIntegerBase& other);
        /**
         * Divides this by the given integer.
         * This can only be used when the given integer divides into
         * this exactly, and for large integers this is much faster than
         * ordinary division.  This integer is changed to reflect the result.
         *
         * \pre The given integer divides exactly into
         * this integer, i.e. \a this divided by \a other is an integer.
         * \pre \a other is not zero.
         * \pre This integer is not infinite.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& divByExact(long other);
        /**
         * Reduces this integer modulo the given integer.
         * If non-zero, the result will have the same sign as the original
         * value of this integer.
         * This integer is changed to reflect the result.
         *
         * For a mod routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         * \pre Neither this nor \a other is infinite.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer modulo which this integer will be
         * reduced.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator %=(const NIntegerBase& other);
        /**
         * Reduces this integer modulo the given integer.
         * If non-zero, the result will have the same sign as the original
         * value of this integer.
         * This integer is changed to reflect the result.
         *
         * For a mod routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         * \pre This integer is not infinite.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer modulo which this integer will be
         * reduced.
         * @return a reference to this integer with its new value.
         */
        NIntegerBase& operator %=(long other);
        /**
         * Negates this integer.
         * This integer is changed to reflect the result.
         *
         * Negating infinity will result in infinity.
         */
        void negate();
        /**
         * Raises this integer to the power of the given exponent.
         * This integer is changed to reflect the result.
         *
         * Note that 0 to the power of 0 will be 1, infinity to the
         * power of 0 will be 1, and infinity to the power of anything
         * else will be infinity.
         *
         * \pre The given exponent is non-negative.
         *
         * @param exp the power to which this integer will be raised.
         */
        void raiseToPower(unsigned long exp);

        /**
         * Determines the absolute value of this integer.
         * This integer is not changed.
         *
         * @return the absolute value of this integer.
         */
        NIntegerBase abs() const;
        /**
         * Sets this integer to be the greatest common divisor of this
         * and the given integer.
         *
         * The result is guaranteed to be non-negative.  As a
         * special case, gcd(0,0) is considered to be zero.
         *
         * \pre Neither this integer nor \a other is infinite.
         *
         * @param other the integer whose greatest common divisor with
         * this will be found.
         */
        void gcdWith(const NIntegerBase& other);
        /**
         * Determines the greatest common divisor of this and the given
         * integer.  This integer is not changed.
         *
         * The result is guaranteed to be non-negative.  As a
         * special case, gcd(0,0) is considered to be zero.
         *
         * \pre Neither this integer nor \a other is infinite.
         *
         * @param other the integer whose greatest common divisor with
         * this will be found.
         * @return the greatest common divisor of this and the given
         * integer.
         */
        NIntegerBase gcd(const NIntegerBase& other) const;
        /**
         * Sets this integer to be the lowest common multiple of this
         * and the given integer.
         *
         * Note that the result might possibly be negative.
         *
         * \pre Neither this integer nor \a other is infinite.
         *
         * @param other the integer whose lowest common multiple with
         * this will be found.
         */
        void lcmWith(const NIntegerBase& other);
        /**
         * Determines the lowest common multiple of this and the given
         * integer.  This integer is not changed.
         *
         * Note that the result might possibly be negative.
         *
         * \pre Neither this integer nor \a other is infinite.
         *
         * @param other the integer whose lowest common multiple with
         * this will be found.
         * @return the lowest common multiple of this and the given
         * integer.
         */
        NIntegerBase lcm(const NIntegerBase& other) const;

        /**
         * Determines the greatest common divisor of this and the given
         * integer and finds the smallest coefficients with which these
         * integers combine to give their gcd.
         *
         * Note that the given integers need not be non-negative.
         * However, the gcd returned is guaranteed to be non-negative.
         *
         * If \a d is the gcd of \a this and \a other, the values placed
         * into \a u and \a v will be those for which
         * <tt>u*this + v*other = d</tt>,
         * <tt>-abs(this)/d < v*sign(other) <= 0</tt> and
         * <tt>1 <= u*sign(this) <= abs(other)/d</tt>.
         * These equations are not satisfied when either of \a this or
         * \a other are zero, but in this case \a u and \a v are both
         * 0, 1 or -1, using as many zeros as possible.
         *
         * \pre Neither this integer nor \a other is infinite.
         *
         * @param other the integer whose greatest common divisor with
         * this will be found.
         * @param u a variable into which the final coefficient of
         * \a this will be placed.
         * @param v a variable into which the final coefficient of
         * \a other will be placed.
         * @return the greatest common divisor of \a this and \a other.
         */
        NIntegerBase<supportInfinity> gcdWithCoeffs(
            const NIntegerBase<supportInfinity>& other,
            NIntegerBase<supportInfinity>& u,
            NIntegerBase<supportInfinity>& v) const;

        /**
         * Returns the Legendre symbol (\a a/\a p), where
         * \a a is this integer and \a p is an odd prime.
         *
         * The Legendre symbol is equal to 0 if this integer
         * is divisible by \a p, 1 if this integer is congruent
         * to a square mod \a p (but not divisible by \a p),
         * and -1 otherwise.
         *
         * \pre The given integer \a p is an odd positive prime.
         * \pre This integer is not infinite.
         *
         * @param p the given odd prime.
         * @return The Legendre symbol (0, 1 or -1) as described above.
         *
         * @author Ryan Budney
         */
        int legendre(const NIntegerBase<supportInfinity>& p) const;

        /**
         * Generate a pseudo-random integer that is uniformly
         * distributed in the interval [0,*this).
         *
         * \pre This integer is strictly positive.
         *
         * \warning Even if this integer is small, this routine is still
         * slow - it always goes through the GMP large integer routines
         * so that the random number generation algorithm is consistent.
         * If you need a fast random number generator and this integer
         * is small, consider using the standard rand() function instead.
         *
         * @return a pseudo-random integer.
         */
        NIntegerBase<supportInfinity> randomBoundedByThis() const;

        /**
         * Generate a pseudo-random integer that is uniformly
         * distributed in the interval [0,2^n).
         *
         * @param n the maximum number of bits in the pseudo-random
         * integer.
         * @return a pseudo-random integer.
         */
        static NIntegerBase<supportInfinity> randomBinary(unsigned long n);

        /**
         * Generate a pseudo-random integer that is distributed in the
         * interval [0,2^n), with a tendency to have long strings of 0s
         * and 1s in its binary expansion.
         *
         * @param n the maximum number of bits in the pseudo-random integer.
         * @return a pseudo-random integer.
         */
        static NIntegerBase<supportInfinity> randomCornerBinary(
            unsigned long n);

        /**
         * Set this to a copy of the given raw GMP integer.
         *
         * This routine allows NIntegerBase to interact directly with
         * libgmp and libgmpxx if necessary.
         *
         * \ifacespython Not available.
         *
         * @param fromData the raw GMP integer to clone.
         */
        void setRaw(mpz_srcptr fromData);

        /**
         * Returns the raw GMP data that describes this integer.
         *
         * This routine allows NIntegerBase to interact directly with
         * libgmp and libgmpxx if necessary.
         *
         * \warning This routine will have the side-effect of converting
         * this integer to a (bulkier and slower) GMP representation,
         * regardless of whether it is small enough to fit within a native
         * integer.  Excessive use of this routine could lead to a significant
         * performance loss.  It is best to use this only when isNative() is
         * already known to return \c false.
         *
         * \pre This integer is not infinite.
         *
         * \ifacespython Not available.
         *
         * @return the raw GMP data.
         */
        mpz_srcptr rawData() const;

        /**
         * Returns the raw GMP data that describes this integer.
         *
         * This routine allows NIntegerBase to interact directly with
         * libgmp and libgmpxx if necessary.
         *
         * \warning This routine will have the side-effect of converting
         * this integer to a (bulkier and slower) GMP representation,
         * regardless of whether it is small enough to fit within a native
         * integer.  Excessive use of this routine could lead to a significant
         * performance loss.  It is best to use this only when isNative() is
         * already known to return \c false.
         *
         * \pre This integer is not infinite.
         *
         * \ifacespython Not available.
         *
         * @return the raw GMP data.
         */
        mpz_ptr rawData();

        /**
         * Converts this integer to use a GMP large integer representation,
         * regardless of whether this is actually necessary.  The contents
         * of this integer will be preserved.
         *
         * It does not matter which kind of representation this integer
         * is currently using.
         *
         * \pre This integer is not infinite.
         */
        void makeLarge();

        /**
         * Converts this integer to use a native C/C++ long representation,
         * if this is possible.  However, if this integer is outside the range
         * of a C/C++ long, then it will remain as a GMP large integer instead
         * (i.e., nothing will change).  Whatever happens, the contents of this
         * integer will be preserved.
         *
         * It does not matter which kind of representation this integer
         * is currently using.
         *
         * \pre This integer is not infinite.
         */
        void tryReduce();

    private:
        /**
         * Initialises this integer to infinity.
         * All parameters are ignored.
         *
         * This constructor is only defined if \a supportInfinity is \c true.
         * Any attempt to use it when \a supportInfinity is \c false
         * will generate a linker error.
         */
        NIntegerBase(bool, bool);

        /**
         * Sets this integer to be finite.
         * Its new value will be determined by the current contents of
         * \a small_ which will not be touched.
         *
         * If the template parameter \a supportInfinity is \c false,
         * this routine safely does nothing.
         */
        inline void makeFinite();

        /**
         * Converts this integer from a native C/C++ long representation
         * into a GMP large integer representation.
         *
         * The contents of \a small will be copied into \a large.
         *
         * \pre \a large_ is null (i.e., we are indeed using a native
         * C/C++ long representation at present).
         * \pre This integer is not infinite.
         */
        void forceLarge();

        /**
         * Destroys the GMP large integer representation and reverts to
         * a native C/C++ long.
         *
         * The new value of this integer will be the current contents of
         * \a small_ (i.e., there is no attempt to "extract" a native long
         * from the contents of \a large_).
         *
         * \pre \a large_ is non-null (i.e., we are indeed using a large
         * integer reprentation at present).
         * \pre This integer is not infinite.
         */
        void clearLarge();

        /**
         * Converts this integer from a GMP large integer representation
         * into a native C/C++ long representation.
         *
         * The contents of \a large will be extracted and copied into \a small.
         *
         * \pre \a large_ is non-null, and the large integer that it
         * represents lies between LONG_MIN and LONG_MAX inclusive.
         * \pre This integer is not infinite.
         */
        void forceReduce();

    friend class NIntegerBase<! supportInfinity>; // For conversions.

    template <int bytes>
    friend class NNativeInteger; // For conversions.

    template <bool supportInfinity_>
    friend std::ostream& operator << (std::ostream& out,
        const NIntegerBase<supportInfinity_>& large);
};

/**
 * NLargeInteger is a typedef for NIntegerBase<true>, which offers
 * arbitrary precision integers with support for infinity.
 *
 * \ifacespython This typedef is available in Python.
 */
typedef NIntegerBase<true> NLargeInteger;

/**
 * NInteger is a typedef for NIntegerBase<false>, which offers
 * arbitrary precision integers without support for infinity.
 *
 * \ifacespython This typedef is available in Python.
 */
typedef NIntegerBase<false> NInteger;

/**
 * Writes the given integer to the given output stream.
 *
 * @param out the output stream to which to write.
 * @param i the integer to write.
 * @return a reference to \a out.
 */
template <bool supportInfinity>
REGINA_API std::ostream& operator << (std::ostream& out,
    const NIntegerBase<supportInfinity>& i);

/**
 * Adds the given native integer to the given large integer.
 * If the large integer is infinite, the result will also be infinity.
 *
 * \ifacespython Not available.
 *
 * @param lhs the native integer to add.
 * @param rhs the large integer to add.
 * @return the sum \a lhs plus \a rhs.
 */
template <bool supportInfinity>
REGINA_API NIntegerBase<supportInfinity> operator + (long lhs,
    const NIntegerBase<supportInfinity>& rhs);

/**
 * Multiplies the given native integer with the given large integer.
 * If the large integer is infinite, the result will also be infinity.
 *
 * \ifacespython Not available.
 *
 * @param lhs the native integer to multiply.
 * @param rhs the large integer to multiply.
 * @return the product \a lhs times \a rhs.
 */
template <bool supportInfinity>
REGINA_API NIntegerBase<supportInfinity> operator * (long lhs,
    const NIntegerBase<supportInfinity>& rhs);

/*@}*/

} // namespace regina

#ifndef __DOXYGEN
namespace libnormaliz {
/**
 * Explicit integer cast functions, for compatibility with Normaliz.
 *
 * We define functions separately for each variant of \a supportInfinity
 * to avoid partial template specialisation.
 *
 * @param a an instance of some arbitrary integer type.
 * @return the given integer, cast as a native long.
 */
template <typename Integer>
long explicit_cast_to_long(const Integer& a);

template <>
inline long explicit_cast_to_long<regina::NIntegerBase<true> >(
        const regina::NIntegerBase<true>& a) {
    return a.longValue();
}
template <>
inline long explicit_cast_to_long<regina::NIntegerBase<false> >(
        const regina::NIntegerBase<false>& a) {
    return a.longValue();
}
} //namespace libnormaliz
#endif

namespace regina {

/**
 * A wrapper class for a native, fixed-precision integer type of the
 * given size.
 *
 * This class behaves just like native integer arithmetic, where the
 * underlying integer type is signed and stores the given number of bytes.
 * There is no overflow testing, and it is up to the user to ensure that
 * overflows do not occur.  On the other hand, this class is almost as
 * fast as native integer arithmetic (i.e., there is very little overhead).
 *
 * The reason for using this class, instead of working directly in a native
 * integer type, is that this class offers an interface that is compatible with
 * NInteger.  Only some of the NInteger member functions are offered here;
 * however, those that are offered behave just like their NInteger
 * counterparts (with the single exception that all arithmetic in
 * NNativeInteger is subject to overflow).  Developers can therefore
 * switch between integer types easily with minimal changes to
 * their code, or support both NInteger and NNativeInteger types as
 * template arguments.
 *
 * \pre The system must support integers of the given size; in particular,
 * there must be an appropriate specialisation IntOfSize<bytes>.
 *
 * \ifacespython Not present.
 */
template <int bytes>
class REGINA_API NNativeInteger {
    public:
        typedef typename IntOfSize<bytes>::type Native;
            /**< The native data type used to store this integer. */

    private:
        Native data_;
            /**< The value of this integer. */

    public:
        /**
         * Initialises this integer to zero.
         */
        NNativeInteger();
        /**
         * Initialises this integer to the given value.
         *
         * @param value the new value of this integer.
         */
        NNativeInteger(Native value);
        /**
         * Initialises this integer to the given value.
         *
         * @param value the new value of this integer.
         */
        NNativeInteger(const NNativeInteger<bytes>& value);
        /**
         * Initialises this integer to the given value.
         *
         * This constructor is marked as explicit in the hope of
         * avoiding accidental (and unintentional) mixing of integer classes.
         *
         * It is the programmer's reponsibility to ensure that the given value
         * fits within the required range.  If the given value is too large or
         * small to fit into this native type, then this new NNativeInteger
         * will have an undefined initial value.
         *
         * \pre If \a bytes is larger than sizeof(long), then
         * \a bytes is a strict \e multiple of sizeof(long).  For
         * instance, if longs are 8 bytes then you can use this
         * routine with \a bytes=16 but not \a bytes=12.
         * This restriction is enforced through a compile-time assertion,
         * but may be lifted in future versions of Regina.
         *
         * \pre The given integer is not infinity.
         *
         * \ifacespython Not present.
         *
         * @param value the new value of this integer.
         */
        template <bool supportInfinity>
        explicit NNativeInteger(const NIntegerBase<supportInfinity>& value);

        /**
         * Returns whether or not this integer is zero.
         *
         * @return \c true if and only if this integer is zero.
         */
        bool isZero() const;

        /**
         * Returns the sign of this integer.
         *
         * @return +1, -1 or 0 according to whether this integer is
         * positive, negative or zero.
         */
        int sign() const;
        /**
         * Returns the value of this integer in its native type.
         *
         * @return the value of this integer.
         */
        Native nativeValue() const;

        /**
         * Sets this integer to the given value.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator =(const NNativeInteger& value);
        /**
         * Sets this integer to the given value.
         *
         * @param value the new value of this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator =(Native value);
        /**
         * Swaps the values of this and the given integer.
         *
         * @param other the integer whose value will be swapped with
         * this.
         */
        void swap(NNativeInteger& other);

        /**
         * Determines if this is equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * equal.
         */
        bool operator ==(const NNativeInteger& rhs) const;
        /**
         * Determines if this is equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * equal.
         */
        bool operator ==(Native rhs) const;
        /**
         * Determines if this is not equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * not equal.
         */
        bool operator !=(const NNativeInteger& rhs) const;
        /**
         * Determines if this is not equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this and the given integer are
         * not equal.
         */
        bool operator !=(Native rhs) const;
        /**
         * Determines if this is less than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than the given
         * integer.
         */
        bool operator <(const NNativeInteger& rhs) const;
        /**
         * Determines if this is less than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than the given
         * integer.
         */
        bool operator <(Native rhs) const;
        /**
         * Determines if this is greater than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than the given
         * integer.
         */
        bool operator >(const NNativeInteger& rhs) const;
        /**
         * Determines if this is greater than the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than the given
         * integer.
         */
        bool operator >(Native rhs) const;
        /**
         * Determines if this is less than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than or equal to
         * the given integer.
         */
        bool operator <=(const NNativeInteger& rhs) const;
        /**
         * Determines if this is less than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is less than or equal to
         * the given integer.
         */
        bool operator <=(Native rhs) const;
        /**
         * Determines if this is greater than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than or equal
         * to the given integer.
         */
        bool operator >=(const NNativeInteger& rhs) const;
        /**
         * Determines if this is greater than or equal to the given integer.
         *
         * @param rhs the integer with which this will be compared.
         * @return \c true if and only if this is greater than or equal
         * to the given integer.
         */
        bool operator >=(Native rhs) const;

        /**
         * The preincrement operator.
         * This operator increments this integer by one, and returns a
         * reference to the integer \e after the increment.
         *
         * \ifacespython Not available.
         *
         * @return a reference to this integer after the increment.
         */
        NNativeInteger& operator ++();

        /**
         * The postincrement operator.
         * This operator increments this integer by one, and returns a
         * copy of the integer \e before the increment.
         *
         * \ifacespython Not available.
         *
         * @return a copy of this integer before the
         * increment took place.
         */
        NNativeInteger operator ++(int);

        /**
         * The predecrement operator.
         * This operator decrements this integer by one, and returns a
         * reference to the integer \e after the decrement.
         *
         * \ifacespython Not available.
         *
         * @return a reference to this integer after the decrement.
         */
        NNativeInteger& operator --();

        /**
         * The postdecrement operator.
         * This operator decrements this integer by one, and returns a
         * copy of the integer \e before the decrement.
         *
         * \ifacespython Not available.
         *
         * @return a copy of this integer before the
         * decrement took place.
         */
        NNativeInteger operator --(int);

        /**
         * Adds this to the given integer and returns the result.
         * This integer is not changed.
         *
         * @param other the integer to add to this integer.
         * @return the sum \a this plus \a other.
         */
        NNativeInteger operator +(const NNativeInteger& other) const;
        /**
         * Adds this to the given integer and returns the result.
         * This integer is not changed.
         *
         * @param other the integer to add to this integer.
         * @return the sum \a this plus \a other.
         */
        NNativeInteger operator +(Native other) const;
        /**
         * Subtracts the given integer from this and returns the result.
         * This integer is not changed.
         *
         * @param other the integer to subtract from this integer.
         * @return the difference \a this minus \a other.
         */
        NNativeInteger operator -(const NNativeInteger& other) const;
        /**
         * Subtracts the given integer from this and returns the result.
         * This integer is not changed.
         *
         * @param other the integer to subtract from this integer.
         * @return the difference \a this minus \a other.
         */
        NNativeInteger operator -(Native other) const;
        /**
         * Multiplies this by the given integer and returns the
         * result.
         * This integer is not changed.
         *
         * @param other the integer to multiply by this integer.
         * @return the product \a this times \a other.
         */
        NNativeInteger operator *(const NNativeInteger& other) const;
        /**
         * Multiplies this by the given integer and returns the
         * result.
         * This integer is not changed.
         *
         * @param other the integer to multiply by this integer.
         * @return the product \a this times \a other.
         */
        NNativeInteger operator *(Native other) const;
        /**
         * Divides this by the given integer and returns the result.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is not changed.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NNativeInteger operator /(const NNativeInteger& other) const;
        /**
         * Divides this by the given integer and returns the result.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is not changed.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NNativeInteger operator /(Native other) const;
        /**
         * Divides this by the given integer and returns the result.
         * For native integers, this is identical to operator /.
         *
         * \pre \a other is not zero.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NNativeInteger divExact(const NNativeInteger& other) const;
        /**
         * Divides this by the given integer and returns the result.
         * For native integers, this is identical to operator /.
         *
         * \pre \a other is not zero.
         *
         * @param other the integer to divide this by.
         * @return the quotient \a this divided by \a other.
         */
        NNativeInteger divExact(Native other) const;
        /**
         * Determines the remainder when this integer is divided by the
         * given integer.  If non-zero, the result will have the same sign
         * as this integer.
         * This integer is not changed.
         *
         * For a division routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer to divide this by.
         * @return the remainder \a this modulo \a other.
         */
        NNativeInteger operator %(const NNativeInteger& other) const;
        /**
         * Determines the remainder when this integer is divided by the
         * given integer.  If non-zero, the result will have the same sign
         * as this integer.
         * This integer is not changed.
         *
         * For a division routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer to divide this by.
         * @return the remainder \a this modulo \a other.
         */
        NNativeInteger operator %(Native other) const;

        /**
         * Uses the division algorithm to obtain a quotient and
         * remainder when dividing by the given integer.
         *
         * Suppose this integer is \a n and we pass the divisor \a d.
         * The <em>division algorithm</em> describes the result of
         * dividing \a n by \a d; in particular, it expresses
         * <tt>n = qd + r</tt>, where \a q is the quotient and
         * \a r is the remainder.
         *
         * The division algorithm is precise about which values of \a q
         * and \a r are chosen; in particular it chooses the unique \a r
         * in the range <tt>0 <= r < |d|</tt>.
         *
         * Note that this differs from other division routines in this
         * class, in that it always rounds to give a non-negative remainder.
         * Thus NNativeInteger(-7).divisionAlg(3) gives quotient -3 and
         * remainder 2, whereas (-7)/3 gives quotient -2 and (-7)\%3 gives
         * remainder -1.
         *
         * The two results are passed back to the caller as follows:
         * The quotient \a q is passed back as the return value of the
         * function, and the remainder \a r is stored in the reference
         * argument \a r.
         *
         * In the special case where the given divisor is 0 (not
         * allowed by the usual division algorithm), this routine selects
         * quotient 0 and remainder \a n.
         *
         * @param divisor the divisor \a d.
         * @param remainder used to store the remainder \a r when the
         * functon returns.  The initial value of this argument is ignored.
         * @return the quotient \a q.
         *
         * @author Ryan Budney & B.B.
         */
        NNativeInteger<bytes> divisionAlg(
                const NNativeInteger<bytes>& divisor,
                NNativeInteger<bytes>& remainder) const;

        /**
         * Determines the negative of this integer.
         * This integer is not changed.
         *
         * @return the negative of this integer.
         */
        NNativeInteger operator -() const;

        /**
         * Adds the given integer to this.
         * This integer is changed to reflect the result.
         *
         * @param other the integer to add to this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator +=(const NNativeInteger& other);
        /**
         * Adds the given integer to this.
         * This integer is changed to reflect the result.
         *
         * @param other the integer to add to this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator +=(Native other);
        /**
         * Subtracts the given integer from this.
         * This integer is changed to reflect the result.
         *
         * @param other the integer to subtract from this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator -=(const NNativeInteger& other);
        /**
         * Subtracts the given integer from this.
         * This integer is changed to reflect the result.
         *
         * @param other the integer to subtract from this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator -=(Native other);
        /**
         * Multiplies the given integer by this.
         * This integer is changed to reflect the result.
         *
         * @param other the integer to multiply with this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator *=(const NNativeInteger& other);
        /**
         * Multiplies the given integer by this.
         * This integer is changed to reflect the result.
         *
         * @param other the integer to multiply with this integer.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator *=(Native other);
        /**
         * Divides this by the given integer.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is changed to reflect the result.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator /=(const NNativeInteger& other);
        /**
         * Divides this by the given integer.
         * The result will be truncated to an integer, i.e. rounded
         * towards zero.
         * This integer is changed to reflect the result.
         *
         * For a division routine that always rounds down, see divisionAlg().
         *
         * \pre \a other must be non-zero.
         *
         * \warning As I understand it, the direction of rounding for
         * native C/C++ integer division was fixed in the C++11
         * specification, but left to the compiler implementation in
         * earlier versions of the specification; however, any modern
         * hardware should satisfy the C++11 rounding rule as described above.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator /=(Native other);
        /**
         * Divides this by the given integer.
         * For native integers, this routine is identical to operator /=.
         *
         * \pre \a other is not zero.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& divByExact(const NNativeInteger& other);
        /**
         * Divides this by the given integer.
         * For native integers, this routine is identical to operator /=.
         *
         * \pre \a other is not zero.
         *
         * @param other the integer to divide this by.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& divByExact(Native other);
        /**
         * Reduces this integer modulo the given integer.
         * If non-zero, the result will have the same sign as the original
         * value of this integer.
         * This integer is changed to reflect the result.
         *
         * For a mod routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer modulo which this integer will be
         * reduced.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator %=(const NNativeInteger& other);
        /**
         * Reduces this integer modulo the given integer.
         * If non-zero, the result will have the same sign as the original
         * value of this integer.
         * This integer is changed to reflect the result.
         *
         * For a mod routine that always returns a non-negative
         * remainder, see divisionAlg().
         *
         * \pre \a other is not zero.
         *
         * \warning As I understand it, the sign of the result under
         * native C/C++ integer division when the second operand is
         * negative was fixed in the C++11 specification, but left to the
         * compiler implementation in earlier versions of the specification;
         * however, any modern hardware should satisfy the C++11 sign rule
         * as described above.
         *
         * @param other the integer modulo which this integer will be
         * reduced.
         * @return a reference to this integer with its new value.
         */
        NNativeInteger& operator %=(Native other);
        /**
         * Negates this integer.
         * This integer is changed to reflect the result.
         */
        void negate();
        /**
         * Sets this integer to be the greatest common divisor of this
         * and the given integer.
         *
         * The result is guaranteed to be non-negative.  As a
         * special case, gcd(0,0) is considered to be zero.
         *
         * @param other the integer whose greatest common divisor with
         * this will be found.
         */
        void gcdWith(const NNativeInteger& other);
        /**
         * Determines the greatest common divisor of this and the given
         * integer.  This integer is not changed.
         *
         * The result is guaranteed to be non-negative.  As a
         * special case, gcd(0,0) is considered to be zero.
         *
         * @param other the integer whose greatest common divisor with
         * this will be found.
         * @return the greatest common divisor of this and the given
         * integer.
         */
        NNativeInteger gcd(const NNativeInteger& other) const;

    template <int bytes_>
    friend std::ostream& operator << (std::ostream& out,
        const NNativeInteger<bytes_>& large);
};

/**
 * Writes the given integer to the given output stream.
 *
 * @param out the output stream to which to write.
 * @param i the integer to write.
 * @return a reference to \a out.
 */
template <int bytes>
REGINA_API std::ostream& operator << (std::ostream& out, const NNativeInteger<bytes>& i);

/**
 * NNativeLong is a typedef for the NNativeInteger template class whose
 * underlying integer type is a native long.
 *
 * \ifacespython Not present.
 */
typedef NNativeInteger<sizeof(long)> NNativeLong;

/*@}*/

// Inline functions for NIntegerBase

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::NIntegerBase() :
        small_(0), large_(0) {
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::NIntegerBase(int value) :
        small_(value), large_(0) {
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::NIntegerBase(unsigned value) :
        small_(value) {
    // Detect overflow.
    if (small_ < 0) {
        large_ = new mpz_t;
        mpz_init_set_ui(large_, value);
    } else
        large_ = 0;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::NIntegerBase(long value) :
        small_(value), large_(0) {
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::NIntegerBase(unsigned long value) :
        small_(value) {
    // Detect overflow.
    if (small_ < 0) {
        large_ = new mpz_t;
        mpz_init_set_ui(large_, value);
    } else
        large_ = 0;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::NIntegerBase(
        const NIntegerBase<supportInfinity>& value) {
    if (value.isInfinite()) {
        large_ = 0;
        makeInfinite();
    } else if (value.large_) {
        large_ = new mpz_t;
        mpz_init_set(large_, value.large_);
    } else {
        small_ = value.small_;
        large_ = 0;
    }
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::NIntegerBase(
        const NIntegerBase<! supportInfinity>& value) {
    // If value is infinite, we cannot make this infinite.
    // This is why we insist via preconditions that value is finite.
    if (value.large_) {
        large_ = new mpz_t;
        mpz_init_set(large_, value.large_);
    } else {
        small_ = value.small_;
        large_ = 0;
    }
}

template <bool supportInfinity>
template <int bytes>
inline NIntegerBase<supportInfinity>::NIntegerBase(
        const NNativeInteger<bytes>& value) :
        small_(value.nativeValue()), large_(0) {
    BOOST_STATIC_ASSERT(bytes % sizeof(long) == 0);
    if (sizeof(long) < bytes && value.nativeValue() != small_) {
        // It didn't fit.  Take things one long at a time.
        unsigned blocks = bytes / sizeof(long);
        large_ = new mpz_t;
        mpz_init_set_si(large_, static_cast<long>(
            value.nativeValue() >> ((blocks - 1) * 8 * sizeof(long))));
        for (unsigned i = 2; i <= blocks; ++i) {
            mpz_mul_2exp(large_, large_, 8 * sizeof(long));
            mpz_add_ui(large_, large_, static_cast<unsigned long>(
                value.nativeValue() >> ((blocks - i) * 8 * sizeof(long))));
        }
    }
}

template <bool supportInfinity>
template <int bytes>
inline typename IntOfSize<bytes>::type
        NIntegerBase<supportInfinity>::nativeValue() const {
    typedef typename IntOfSize<bytes>::type Native;
    typedef typename IntOfSize<bytes>::utype UNative;
    BOOST_STATIC_ASSERT(bytes % sizeof(long) == 0);

    if (sizeof(long) >= bytes || ! large_) {
        // Suppose this integer lies within the given byte range.
        // If sizeof(long) >= bytes, then it can be made to fit inside a long.
        // If sizeof(long) < bytes but this is a native long, then we
        // have already got it inside a long.
        // Either way, we can just pass the long value across.
        return static_cast<Native>(longValue());
    }
    // From here: this is a GMP integer, and the return type is too
    // large for a long.  Take one long-sized chunk at a time.
    Native ans = 0;
    unsigned blocks = bytes / sizeof(long);

    mpz_t tmp;
    mpz_init_set(tmp, large_);
    for (unsigned i = 0; i < blocks - 1; ++i) {
        ans += (static_cast<UNative>(mpz_get_ui(tmp))
            << (i * 8 * sizeof(long)));
        mpz_fdiv_q_2exp(tmp, tmp, 8 * sizeof(long));
    }
    ans += (static_cast<Native>(mpz_get_si(tmp))
        << ((blocks - 1) * 8 * sizeof(long)));

    mpz_clear(tmp);
    return ans;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>::~NIntegerBase() {
    if (large_) {
        mpz_clear(large_);
        delete[] large_;
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::isNative() const {
    return (! isInfinite()) && (! large_);
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::isZero() const {
    return (! isInfinite()) &&
        (((! large_) && (! small_)) || (large_ && mpz_sgn(large_) == 0));
}

template <bool supportInfinity>
inline int NIntegerBase<supportInfinity>::sign() const {
    return (isInfinite() ? 1 :
        large_ ? mpz_sgn(large_) :
        small_ > 0 ? 1 : small_ < 0 ? -1 : 0);
}

template <>
inline bool NIntegerBase<true>::isInfinite() const {
    return infinite_;
}

template <>
inline bool NIntegerBase<false>::isInfinite() const {
    return false;
}

template <>
inline void NIntegerBase<true>::makeInfinite() {
    infinite_ = true;
    if (large_)
        clearLarge();
}

template <>
inline void NIntegerBase<false>::makeInfinite() {
}

template <bool supportInfinity>
inline long NIntegerBase<supportInfinity>::longValue() const {
    return (large_ ? mpz_get_si(large_) : small_);
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator =(
        const NIntegerBase<supportInfinity>& value) {
    if (value.isInfinite()) {
        makeInfinite();
        return *this;
    }
    makeFinite();
    if (value.large_) {
        if (large_)
            mpz_set(large_, value.large_);
        else {
            large_ = new mpz_t;
            mpz_init_set(large_, value.large_);
        }
    } else {
        small_ = value.small_;
        if (large_)
            clearLarge();
    }
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator =(
        const NIntegerBase<! supportInfinity>& value) {
    // If value is infinite, we cannot make this infinite.
    // This is why we insist via preconditions that value is finite.
    makeFinite();
    if (value.large_) {
        if (large_)
            mpz_set(large_, value.large_);
        else {
            large_ = new mpz_t;
            mpz_init_set(large_, value.large_);
        }
    } else {
        small_ = value.small_;
        if (large_)
            clearLarge();
    }
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator =(int value) {
    makeFinite();
    small_ = value;
    if (large_)
        clearLarge();
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator =(unsigned value) {
    makeFinite();
    small_ = value;

    // Did we overflow?
    if (small_ < 0) {
        // Yes, it's an overflow: just a bit too large for a signed long
        // (literally).
        if (large_)
            mpz_set_ui(large_, value);
        else {
            large_ = new mpz_t;
            mpz_init_set_ui(large_, value);
        }
    } else if (large_) {
        // No overflow, but we must clear out any old large integer value.
        clearLarge();
    }
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator =(long value) {
    makeFinite();
    small_ = value;
    if (large_)
        clearLarge();
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator =(unsigned long value) {
    makeFinite();
    small_ = value;

    // Did we overflow?
    if (small_ < 0) {
        // Yes, it's an overflow: just a bit too large for a signed long
        // (literally).
        if (large_)
            mpz_set_ui(large_, value);
        else {
            large_ = new mpz_t;
            mpz_init_set_ui(large_, value);
        }
    } else if (large_) {
        // No overflow, but we must clear out any old large integer value.
        clearLarge();
    }
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator =(const std::string& value) {
    return (*this) = value.c_str();
}

template <>
inline void NIntegerBase<true>::swap(NIntegerBase<true>& other) {
    // This should just work, since large_ is a pointer.
    std::swap(infinite_, other.infinite_);
    std::swap(small_, other.small_);
    std::swap(large_, other.large_);
}

template <>
inline void NIntegerBase<false>::swap(NIntegerBase<false>& other) {
    // This should just work, since large_ is a pointer.
    std::swap(small_, other.small_);
    std::swap(large_, other.large_);
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator ==(
        const NIntegerBase<supportInfinity>& rhs) const {
    if (isInfinite() && rhs.isInfinite())
        return true;
    else if (isInfinite() || rhs.isInfinite())
        return false;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) == 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) == 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) == 0);
        else
            return (small_ == rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator ==(
        const NIntegerBase<! supportInfinity>& rhs) const {
    // The types are different, so both cannot be infinity.
    if (isInfinite() || rhs.isInfinite())
        return false;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) == 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) == 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) == 0);
        else
            return (small_ == rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator ==(long rhs) const {
    if (isInfinite())
        return false;
    else if (large_)
        return (mpz_cmp_si_cpp(large_, rhs) == 0);
    else
        return (small_ == rhs);
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator !=(
        const NIntegerBase<supportInfinity>& rhs) const {
    if (isInfinite() && rhs.isInfinite())
        return false;
    else if (isInfinite() || rhs.isInfinite())
        return true;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) != 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) != 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) != 0);
        else
            return (small_ != rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator !=(
        const NIntegerBase<! supportInfinity>& rhs) const {
    // The types are different, so both cannot be infinity.
    if (isInfinite() || rhs.isInfinite())
        return true;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) != 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) != 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) != 0);
        else
            return (small_ != rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator !=(long rhs) const {
    if (isInfinite())
        return true;
    else if (large_)
        return (mpz_cmp_si_cpp(large_, rhs) != 0);
    else
        return (small_ != rhs);
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator <(
        const NIntegerBase<supportInfinity>& rhs) const {
    if (isInfinite())
        return false;
    else if (rhs.isInfinite())
        return true;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) < 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) < 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) > 0); // back-to-front
        else
            return (small_ < rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator <(long rhs) const {
    if (isInfinite())
        return false;
    else if (large_)
        return (mpz_cmp_si_cpp(large_, rhs) < 0);
    else
        return (small_ < rhs);
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator >(
        const NIntegerBase<supportInfinity>& rhs) const {
    if (rhs.isInfinite())
        return false;
    else if (isInfinite())
        return true;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) > 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) > 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) < 0); // back-to-front
        else
            return (small_ > rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator >(long rhs) const {
    if (isInfinite())
        return true;
    else if (large_)
        return (mpz_cmp_si_cpp(large_, rhs) > 0);
    else
        return (small_ > rhs);
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator <=(
        const NIntegerBase<supportInfinity>& rhs) const {
    if (rhs.isInfinite())
        return true;
    else if (isInfinite())
        return false;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) <= 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) <= 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) >= 0); // back-to-front
        else
            return (small_ <= rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator <=(long rhs) const {
    if (isInfinite())
        return false;
    else if (large_)
        return (mpz_cmp_si_cpp(large_, rhs) <= 0);
    else
        return (small_ <= rhs);
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator >=(
        const NIntegerBase<supportInfinity>& rhs) const {
    if (isInfinite())
        return true;
    else if (rhs.isInfinite())
        return false;
    else if (large_) {
        if (rhs.large_)
            return (mpz_cmp(large_, rhs.large_) >= 0);
        else
            return (mpz_cmp_si_cpp(large_, rhs.small_) >= 0);
    } else {
        if (rhs.large_)
            return (mpz_cmp_si_cpp(rhs.large_, small_) <= 0); // back-to-front
        else
            return (small_ >= rhs.small_);
    }
}

template <bool supportInfinity>
inline bool NIntegerBase<supportInfinity>::operator >=(long rhs) const {
    if (isInfinite())
        return true;
    else if (large_)
        return (mpz_cmp_si_cpp(large_, rhs) >= 0);
    else
        return (small_ >= rhs);
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator ++() {
    if (! isInfinite()) {
        if (large_)
            mpz_add_ui(large_, large_, 1);
        else if (small_ != LONG_MAX)
            ++small_;
        else {
            // This is the point at which we overflow.
            forceLarge();
            mpz_add_ui(large_, large_, 1);
        }
    }
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator ++(int) {
    if (isInfinite())
        return *this;

    // Hrmph, just do the standard thing for now.
    // It's not clear how much microoptimisation will help..?
    NIntegerBase<supportInfinity> ans(*this);
    ++(*this);
    return ans;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator --() {
    if (! isInfinite()) {
        if (large_)
            mpz_sub_ui(large_, large_, 1);
        else if (small_ != LONG_MIN)
            --small_;
        else {
            // This is the point at which we overflow.
            forceLarge();
            mpz_sub_ui(large_, large_, 1);
        }
    }
    return *this;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator --(int) {
    if (isInfinite())
        return *this;

    // Hrmph, just do the standard thing for now.
    // It's not clear how much microoptimisation will help..?
    NIntegerBase<supportInfinity> ans(*this);
    --(*this);
    return ans;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator +(
        const NIntegerBase<supportInfinity>& other) const {
    if (isInfinite())
        return *this;
    if (other.isInfinite())
        return other;

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans += other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator +(long other) const {
    if (isInfinite())
        return *this;

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans += other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator -(
        const NIntegerBase<supportInfinity>& other) const {
    if (isInfinite())
        return *this;
    if (other.isInfinite())
        return other;

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans -= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator -(long other) const {
    if (isInfinite())
        return *this;

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans -= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator *(
        const NIntegerBase<supportInfinity>& other) const {
    if (isInfinite())
        return *this;
    if (other.isInfinite())
        return other;

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans *= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator *(long other) const {
    if (isInfinite())
        return *this;

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans *= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator /(
        const NIntegerBase<supportInfinity>& other) const {
    if (isInfinite())
        return *this;
    if (other.isInfinite())
        return (long)0;
    if (other.isZero()) {
        NIntegerBase<supportInfinity> ans;
        ans.makeInfinite();
        return ans;
    }

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans /= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator /(long other) const {
    if (isInfinite())
        return *this;
    if (other == 0) {
        NIntegerBase<supportInfinity> ans;
        ans.makeInfinite();
        return ans;
    }

    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans /= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::divExact(
        const NIntegerBase<supportInfinity>& other) const {
    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans.divByExact(other);
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::divExact(long other) const {
    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans.divByExact(other);
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator %(
        const NIntegerBase<supportInfinity>& other) const {
    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans %= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator %(long other) const {
    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    return ans %= other;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>
        NIntegerBase<supportInfinity>::operator -() const {
    if (isInfinite())
        return *this;
    if (large_) {
        NIntegerBase<supportInfinity> ans;
        ans.large_ = new mpz_t;
        mpz_init(ans.large_);
        mpz_neg(ans.large_, large_);
        return ans;
    } else if (small_ == LONG_MIN) {
        // Overflow, just.
        NIntegerBase<supportInfinity> ans;
        ans.large_ = new mpz_t;
        mpz_init_set_si(ans.large_, small_);
        mpz_neg(ans.large_, ans.large_);
        return ans;
    } else
        return NIntegerBase<supportInfinity>(-small_);
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator +=(
        const NIntegerBase<supportInfinity>& other) {
    if (isInfinite())
        return *this;
    else if (other.isInfinite()) {
        makeInfinite();
        return *this;
    }
    if (other.large_) {
        if (! large_)
            forceLarge();
        mpz_add(large_, large_, other.large_);
        return *this;
    } else
        return (*this) += other.small_;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity>&
        NIntegerBase<supportInfinity>::operator -=(
        const NIntegerBase<supportInfinity>& other) {
    if (isInfinite())
        return *this;
    else if (other.isInfinite()) {
        makeInfinite();
        return *this;
    }
    if (other.large_) {
        if (! large_)
            forceLarge();
        mpz_sub(large_, large_, other.large_);
        return *this;
    } else
        return (*this) -= other.small_;
}

template <bool supportInfinity>
inline void NIntegerBase<supportInfinity>::negate() {
    if (isInfinite())
        return;
    if (large_)
        mpz_neg(large_, large_);
    else if (small_ == LONG_MIN) {
        // Overflow, just.
        forceLarge();
        mpz_neg(large_, large_);
    } else
        small_ = -small_;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity> NIntegerBase<supportInfinity>::abs()
        const {
    if (isInfinite())
        return *this;
    if (large_) {
        NIntegerBase<supportInfinity> ans;
        ans.large_ = new mpz_t;
        mpz_init_set(ans.large_, large_);
        mpz_abs(ans.large_, large_);
        return ans;
    } else if (small_ == LONG_MIN) {
        // Overflow, just.
        NIntegerBase<supportInfinity> ans;
        ans.large_ = new mpz_t;
        mpz_init_set_si(ans.large_, small_);
        mpz_neg(ans.large_, ans.large_);
        return ans;
    } else
        return NIntegerBase<supportInfinity>(small_ >= 0 ? small_ : - small_);
}

template <bool supportInfinity>
NIntegerBase<supportInfinity> NIntegerBase<supportInfinity>::gcd(
        const NIntegerBase<supportInfinity>& other) const {
    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    ans.gcdWith(other);
    return ans;
}

template <bool supportInfinity>
NIntegerBase<supportInfinity> NIntegerBase<supportInfinity>::lcm(
        const NIntegerBase<supportInfinity>& other) const {
    // Do the standard thing for now.
    NIntegerBase<supportInfinity> ans(*this);
    ans.lcmWith(other);
    return ans;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity> operator +(long lhs,
        const NIntegerBase<supportInfinity>& rhs) {
    return rhs + lhs;
}

template <bool supportInfinity>
inline NIntegerBase<supportInfinity> operator *(long lhs,
        const NIntegerBase<supportInfinity>& rhs) {
    return rhs * lhs;
}

template <bool supportInfinity>
inline void NIntegerBase<supportInfinity>::setRaw(mpz_srcptr fromData) {
    makeFinite();
    if (! large_) {
        large_ = new mpz_t;
        mpz_init_set(large_, fromData);
    } else {
        mpz_set(large_, fromData);
    }
}

template <bool supportInfinity>
inline mpz_srcptr NIntegerBase<supportInfinity>::rawData() const {
    // Cast away the const, since we are not changing the mathematical value.
    // We are, however, bulking up the representation.
    const_cast<NIntegerBase<supportInfinity>&>(*this).makeLarge();
    return large_;
}

template <bool supportInfinity>
inline mpz_ptr NIntegerBase<supportInfinity>::rawData() {
    makeLarge();
    return large_;
}

template <bool supportInfinity>
inline void NIntegerBase<supportInfinity>::makeLarge() {
    if (! large_)
        forceLarge();
}

template <bool supportInfinity>
inline void NIntegerBase<supportInfinity>::tryReduce() {
    if (large_ && mpz_cmp_si(large_, LONG_MAX) <= 0 &&
            mpz_cmp_si(large_, LONG_MIN) >= 0)
        forceReduce();
}

template <bool supportInfinity>
inline void NIntegerBase<supportInfinity>::forceLarge() {
    large_ = new mpz_t;
    mpz_init_set_si(large_, small_);
}

template <bool supportInfinity>
inline void NIntegerBase<supportInfinity>::clearLarge() {
    mpz_clear(large_);
    delete[] large_;
    large_ = 0;
}

template <bool supportInfinity>
inline void NIntegerBase<supportInfinity>::forceReduce() {
    small_ = mpz_get_si(large_);
    mpz_clear(large_);
    delete[] large_;
    large_ = 0;
}

template <>
inline void NIntegerBase<true>::makeFinite() {
    infinite_ = false;
}

template <>
inline void NIntegerBase<false>::makeFinite() {
}

// This definition must come *after* the definition of makeInfinite()
// to keep the compiler happy.
template <>
inline NIntegerBase<true>::NIntegerBase(bool, bool) : large_(0) {
    // The infinity constructor.
    makeInfinite();
}

// Inline functions for NNativeInteger

template <int bytes>
inline NNativeInteger<bytes>::NNativeInteger() : data_(0) {
}

template <int bytes>
inline NNativeInteger<bytes>::NNativeInteger(Native value) : data_(value) {
}

template <int bytes>
inline NNativeInteger<bytes>::NNativeInteger(
        const NNativeInteger<bytes>& value) :
        data_(value.data_) {
}

template <int bytes>
template <bool supportInfinity>
inline NNativeInteger<bytes>::NNativeInteger(
        const NIntegerBase<supportInfinity>& value) :
        data_(value.template nativeValue<bytes>()) {
}

template <int bytes>
inline bool NNativeInteger<bytes>::isZero() const {
    return (data_ == 0);
}

template <int bytes>
inline int NNativeInteger<bytes>::sign() const {
    return (data_ > 0 ? 1 : data_ < 0 ? -1 : 0);
}

template <int bytes>
inline typename NNativeInteger<bytes>::Native NNativeInteger<bytes>::
        nativeValue() const {
    return data_;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator =(
        const NNativeInteger<bytes>& value) {
    data_ = value.data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator =(Native value) {
    data_ = value;
    return *this;
}

template <int bytes>
inline void NNativeInteger<bytes>::swap(NNativeInteger<bytes>& other) {
    std::swap(data_, other.data_);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator ==(
        const NNativeInteger<bytes>& rhs) const {
    return (data_ == rhs.data_);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator ==(Native rhs) const {
    return (data_ == rhs);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator !=(
        const NNativeInteger<bytes>& rhs) const {
    return (data_ != rhs.data_);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator !=(Native rhs) const {
    return (data_ != rhs);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator <(
        const NNativeInteger<bytes>& rhs) const {
    return (data_ < rhs.data_);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator <(Native rhs) const {
    return (data_ < rhs);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator >(
        const NNativeInteger<bytes>& rhs) const {
    return (data_ > rhs.data_);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator >(Native rhs) const {
    return (data_ > rhs);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator <=(
        const NNativeInteger<bytes>& rhs) const {
    return (data_ <= rhs.data_);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator <=(Native rhs) const {
    return (data_ <= rhs);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator >=(
        const NNativeInteger<bytes>& rhs) const {
    return (data_ >= rhs.data_);
}

template <int bytes>
inline bool NNativeInteger<bytes>::operator >=(Native rhs) const {
    return (data_ >= rhs);
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator ++() {
    ++data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator ++(int) {
    return NNativeInteger<bytes>(data_++);
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator --() {
    --data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator --(int) {
    return NNativeInteger<bytes>(data_--);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator +(
        const NNativeInteger<bytes>& other) const {
    return NNativeInteger<bytes>(data_ + other.data_);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator +(
        Native other) const {
    return NNativeInteger<bytes>(data_ + other);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator -(
        const NNativeInteger<bytes>& other) const {
    return NNativeInteger<bytes>(data_ - other.data_);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator -(
        Native other) const {
    return NNativeInteger<bytes>(data_ - other);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator *(
        const NNativeInteger<bytes>& other) const {
    return NNativeInteger<bytes>(data_ * other.data_);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator *(
        Native other) const {
    return NNativeInteger<bytes>(data_ * other);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator /(
        const NNativeInteger<bytes>& other) const {
    return NNativeInteger<bytes>(data_ / other.data_);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator /(
        Native other) const {
    return NNativeInteger<bytes>(data_ / other);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::divExact(
        const NNativeInteger<bytes>& other) const {
    return NNativeInteger<bytes>(data_ / other.data_);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::divExact(
        Native other) const {
    return NNativeInteger<bytes>(data_ / other);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator %(
        const NNativeInteger<bytes>& other) const {
    return NNativeInteger<bytes>(data_ % other.data_);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator %(
        Native other) const {
    return NNativeInteger<bytes>(data_ % other);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::divisionAlg(
        const NNativeInteger<bytes>& divisor,
        NNativeInteger<bytes>& remainder) const {
    if (divisor == 0) {
        remainder.data_ = data_;
        return 0;
    }

    // Native integer division could leave a negative remainder
    // regardless of the sign of the divisor (I think the standard
    // indicates that the decision is based on the sign of *this?).
    NNativeInteger<bytes> quotient = data_ / divisor.data_;
    remainder = data_ - (quotient.data_ * divisor.data_);
    if (remainder.data_ < 0) {
        if (divisor.data_ > 0) {
            remainder.data_ += divisor.data_;
            --quotient.data_;
        } else {
            remainder.data_ -= divisor.data_;
            ++quotient.data_;
        }
    }

    return quotient;
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::operator -() const {
    return NNativeInteger<bytes>(- data_);
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator += (
        const NNativeInteger<bytes>& other) {
    data_ += other.data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator += (
        Native other) {
    data_ += other;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator -= (
        const NNativeInteger<bytes>& other) {
    data_ -= other.data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator -= (
        Native other) {
    data_ -= other;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator *= (
        const NNativeInteger<bytes>& other) {
    data_ *= other.data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator *= (
        Native other) {
    data_ *= other;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator /= (
        const NNativeInteger<bytes>& other) {
    data_ /= other.data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator /= (
        Native other) {
    data_ /= other;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::divByExact(
        const NNativeInteger<bytes>& other) {
    data_ /= other.data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::divByExact(Native other) {
    data_ /= other;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator %= (
        const NNativeInteger<bytes>& other) {
    data_ %= other.data_;
    return *this;
}

template <int bytes>
inline NNativeInteger<bytes>& NNativeInteger<bytes>::operator %= (
        Native other) {
    data_ %= other;
    return *this;
}

template <int bytes>
inline void NNativeInteger<bytes>::negate() {
    data_ = - data_;
}

template <int bytes>
void NNativeInteger<bytes>::gcdWith(const NNativeInteger<bytes>& other) {
    Native a = data_;
    Native b = other.data_;

    if (a < 0) a = -a;
    if (b < 0) b = -b;

    /**
     * Now everything is non-negative.
     * The following code is based on Stein's binary GCD algorithm.
     */
    if (! a) {
        data_ = b;
        return;
    }
    if (! b) {
        data_ = a;
        return;
    }

    // Compute the largest common power of 2.
    int pow2;
    for (pow2 = 0; ! ((a | b) & 1); ++pow2) {
        a >>= 1;
        b >>= 1;
    }

    // Strip out all remaining powers of 2 from a and b.
    while (! (a & 1))
        a >>= 1;
    while (! (b & 1))
        b >>= 1;

    while (a != b) {
        // INV: a and b are both odd and non-zero.
        if (a < b) {
            b -= a;
            do
                b >>= 1;
            while (! (b & 1));
        } else {
            a -= b;
            do
                a >>= 1;
            while (! (a & 1));
        }
    }
    data_ = (a << pow2);
}

template <int bytes>
inline NNativeInteger<bytes> NNativeInteger<bytes>::gcd(
        const NNativeInteger<bytes>& other) const {
    NNativeInteger<bytes> ans(data_);
    ans.gcdWith(other);
    return ans;
}

template <int bytes>
inline std::ostream& operator << (std::ostream& out,
        const NNativeInteger<bytes>& i) {
    return out << i.data_;
}

#ifdef INT128_AVAILABLE
template <>
inline std::ostream& operator << (std::ostream& out,
        const NNativeInteger<16>& i) {
    // The standard library does not support 128-bit integers (at least not
    // always).  Go through NInteger and GMP instead (for now).
    return out << NInteger(i);
}
#endif

} // namespace regina

#endif

