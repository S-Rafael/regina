
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 2011-2021, Ben Burton                                   *
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

/*! \file enumerate/treeconstraint.h
 *  \brief Constraint classes for use with tree traversal enumeration methods.
 */

#ifndef __REGINA_TREECONSTRAINT_H
#ifndef __DOXYGEN
#define __REGINA_TREECONSTRAINT_H
#endif

#include "maths/integer.h"
#include "surfaces/normalcoords.h"
#include "surfaces/normalsurface.h"

namespace regina {

class AngleStructure;

template <typename IntType> class LPMatrix;
template <class LPConstraint> struct LPCol;
template <class LPConstraint> class LPInitialTableaux;
template <class LPConstraint, typename IntType> class LPData;

class LPConstraintNone;

/**
 * A base class for additional linear constraints that we can add to the
 * tableaux of normal surface or angle structure matching equations.  This is
 * used with TreeEnumeration, TreeSingleSoln and related algorithms for
 * enumerating and locating normal surfaces or angle structures in a
 * 3-manifold triangulation.  See the LPInitialTableaux class notes for
 * details on how these constraints interact with the tableaux of
 * matching equations.
 *
 * The linear constraints may be equalities or inequalities, and there
 * may be more than one such constraint.  If all constraints are
 * homogeneous equalities, the class should derive from LPConstraintSubspace
 * instead (not this base class).
 *
 * In angle structure coordinates, these linear constraints must \e not
 * involve the scaling coordinate (the final coordinate that is used to
 * convert the angle structure polytope into a polyhedral cone).
 * The coefficient for the final scaling coordinate in each additional
 * linear constraint will be assumed to be zero.
 *
 * Bear in mind that the tableaux that these constraints are working with will
 * not necessarily use the same coordinates as the underlying enumeration task
 * (e.g., the tableaux will never include separate columns for octagon
 * coordinates).  See LPInitialTableaux for a more detailed discussion of this.
 *
 * This base class provides no functionality.  For documentation's sake
 * only, the notes here describe the functionality that any subclass
 * \e must implement.  We note again that LPConstraintBase does not
 * provide any implementations at all, and subclasses are completely
 * responsible for their own implementations.
 *
 * All constraint classes provide their functionality through static routines:
 * they do not contain any member data, and it is unnecessary (but harmless) to
 * construct them.  On the other hand, their inner Coefficients classes
 * do contain data, and must support value semantics; see the Coefficients
 * documentation for details.
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class LPConstraintBase {
#ifdef __DOXYGEN
    public:
        enum {
            /**
             * The number of additional linear constraints that we impose.
             * Each constraint will generate one new variable (column)
             * and one new equation (row) in the tableaux.
             */
            nConstraints
        };

        /**
         * Stores the extra coefficients in a single column for the
         * \a nConstraints additional rows that we add to the tableaux
         * to describe the \a nConstraints additional linear equations
         * or inequalities.
         *
         * Subclasses may store these coefficients however they like
         * (in particular, they may optimise for sparse coefficients,
         * binary coefficients, and so on).  They will only ever be
         * accessed through the member functions of this Coefficients class.
         *
         * This Coefficients class must support value semantics: it \e must
         * provide copy construction and assignment, and if it will help
         * performance it \e may also provide move construction and assignment.
         * It should not provide custom swap() functions: all swapping will
         * be done via std::swap(), which uses move operations where possible.
         */
        struct Coefficients {
            /**
             * Creates an uninitialised set of coefficients for a single
             * column.  These cofficients must be initialised through a
             * call to addRows() before they can be used.
             */
            Coefficients();

            /**
             * Creates a new copy of the given set of coefficients.
             */
            Coefficients(const Coefficients&);

            /**
             * Sets this to be a copy of the given set of coefficients.
             *
             * @return a reference to this set of coefficients.
             */
            Coefficients& operator = (const Coefficients&);

            /**
             * Explicitly fills the final row(s) of the given tableaux matrix 
             * with the coefficients stored in this Coefficients structure.
             * In essence, this routine simply copies this sparse and/or
             * specialised representation of the final row(s) into a
             * more standard dense matrix representation.
             *
             * This routine should only affect the final \a nConstraints
             * entries in the given column of the matrix.  It may assume
             * that these final row(s) have already been initialised to zero.
             *
             * \pre The given matrix has at least \a nConstraints rows
             * and at least \a col + 1 columns.
             * \pre The final \a nConstraints entries in column \a col
             * of the given matrix have already been set to zero.
             *
             * @param m the matrix in which to place these column
             * coefficients.
             * @param col the column of the given matrix in which to
             * place these coefficients.
             */
            template <typename IntType>
            void fillFinalRows(LPMatrix<IntType>& m, unsigned col) const;

            /**
             * Computes the inner product of (i) the final \a nConstraints
             * entries in the given row of the given matrix with (ii) the
             * \a nConstraints column coefficients stored in this data
             * structure.
             *
             * \pre The given matrix has at least \a nConstraints columns
             * and at least \a mRow + 1 rows.
             *
             * @param m the matrix whose row we will use in the inner product.
             * @param mRow the row of the matrix \a m to use in the inner
             * product.
             * @return the resulting portion of the inner product.
             */
            template <typename IntType>
            IntType innerProduct(const LPMatrix<IntType>& m, unsigned mRow)
                const;

            /**
             * A variant of innerProduct() that takes into account any
             * adjustments to these linear constraint(s) that are required when
             * this is a quadrilateral column being used to represent an
             * octagon type.
             *
             * The LPData class offers support for octagonal almost normal
             * surfaces, in which exactly one tetrahedron is allowed to have
             * exactly one octagon type.  We represent such an octagon as a
             * \e pair of incompatible quadrilaterals within the same
             * tetrahedron.  See the LPData class notes for details on how
             * this works.
             *
             * In some settings, our extra linear constraints must behave
             * differently in the presence of octagons (i.e., the coefficient
             * of the octagon type is not just the sum of coefficients of the
             * two constituent quadrilateral types).  This routine effectively
             * allows us to adjust the tableaux accordingly.
             *
             * Specifically: this routine computes the inner product of (i) the
             * final \a nConstraints entries in the given row of the given
             * matrix with (ii) the \a nConstraints column coefficients
             * stored in this data structure.  We assume that this column
             * in the underlying tableaux describes one of the two
             * quadrilateral coordinates in some tetrahedron that together
             * form an octagon type, and if necessary we implicitly adjust
             * the coefficients stored in this data structure accordingly.
             *
             * This routine is not used with angle structure coordinates.
             *
             * \pre The given matrix has at least \a nConstraints columns
             * and at least \a mRow + 1 rows.
             *
             * \pre This column of the underlying tableaux describes one
             * of the two quadrilateral coordinates that are being
             * combined to form an octagon type within some tetrahedron.
             *
             * @param m the matrix whose row we will use in the inner product.
             * @param mRow the row of the matrix \a m to use in the inner
             * product.
             * @return the resulting portion of the inner product.
             */
            template <typename IntType>
            IntType innerProductOct(const LPMatrix<IntType>& m, unsigned mRow)
                const;
        };

        /**
         * Explicitly constructs equations for the linear function(s)
         * constrained by this class.  Specifically, this routine takes an
         * array of Coefficients objects (one for each column of the initial
         * tableaux) and fills in the necessary coefficient data.
         *
         * The precise form of the linear function(s) will typically
         * depend upon the underlying triangulation.  For this reason,
         * the triangulation is explicitly passed, along with the
         * permutation that indicates which columns of the initial tableaux
         * correspond to which normal or angle structure coordinates.
         *
         * More precisely: recall that, for each linear function, the initial
         * tableaux acquires one new variable \a x_i that evaluates this linear
         * function f(x).  This routine must create the corresponding row that
         * sets <tt>f(x) - x_i = 0</tt>.  Thus it must construct the
         * coefficients of f(x) in the columns corresponding to normal
         * coordinates, and it must also set a coefficient of -1 in the
         * column for the corresponding new variable.
         *
         * For each subclass \a S of LPConstraintBase, the array \a col
         * must be an array of objects of type LPCol<S>.
         * The class LPCol<S> is itself a larger subclass of
         * the Coefficients class.  This exact type must be used because the
         * compiler must know how large each column object is in
         * order to correct access each element of the given array.
         *
         * As described in the LPInitialTableaux class notes, it might
         * not be possible to construct the linear functions (since the
         * triangulation might not satisfy the necessary requirements).
         * In this case, this routine should ensure that the linear
         * functions are in fact the zero functions, and should return
         * \c false (but it must still set -1 coefficients for the new
         * variables as described above).  Otherwise (if the linear function
         * were successfully constructed) this routine should return \c true.
         *
         * If you are implementing this routine in a subclass that
         * works with angle structure coordinates, remember that your
         * linear constraints must not interact with the scaling coordinate
         * (the final angle structure coordinate that is used to projectivise
         * the angle structure polytope into a polyhedral cone).  Your
         * implementation of this routine \e must ensure that your
         * linear constraints all have coefficient zero in this column.
         *
         * \pre For all coefficients in the array \a col, the
         * Coefficients substructures have all been initialised with the
         * default constructor and not modified since.
         *
         * @param col the array of columns as stored in the initial
         * tableaux (i.e., the data member LPInitialTableaux::col_).
         * @param columnPerm the corresponding permutation of columns
         * that describes how columns of the tableaux correspond to normal or
         * angle structure coordinates in the underlying triangulation
         * (i.e., the data member LPInitialTableaux::columnPerm_).
         * @param tri the underlying triangulation.
         * @return \c true if the linear functions were successfully
         * constructed, or \c false if not (in which case they will be
         * replaced with the zero functions instead).
         */
        static bool addRows(LPCol<LPConstraintBase>* col,
            const int* columnPerm, const Triangulation<3>& tri);

        /**
         * Explicitly constraints each of these linear functions to an
         * equality or inequality in the underlying tableaux.  This will
         * typically consist of a series of calls to LPData::constrainZero()
         * and/or LPData::constrainPositive().
         *
         * The variables for these extra linear functions are stored in
         * columns <tt>numCols - nConstraints</tt>, ..., <tt>numCols - 1</tt>
         * of the given tableaux, and so your calls to LPData::constrainZero()
         * and/or LPData::constrainPositive() should operate on these
         * (and only these) columns.
         * 
         * \pre These column coefficients belong to the initial starting
         * tableaux (LPInitialTableaux) from which the given tableaux is
         * derived.
         *
         * @param lp the tableaux in which to constrain these linear
         * functions.
         * @param numCols the number of columns in the given tableaux.
         */
        template <typename IntType>
        static void constrain(LPData<LPConstraintNone, IntType>& lp,
            unsigned numCols);

        /**
         * Ensures that the given normal surface satisfies the extra
         * constraints described by this class.
         *
         * Ideally this test is not based on explicitly recomputing the
         * linear function(s), but instead runs independent tests.
         * For instance, if this class is used to constraint Euler
         * characteristic, then ideally this routine would call
         * s.eulerChar() and test the return value of that routine instead.
         *
         * If these linear constraints work with angle structure coordinates
         * (not normal or almost normal surfaces), then this routine should
         * return \c false.
         *
         * @param s the surface to test.
         * @return \c true if the given surface satisfies these linear
         * constraints, or \c false if it does not.
         */
        static bool verify(const NormalSurface& s);

        /**
         * Ensures that the given angle structure satisfies the extra
         * constraints described by this class.
         *
         * Ideally this test is not based on explicitly recomputing the
         * linear function(s), but instead runs independent tests;
         * see the related routine verify(const NormalSurface&) for examples.
         *
         * If these linear constraints work with normal or almost normal
         * surfaces (not angle structure coordinates), then this routine should
         * return \c false.
         *
         * @param s the angle structure to test.
         * @return \c true if the given angle structure satisfies these linear
         * constraints, or \c false if it does not.
         */
        static bool verify(const AngleStructure& s);

        /**
         * Indicates whether the given vector encoding is supported by
         * this constraint class.
         *
         * This routine assumes that the given encoding is already known to be
         * supported by the generic tree traversal infrastructure, and only
         * returns \c false if there are additional prerequisites
         * imposed by this particular constraint class that the given
         * encoding does not satisfy.  If this constraint class does not impose
         * any of its own additional conditions, this routine may
         * simply return \c true.
         *
         * The only features of the encoding that this routine should
         * examine are what coordinates are stored (e.g.,
         * NormalEncoding::storesTriangles()).  In particular, this
         * routine will not look at any "semantic guarantees" (e.g.
         * NormalEncoding::couldBeNonCompact()).
         *
         * @param enc the vector encoding being queried.  This must
         * be one of the vector encodings known to be supported by the
         * generic TreeTraversal infrastructure, and in particular it
         * may be the special angle structure encoding.
         * @return \c true if and only if this vector encoding is
         * also supported by this specific constraint class.
         */
        static bool supported(NormalEncoding enc);
#endif
};

/**
 * A subclass of LPConstraintBase used for constraints defined entirely
 * by homogeneous linear equations.
 *
 * Any set of constraints defined entirely by homogeneous linear
 * equations should derive from LPConstraintSubspace, not LPConstraintBase.
 * In other words, any set of constraints derived from LPConstraintSubspace
 * should simply restrict our attention to a vector subspace of the
 * normal surface or angle structure coordinate system.
 *
 * This class does not provide any additional functionality.  It is
 * merely a convenience to help describe and enforce preconditions.
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class LPConstraintSubspace : public LPConstraintBase {
};

/**
 * A do-nothing class that imposes no additional linear constraints on
 * the tableaux of normal surface or angle structure matching equations.
 *
 * See the LPConstraintBase class notes for details on all member
 * functions and structs.
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class LPConstraintNone : public LPConstraintSubspace {
    public:
        enum { nConstraints = 0 };

        /**
         * Stores the extra coefficients in the tableaux associated
         * with this constraint class (which for this class is a no-op,
         * since in this case there are no extra coefficients).
         *
         * See the LPConstraintBase::Coefficients notes for further details.
         */
        struct Coefficients {
            Coefficients() = default;
            Coefficients(const Coefficients&) = default;
            Coefficients& operator = (const Coefficients&) = default;

            template<typename IntType>
            void fillFinalRows(LPMatrix<IntType>& m, unsigned col) const;
            template<typename IntType>
            IntType innerProduct(const LPMatrix<IntType>&, unsigned) const;
            template<typename IntType>
            IntType innerProductOct(const LPMatrix<IntType>&, unsigned) const;
        };

        static bool addRows(LPCol<regina::LPConstraintNone>*,
            const int*, const Triangulation<3>&);
        template<typename IntType>
        static void constrain(
            LPData<regina::LPConstraintNone, IntType>&, unsigned);
        static bool verify(const NormalSurface&);
        static bool verify(const AngleStructure&);
        static bool supported(NormalEncoding enc);
};

/**
 * A class that constraints the tableaux of normal surface matching equations
 * to ensure that Euler characteristic is strictly positive.
 *
 * There are many ways of writing Euler characteritic as a linear
 * function.  The function constructed here has integer coefficients,
 * but otherwise has no special properties of note.
 *
 * This constraint can work with either normal or almost normal
 * coordinates.  In the case of almost normal coordinates, the function
 * is modified to measure Euler characteristic minus the number of
 * octagons (a technique of Casson, also employed by Jaco and Rubinstein, that
 * is used to ensure we do not have more than two octagons when searching for
 * a normal or almost normal sphere in the 3-sphere recognition algorithm).
 *
 * See the LPConstraintBase class notes for details on all member
 * functions and structs.
 *
 * \pre We are working with a normal or almost normal vector encoding
 * that includes triangle coordinates (i.e., the encoding for standard
 * normal or standard almost normal coordinates).
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class LPConstraintEulerPositive : public LPConstraintBase {
    public:
        enum { nConstraints = 1 };

        /**
         * Stores the extra coefficients in the tableaux associated with this
         * constraint class (in this case, one extra integer per column).
         *
         * See the LPConstraintBase::Coefficients notes for further details.
         */
        struct Coefficients {
            int euler { 0 };
                /**< The coefficient of the Euler characteristic
                     function for the corresponding column of the matching
                     equation matrix. */

            Coefficients() = default;
            Coefficients(const Coefficients&) = default;
            Coefficients& operator = (const Coefficients&) = default;
            template<typename IntType>
            void fillFinalRows(LPMatrix<IntType>& m, unsigned col) const;
            template<typename IntType>
            IntType innerProduct(const LPMatrix<IntType>& m,
                    unsigned mRow) const;
            template<typename IntType>
            IntType innerProductOct(const LPMatrix<IntType>& m,
                    unsigned mRow) const;
        };

        static bool addRows(
            LPCol<regina::LPConstraintEulerPositive>* col,
            const int* columnPerm, const Triangulation<3>& tri);
        template<typename IntType>
        static void constrain(
            LPData<regina::LPConstraintEulerPositive, IntType>& lp,
            unsigned numCols);
        static bool verify(const NormalSurface& s);
        static bool verify(const AngleStructure&);
        static bool supported(NormalEncoding enc);
};

/**
 * A deprecated typedef for LPConstraintEulerPositive.
 *
 * The old name LPConstraintEuler should no longer be used, since Regina
 * now provides multiple constraint types relating to Euler characteristic.
 *
 * \ingroup enumerate
 */
typedef LPConstraintEulerPositive LPConstraintEuler [[deprecated]];

/**
 * A class that constraints the tableaux of normal surface matching equations
 * to ensure that Euler characteristic is zero.
 *
 * There are many ways of writing Euler characteritic as a linear
 * function.  The function constructed here has integer coefficients,
 * but otherwise has no special properties of note.
 *
 * This constraint currently only works with normal (and \e not almost normal)
 * coordinates.
 *
 * See the LPConstraintBase class notes for details on all member
 * functions and structs.
 *
 * \pre We are working with a normal vector encoding that includes triangle
 * coordinates, and that does \e not include octagon coordinates (i.e,
 * the encoding for standard normal coordinates).
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class LPConstraintEulerZero : public LPConstraintSubspace {
    public:
        enum { nConstraints = 1 };

        /**
         * Stores the extra coefficients in the tableaux associated with this
         * constraint class (in this case, one extra integer per column).
         *
         * See the LPConstraintBase::Coefficients notes for further details.
         */
        struct Coefficients {
            int euler { 0 };
                /**< The coefficient of the Euler characteristic
                     function for the corresponding column of the matching
                     equation matrix. */

            Coefficients() = default;
            Coefficients(const Coefficients&) = default;
            Coefficients& operator = (const Coefficients&) = default;
            template<typename IntType>
            void fillFinalRows(LPMatrix<IntType>& m, unsigned col) const;
            template<typename IntType>
            IntType innerProduct(const LPMatrix<IntType>& m,
                    unsigned mRow) const;
            template<typename IntType>
            IntType innerProductOct(const LPMatrix<IntType>& m,
                    unsigned mRow) const;
        };

        static bool addRows(
            LPCol<regina::LPConstraintEulerZero>* col,
            const int* columnPerm, const Triangulation<3>& tri);
        template<typename IntType>
        static void constrain(
            LPData<regina::LPConstraintEulerZero, IntType>& lp,
            unsigned numCols);
        static bool verify(const NormalSurface& s);
        static bool verify(const AngleStructure&);
        static bool supported(NormalEncoding enc);
};

/**
 * A class that constraints the tableaux of normal surface matching equations
 * to ensure that normal surfaces in an ideal triangulation are compact
 * (thereby avoiding spun normal surfaces with infinitely many triangles).
 *
 * At present this class can only work with oriented triangulations that have
 * precisely one vertex, which is ideal with torus link.  These
 * constraints are explicitly checked by addRows(), which returns \c false
 * if they are not satisfied.  Moreover, this constraint calls on
 * SnapPea for some calculations: in the unexpected situation where
 * SnapPea retriangulates, the linear function cannot be constructed and
 * addRows() will again return \c false.  You should always test
 * LPInitialTableaux::constraintsBroken() to verify that the linear
 * functions have been constructed correctly.
 *
 * Also, at present this class can only work with quadrilateral normal
 * coordinates (and cannot handle almost normal coordinates at all).
 * This is \e not explicitly checked; instead it appears as a
 * precondition (see below).
 *
 * See the LPConstraintBase class notes for details on all member
 * functions and structs.
 *
 * \pre We are working with a normal or almost normal vector encoding that
 * does not include triangle coordinates (i.e., the encoding for quad or
 * quad-oct normal coordinates).
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class LPConstraintNonSpun : public LPConstraintSubspace {
    public:
        enum { nConstraints = 2 };

        /**
         * Stores the extra coefficients in the tableaux associated with this
         * constraint class (in this case, two extra integers per column).
         *
         * See the LPConstraintBase::Coefficients notes for further details.
         */
        struct Coefficients {
            int meridian { 0 };
                /**< The coefficient of the meridian equation for the
                     corresponding column of the matching equation matrix. */
            int longitude { 0 };
                /**< The coefficient of the longitude equation for the
                     corresponding column of the matching equation matrix. */

            Coefficients() = default;
            Coefficients(const Coefficients&) = default;
            Coefficients& operator = (const Coefficients&) = default;
            template <typename IntType>
            void fillFinalRows(LPMatrix<IntType>& m, unsigned col) const;
            template <typename IntType>
            IntType innerProduct(const LPMatrix<IntType>& m,
                    unsigned mRow) const;
            template <typename IntType>
            IntType innerProductOct(const LPMatrix<IntType>& m,
                    unsigned mRow) const;
        };

        static bool addRows(
            LPCol<regina::LPConstraintNonSpun>* col,
            const int* columnPerm, const Triangulation<3>& tri);
        template <typename IntType>
        static void constrain(
            LPData<regina::LPConstraintNonSpun, IntType>& lp,
            unsigned numCols);
        static bool verify(const NormalSurface& s);
        static bool verify(const AngleStructure&);
        static bool supported(NormalEncoding enc);
};

/**
 * A base class for additional banning and marking constraints that we
 * can place on tree traversal algorithms.  This is used with
 * TreeEnumeration, TreeSingleSoln and related algorithms for
 * enumerating and locating normal surfaces and angle structures in a
 * 3-manifold triangulation.
 *
 * This class adds constraints of two types:
 *
 * - \e Banning constraints, which ensure that certain coordinates
 *   are set to zero;
 *
 * - \e Marking constraints, which are more flexible and can be used in
 *   different ways by different algorithms.
 *
 * All of these constraints operate only on normal or angle structure
 * coordinates in the underlying tableaux (and in particular not the
 * additional variables introduced by additional linear constraints,
 * as described by LPConstraintBase and its subclasses).
 *
 * Currently marking is used in the following ways:
 *
 * - The TreeEnumeration and TautEnumeration algorithms do not use marking
 *   at all.
 *
 * - In the TreeSingleSoln algorithm, marking affects what is considered
 *   a non-trivial normal surface.  Normally, a non-trivial surface is defined
 *   to be one in which some triangle coordinate is zero.  With marking,
 *   a non-trivial surface is redefined to be one in which some \e unmarked
 *   triangle coordinate is zero.  In other words, marked triangle types
 *   are effectively ignored when determining whether a surface is non-trivial
 *   or not.
 *
 * At present, marking is not used at all for quadrilateral coordinates
 * or angle structures.  However, marking is a very new feature, and this
 * concept may be expanded in future versions of Regina.
 *
 * This class does not record disc types in the order of their normal or
 * angle structure coordinates; instead it records them in the order of their
 * columns in a tableaux for linear programming (as used in LPInitialTableaux).
 * This means that there is a little more work required in setting up
 * the initial lists of banned and marked columns, but then these lists are
 * easy to use on the fly during tree traversal algorithms.
 *
 * Bear in mind that the tableaux that these constraints are working with will
 * not necessarily use the same coordinates as the underlying enumeration task
 * (e.g., the tableaux will never include separate columns for octagon
 * coordinates).  See LPInitialTableaux for a more detailed discussion of this.
 *
 * This base class provides limited functionality (as documented below).
 * Subclasses \e must implement a constructor (which, like this base
 * class, takes a triangulation and a coordinate system), must implement
 * init() which determines which coordinates are banned and/or marked,
 * and must implement supported(), which indicates which normal or angle
 * structure coordinate system this constraint class can work with.
 *
 * All ban constraint classes are designed to help TreeTraversal manage
 * significant enumeration and search operations, and objects of these classes
 * cannot be constructed directly by end users.  To use a ban constraint class,
 * pass it as a template parameter to one of the tree traversal subclasses
 * (e.g., TreeEnumeration, TreeSingleSolution, or TautEnumeration).
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class BanConstraintBase {
    protected:
        const Triangulation<3>& tri_;
            /**< The triangulation with which we are working. */
        NormalEncoding enc_;
            /**< The vector encoding that our enumeration task is using. */
        bool* banned_;
            /**< Indicates which columns of a tableaux correspond to banned
                 coordinates (e.g., banned normal disc types).
                 The size of this array is the number of normal or angle
                 structure coordinates (so we explicitly exclude extra columns
                 that arise from the template parameter LPConstraint. */
        bool* marked_;
            /**< Indicates which columns of a tableaux correspond to marked
                 coordinates (e.g., marked normal disc types).
                 The size of this array is the number of normal or angle
                 structure coordinates (so we explicitly exclude extra columns
                 that arise from the template parameter LPConstraint. */

    protected:
        /**
         * Constructs and initialises the \a banned_ and \a marked_ arrays
         * to be entirely \c false.  The only purpose of passing the
         * triangulation and vector encoding is to determine how many
         * normal or angle structure coordinates we are dealing with.
         *
         * \warning Before you use this object, the routine init() must be
         * called to fill in the \a banned_ and \a marked_ arrays with the
         * correct data.  Otherwise you will have no banned or marked disc
         * types at all.
         *
         * @param tri the triangulation with which we are working.
         * @param enc the vector encoding being used for this enumeration task.
         * This must be one of the vector encodings known to be supported by
         * the generic TreeTraversal infrastructure, and in particular it
         * may be the special angle structure encoding.
         */
        BanConstraintBase(const Triangulation<3>& tri, NormalEncoding enc);

        /**
         * Destroys this object and all associated data.
         */
        ~BanConstraintBase();

        /**
         * Enforces all bans described by this class in the given
         * tableaux.  Specifically, for each banned coordinate, this
         * routine calls LPData::constrainZero() on the corresponding
         * coordinate column.
         *
         * @param lp the tableaux in which to enforce the bans.
         */
        template <class LPConstraint, typename IntType>
        void enforceBans(LPData<LPConstraint, IntType>& lp) const;

        /**
         * Identifies whether the given column of the tableaux corresponds to
         * a marked coordinate (e.g., a marked normal disc type).
         *
         * @param column a column of the tableaux.  This must be one of
         * the columns corresponding to a normal or angle structure coordinate,
         * not one of the extra columns induced by an LPConstraint parameter
         * for the tree traversal class.
         * @return \c true if and only if the given column corresponds
         * to a marked coordinate.
         */
        bool marked(size_t column) const;

#ifdef __DOXYGEN
        /**
         * Identifies which coordinates to ban and mark, and records the
         * corresponding tableaux columns in the \a banned_ and \a marked_
         * arrays respectively.
         *
         * @param columnPerm the permutation of columns that describes how
         * columns of the tableaux correspond to normal or angle strutcure
         * coordinates in the underlying triangulation.  Specifically, this
         * permutation must be the same permutation returned by
         * LPInitialTableaux::columnPerm().
         */
        void init(const int* columnPerm);

        /**
         * Indicates whether the given coordinate system is supported by
         * this constraint class.
         *
         * This routine assumes that the given system is already known to be
         * supported by the generic tree traversal infrastructure, and only
         * returns \c false if there are additional prerequisites
         * imposed by this particular constraint class that the given
         * system does not satisfy.  If this constraint class does not impose
         * any of its own additional conditions, this routine may
         * simply return \c true.
         *
         * The only features of the encoding that this routine should
         * examine are what coordinates are stored (e.g.,
         * NormalEncoding::storesTriangles()).  In particular, this
         * routine will not look at any "semantic guarantees" (e.g.
         * NormalEncoding::couldBeNonCompact()).
         *
         * @param enc the vector encoding being queried.  This must
         * be one of the vector encodings known to be supported by the
         * generic TreeTraversal infrastructure, and in particular it
         * may be the special angle structure encoding.
         * @return \c true if and only if this vector encoding is
         * also supported by this specific constraint class.
         */
        static bool supported(NormalEncoding enc);
#endif

        // Mark this class as non-copyable.
        BanConstraintBase(const BanConstraintBase&) = delete;
        BanConstraintBase& operator = (const BanConstraintBase&) = delete;
};

/**
 * A do-nothing class that bans no coordinates and marks no coordinates.
 *
 * This is intended to act as a drop-in replacement for a "real" BanConstraint
 * class (i.e., a subclass of BanConstraintBase).  However, to avoid any
 * overhead in this trivial case, BanNone does \e not derive from
 * BanConstraintBase, and all of its routines do nothing at all.
 *
 * See the BanConstraintBase class notes for details on the interface
 * that this class adheres to.
 *
 * This class is designed to help TreeTraversal manage significant enumeration
 * and search operations, and objects of this class cannot be constructed
 * directly by end users.  To use this class, pass it as the BanConstraint
 * template parameter to one of the tree traversal subclasses
 * (e.g., TreeEnumeration, TreeSingleSolution, or TautEnumeration).
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class BanNone {
    protected:
        BanNone(const Triangulation<3>&, NormalEncoding) {}

        template <class LPConstraint, typename IntType>
        void enforceBans(LPData<LPConstraint, IntType>&) const {}

        bool marked(size_t column) const { return false; }
        void init(const int*) {}
        static bool supported(NormalEncoding enc) { return true; }
};

/**
 * A class that bans normal disc types that meet the boundary of the
 * underlying triangulation.  No disc types are marked at all.
 *
 * This class is only for use with normal or almost normal surfaces, not
 * angle structures.
 *
 * \warning This class only works as expected with vector encodings that
 * explicitly include triangles (e.g., encodings for standard normal or
 * almost normal coordinates).  In quadrilateral or quadrilateral-octagon
 * coordinates it will only ban quadrilaterals or octagons that touch
 * the boundary, but it will still allow \e triangles that meet the boundary
 * (since triangle types are not counted in these coordinate systems).
 * The supported() routine will only return \c true for encodings that
 * include triangles.
 *
 * See the BanConstraintBase class notes for details on all member
 * functions and structs.
 *
 * This class is designed to help TreeTraversal manage significant enumeration
 * and search operations, and objects of this class cannot be constructed
 * directly by end users.  To use this class, pass it as the BanConstraint
 * template parameter to one of the tree traversal subclasses
 * (e.g., TreeEnumeration or TreeSingleSolution).
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class BanBoundary : public BanConstraintBase {
    protected:
        /**
         * Constructs and initialises the \a banned_ and \a marked_ arrays
         * to be entirely \c false, as described in the BanConstraintBase
         * superclass constructor.
         *
         * \warning Before you use this object, the routine init() must be
         * called to fill in the \a banned_ and \a marked_ arrays with the
         * correct data.  Otherwise you will have no banned or marked disc
         * types at all.
         *
         * @param tri the triangulation with which we are working.
         * @param enc the vector encoding being used for this enumeration task.
         * This must be one of the normal or almost normal surface vector
         * encodings known to be supported by the generic TreeTraversal
         * infrastructure.
         */
        BanBoundary(const Triangulation<3>& tri, NormalEncoding enc);

        void init(const int* columnPerm);
        static bool supported(NormalEncoding enc);
};

/**
 * A class that bans and marks disc types associated with torus boundary
 * components.  Here we refer exclusively to real torus boundary
 * components (not ideal vertices with torus cusps).  Specifically:
 *
 * - this class bans any normal triangle or quadrilateral that meets a
 *   torus boundary;
 *
 * - this class marks any normal triangle in the link of a vertex on a
 *   torus boundary.
 *
 * This class is only for use with normal or almost normal surfaces, not
 * angle structures.
 *
 * \warning As with BanBoundary, this class only works as expected with
 * vector encodings that explicitly include triangles (e.g., encodings for
 * standard normal or almost normal coordinates).  In quadrilateral or
 * quadrilateral-octagon coordinates it will only ban quadrilaterals or
 * octagons that touch torus boundaries, but it will still allow \e triangles
 * that meet torus boundaries (since triangle types are not counted in these
 * coordinate systems).  The supported() routine will only return \c true for
 * encodings that include triangles.
 *
 * See the BanConstraintBase class notes for details on all member
 * functions and structs.
 *
 * This class is designed to help TreeTraversal manage significant enumeration
 * and search operations, and objects of this class cannot be constructed
 * directly by end users.  To use this class, pass it as the BanConstraint
 * template parameter to one of the tree traversal subclasses
 * (e.g., TreeEnumeration or TreeSingleSolution).
 *
 * \apinotfinal
 *
 * \ifacespython Not present.
 *
 * \ingroup enumerate
 */
class BanTorusBoundary : public BanConstraintBase {
    protected:
        /**
         * Constructs and initialises the \a banned_ and \a marked_ arrays
         * to be entirely \c false, as described in the BanConstraintBase
         * superclass constructor.
         *
         * \warning Before you use this object, the routine init() must be
         * called to fill in the \a banned_ and \a marked_ arrays with the
         * correct data.  Otherwise you will have no banned or marked disc
         * types at all.
         *
         * @param tri the triangulation with which we are working.
         * @param enc the vector encoding being used for this enumeration task.
         * This must be one of the normal or almost normal surface vector
         * encodings known to be supported by the generic TreeTraversal
         * infrastructure.
         */
        BanTorusBoundary(const Triangulation<3>& tri, NormalEncoding enc);

        void init(const int* columnPerm);
        static bool supported(NormalEncoding enc);
};

}

#include "enumerate/treelp.h"

namespace regina {

// Inline functions

template <typename IntType>
inline void LPConstraintNone::Coefficients::fillFinalRows(
        LPMatrix<IntType>& m, unsigned col) const {
}

template <typename IntType>
inline IntType LPConstraintNone::Coefficients::innerProduct(
        const LPMatrix<IntType>&, unsigned) const {
    return 0;
}

template <typename IntType>
inline IntType LPConstraintNone::Coefficients::innerProductOct(
        const LPMatrix<IntType>&, unsigned) const {
    return 0;
}

inline bool LPConstraintNone::addRows(
        LPCol<regina::LPConstraintNone>*,
        const int*, const Triangulation<3>&) {
    return true;
}

template <typename IntType>
inline void LPConstraintNone::constrain(
        LPData<regina::LPConstraintNone, IntType>&, unsigned) {
}

inline bool LPConstraintNone::verify(const NormalSurface&) {
    return true;
}

inline bool LPConstraintNone::verify(const AngleStructure&) {
    return true;
}

inline bool LPConstraintNone::supported(NormalEncoding) {
    return true;
}

template <typename IntType>
inline void LPConstraintEulerPositive::Coefficients::fillFinalRows(
        LPMatrix<IntType>& m, unsigned col) const {
    m.entry(m.rows() - 1, col) = euler;
}

template <typename IntType>
inline IntType LPConstraintEulerPositive::Coefficients::innerProduct(
        const LPMatrix<IntType>& m, unsigned mRow) const {
    IntType ans(m.entry(mRow, m.rows() - 1));
    ans *= euler;
    return ans;
}

template <typename IntType>
inline IntType LPConstraintEulerPositive::Coefficients::innerProductOct(
        const LPMatrix<IntType>& m, unsigned mRow) const {
    // This is called for *two* quad columns (the two quads
    // that combine to give a single octagon).
    //
    // The adjustment in this case is to subtract two from
    // the overall Euler characteristic coefficient for this
    // octagon type (-1 because an octagon has lower Euler
    // characteristic than two quads, and -1 again because
    // we are measuring Euler - #octagons.
    //
    // Happily we can do this by subtracting one from the
    // coefficient in each of the two columns, as
    // implemented below.
    IntType ans(m.entry(mRow, m.rows() - 1));
    ans *= (euler - 1);
    return ans;
}

template <typename IntType>
inline void LPConstraintEulerPositive::constrain(
        LPData<regina::LPConstraintEulerPositive, IntType>& lp,
        unsigned numCols) {
    lp.constrainPositive(numCols - 1);
}

inline bool LPConstraintEulerPositive::verify(const NormalSurface& s) {
    return (s.eulerChar() > 0);
}

inline bool LPConstraintEulerPositive::verify(const AngleStructure&) {
    return false;
}

inline bool LPConstraintEulerPositive::supported(NormalEncoding enc) {
    // Note: storesTriangles() will ensure we are not using angle structures.
    return enc.storesTriangles();
}

template <typename IntType>
inline void LPConstraintEulerZero::Coefficients::fillFinalRows(
        LPMatrix<IntType>& m, unsigned col) const {
    m.entry(m.rows() - 1, col) = euler;
}

template <typename IntType>
inline IntType LPConstraintEulerZero::Coefficients::innerProduct(
        const LPMatrix<IntType>& m, unsigned mRow) const {
    IntType ans(m.entry(mRow, m.rows() - 1));
    ans *= euler;
    return ans;
}

template <typename IntType>
inline IntType LPConstraintEulerZero::Coefficients::innerProductOct(
        const LPMatrix<IntType>& m, unsigned mRow) const {
    // This should never be called, since we never use this
    // constraint with almost normal surfaces.
    // For compilation's sake though, just return the usual inner product.
    return innerProduct(m, mRow);
}

template <typename IntType>
inline void LPConstraintEulerZero::constrain(
        LPData<regina::LPConstraintEulerZero, IntType>& lp, unsigned numCols) {
    lp.constrainZero(numCols - 1);
}

inline bool LPConstraintEulerZero::verify(const NormalSurface& s) {
    return (s.eulerChar() == 0);
}

inline bool LPConstraintEulerZero::verify(const AngleStructure&) {
    return false;
}

inline bool LPConstraintEulerZero::supported(NormalEncoding enc) {
    // Note: storesTriangles() will ensure we are not using angle structures.
    return (enc.storesTriangles() && ! enc.storesOctagons());
}

template <typename IntType>
inline void LPConstraintNonSpun::Coefficients::fillFinalRows(
        LPMatrix<IntType>& m, unsigned col) const {
    m.entry(m.rows() - 2, col) = meridian;
    m.entry(m.rows() - 1, col) = longitude;
}

template <typename IntType>
inline IntType LPConstraintNonSpun::Coefficients::innerProduct(
        const LPMatrix<IntType>& m, unsigned mRow) const {
    IntType ans1(m.entry(mRow, m.rows() - 2));
    ans1 *= meridian;
    IntType ans2(m.entry(mRow, m.rows() - 1));
    ans2 *= longitude;
    ans1 += ans2;
    return ans1;
}

template <typename IntType>
inline IntType LPConstraintNonSpun::Coefficients::innerProductOct(
        const LPMatrix<IntType>& m, unsigned mRow) const {
    // This should never be called, since we never use this
    // constraint with almost normal surfaces.
    // For compilation's sake though, just return the usual
    // inner product.
    return innerProduct(m, mRow);
}

template <typename IntType>
inline void LPConstraintNonSpun::constrain(
        LPData<regina::LPConstraintNonSpun, IntType>& lp, unsigned numCols) {
    lp.constrainZero(numCols - 2);
    lp.constrainZero(numCols - 1);
}

inline bool LPConstraintNonSpun::verify(const NormalSurface& s) {
    return s.isCompact();
}

inline bool LPConstraintNonSpun::verify(const AngleStructure&) {
    return false;
}

inline bool LPConstraintNonSpun::supported(NormalEncoding enc) {
    return ! (enc.storesTriangles() || enc.storesAngles());
}

inline BanConstraintBase::~BanConstraintBase() {
    delete[] banned_;
    delete[] marked_;
}

template <class LPConstraint, typename IntType>
inline void BanConstraintBase::enforceBans(LPData<LPConstraint, IntType>& lp)
        const {
    for (unsigned i = 0; i < lp.coordinateColumns(); ++i)
        if (banned_[i])
            lp.constrainZero(i);
}

inline bool BanConstraintBase::marked(size_t column) const {
    return marked_[column];
}

inline BanBoundary::BanBoundary(const Triangulation<3>& tri,
        NormalEncoding enc) :
        BanConstraintBase(tri, enc) {
}

inline bool BanBoundary::supported(NormalEncoding enc) {
    // Note: storesTriangles() will ensure we are not using angle structures.
    return enc.storesTriangles();
}

inline BanTorusBoundary::BanTorusBoundary(
        const Triangulation<3>& tri, NormalEncoding enc) :
        BanConstraintBase(tri, enc) {
}

inline bool BanTorusBoundary::supported(NormalEncoding enc) {
    // Note: storesTriangles() will ensure we are not using angle structures.
    return enc.storesTriangles();
}

} // namespace regina

#endif
