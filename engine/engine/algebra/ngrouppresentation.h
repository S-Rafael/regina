
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2002, Ben Burton                                   *
 *  For further details contact Ben Burton (benb@acm.org).                *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,        *
 *  MA 02111-1307, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

/*! \file ngrouppresentation.h
 *  \brief Deals with finite presentations of groups.
 */

#ifndef __NGROUPPRESENTATION_H
#ifndef __DOXYGEN
#define __NGROUPPRESENTATION_H
#endif

#include <utility>
#include "utilities/ndoublelist.h"
#include "utilities/ndynamicarray.h"
#include "property/npropertyholder.h"
#include "shareableobject.h"

class NFile;

/**
 * Represents a power of a generator in a group presentation.
 * The first element of this pair is the number that identifies
 * the generator; the second element is the exponent to which the
 * generator is raised.
 *
 * \ifaces Not present.
 */
typedef std::pair<unsigned long, long> NGroupExpressionTerm;

/**
 * Represents an expression involving generators from a group presentation.
 * An expression is represented as a sequence of powers of generators all of
 * which are multiplied in order.  Each power of a generator
 * corresponds to an individual NGroupExpressionTerm.
 *
 * For instance, the expression <tt>g1^2 g3^-1 g6</tt> contains the
 * three terms <tt>g1^2</tt>, <tt>g3^-1</tt> and <tt>g6^1</tt> in that
 * order.
 */
class NGroupExpression : public ShareableObject {
    private:
        NDoubleList<NGroupExpressionTerm> terms;
            /** The terms that make up this expression. */

    public:
        /**
         * Creates a new expression with no terms.
         */
        NGroupExpression();
        /**
         * Creates a new expression that is a clone of the given
         * expression.
         *
         * @param cloneMe the expression to clone.
         */
        NGroupExpression(const NGroupExpression& cloneMe);
        
        /**
         * Returns the list of terms in this expression.
         * These are the actual terms stored internally; any
         * modifications made to this list will show up in the
         * expression itself.
         *
         * \ifaces Not present.
         *
         * @return the list of terms.
         */
        const NDoubleList<NGroupExpressionTerm>& getTerms() const;
        /**
         * Returns the number of terms in this expression.
         *
         * @return the number of terms.
         */
        unsigned long getNumberOfTerms() const;
        /**
         * Returns the term at the given index in this expression.
         * Index 0 represents the first term, index 1
         * represents the second term and so on.
         *
         * \warning This routine is <i>O(n)</i> where \a n is the number
         * of terms in this expression.
         *
         * \ifaces Not present.
         *
         * @param index the index of the term to return; this must be
         * between 0 and getNumberOfTerms()-1 inclusive.
         * @return the requested term.
         */
        const NGroupExpressionTerm& getTerm(unsigned long index) const;
        /**
         * Returns the generator corresonding to the
         * term at the given index in this expression.
         * Index 0 represents the first term, index 1
         * represents the second term and so on.
         *
         * \warning This routine is <i>O(n)</i> where \a n is the number
         * of terms in this expression.
         *
         * @param index the index of the term to return; this must be
         * between 0 and getNumberOfTerms()-1 inclusive.
         * @return the number of the requested generator.
         */
        unsigned long getGenerator(unsigned long index) const;
        /**
         * Returns the exponent corresonding to the
         * term at the given index in this expression.
         * Index 0 represents the first term, index 1
         * represents the second term and so on.
         *
         * \warning This routine is <i>O(n)</i> where \a n is the number
         * of terms in this expression.
         *
         * @param index the index of the term to return; this must be
         * between 0 and getNumberOfTerms()-1 inclusive.
         * @return the requested exponent.
         */
        long getExponent(unsigned long index) const;

        /**
         * Adds the given term to the beginning of this expression.
         *
         * \ifaces Not present.
         *
         * @param term the term to add.
         */
        void addTermFirst(const NGroupExpressionTerm& term);
        /**
         * Adds the given term to the beginning of this expression.
         *
         * @param generator the number of the generator corresponding to
         * the new term.
         * @param exponent the exponent to which the given generator is
         * raised.
         */
        void addTermFirst(unsigned long generator, long exponent);
        /**
         * Adds the given term to the end of this expression.
         *
         * \ifaces Not present.
         *
         * @param term the term to add.
         */
        void addTermLast(const NGroupExpressionTerm& term);
        /**
         * Adds the given term to the end of this expression.
         *
         * @param generator the number of the generator corresponding to
         * the new term.
         * @param exponent the exponent to which the given generator is
         * raised.
         */
        void addTermLast(unsigned long generator, long exponent);

        /**
         * Returns a newly created expression that is the inverse of
         * this expression.  The terms will be reversed and the
         * exponents negated.
         *
         * @return the inverse of this expression.
         */
        NGroupExpression* inverse() const;
        /**
         * Returns a newly created expression that is
         * this expression raised to the given power.
         * Note that the given exponent may be positive, zero or negative.
         * 
         * @param exponent the power to which this expression should be raised.
         * @return this expression raised to the given power.
         */
        NGroupExpression* power(long exponent) const;
        /**
         * Simplifies this expression.
         * Adjacent powers of the same generator will be combined, and
         * terms with an exponent of zero will be removed.
         * Note that it is \e not assumed that the underlying group is
         * abelian.
         *
         * You may declare that the expression is cyclic, in which case
         * it is assumed that terms may be moved from the back to the
         * front and vice versa.  Thus expression <tt>g1 g2 g1 g2 g1</tt>
         * simplifies to <tt>g1^2 g2 g1 g2</tt> if it is cyclic, but does not
         * simplify at all if it is not cyclic.
         *
         * \ifacescorba All parameters are compulsory.
         *
         * @param cyclic \c true if and only if the expression may be
         * assumed to be cyclic.
         * @return \c true if and only if this expression was changed.
         */
        bool simplify(bool cyclic = false);
        /**
         * Replaces every occurrence of the given generator with the
         * given substite expression.  If the given generator was found,
         * the expression will be simplified once the substitution is
         * complete.
         *
         * \ifacescorba All parameters are compulsory.
         *
         * @param generator the generator to be replaced.
         * @param expansion the substitute expression that will replace
         * every occurrence of the given generator.
         * @param cyclic \c true if and only if the expression may be
         * assumed to be cyclic; see simplify() for further details.
         * @return \c true if and only if any substitutions were made.
         */
        bool substitute(unsigned long generator,
            const NGroupExpression& expansion, bool cyclic = false);
    
        /**
         * Writes this expression to the given file.
         *
         * \pre The given file is currently opened for writing.
         *
         * \ifaces Not present.
         *
         * @param out the file to which to write.
         */
        void writeToFile(NFile& out) const;
        /**
         * Reads an expression from the given file.
         * The expression will be newly allocated, and responsibility for
         * its destruction lies with the caller of this routine.
         *
         * \pre The given file is currently opened for reading.
         *
         * \ifaces Not present.
         *
         * @param in the file from which to read.
         * @return a newly allocated expression read from the given file, or
         * 0 if problems arose.
         */
        static NGroupExpression* readFromFile(NFile& in);

        /**
         * The text representation will be of the form
         * <tt>g2^4 g13^-5 g4</tt>.
         */
        virtual void writeTextShort(ostream& out) const;
};

/**
 * Represents a finite presentation of a group.
 *
 * A presentation consists of a number of generators and a set of
 * relations between these generators that together define the group.
 *
 * If there are \a g generators, they will be numbered 0, 1, ..., <i>g</i>-1.
 *
 * \todo \optlong The simplification routines really need work!
 */
class NGroupPresentation : public ShareableObject, public NPropertyHolder {
    protected:
        unsigned long nGenerators;
            /**< The number of generators. */
        NDynamicArray<NGroupExpression*> relations;
            /**< The relations between the generators. */
    
    public:
        /**
         * Creates a new presentation with no generators and no
         * relations.
         */
        NGroupPresentation();
        /**
         * Creates a clone of the given group presentation.
         *
         * @param cloneMe the presentation to clone.
         */
        NGroupPresentation(const NGroupPresentation& cloneMe);
        /**
         * Destroys the group presentation.
         * All relations that are stored will be deallocated.
         */
        virtual ~NGroupPresentation();

        /**
         * Adds one or more generators to the group presentation.
         * If the new presentation has \a g generators, the new
         * generators will be numbered <i>g</i>-1, <i>g</i>-2 and so on.
         *
         * \ifacescorba All parameters are compulsory.
         *
         * @param numToAdd the number of generators to add.
         * @return the number of generators in the new presentation.
         */
        unsigned long addGenerator(unsigned long numToAdd = 1);
        /**
         * Adds the given relation to the group presentation.
         * The relation must be of the form <tt>expression = 1</tt>.
         *
         * This presentation will take ownership of the given
         * expression, may change it and will be responsible for its
         * deallocation.
         *
         * @param rel the expression that the relation sets to 1; for
         * instance, if the relation is <tt>g1^2 g2 = 1</tt> then this
         * parameter should be the expression <tt>g1^2 g2</tt>.
         */
        void addRelation(NGroupExpression* rel);

        /**
         * Returns the number of generators in this group presentation.
         *
         * @return the number of generators.
         */
        unsigned long getNumberOfGenerators() const;
        /**
         * Returns the number of relations in this group presentation.
         *
         * @return the number of relations.
         */
        unsigned long getNumberOfRelations() const;
        /**
         * Returns the relation at the given index in this group
         * presentation.  The relation will be of the form <tt>expresson
         * = 1</tt>.
         *
         * @param index the index of the desired relation; this must be
         * between 0 and getNumberOfRelations()-1 inclusive.
         * @return the expression that the requested relation sets to 1;
         * for instance, if the relation is <tt>g1^2 g2 = 1</tt> then
         * this will be the expression <tt>g1^2 g2</tt>.
         */
        const NGroupExpression& getRelation(unsigned long index) const;

        /**
         * Attempts to simplify the group presentation as intelligently
         * as possible without further input.
         *
         * As with the NTriangulation routine of the same name, this
         * routine is not as intelligent as it makes out to be.
         * In fact, currently it does almost nothing.
         *
         * \todo \featurelong Make this simplification more effective.
         *
         * @return \c true if and only if the group presentation was changed.
         */
        bool intelligentSimplify();

        /**
         * Attempts to recognise the group corresponding to this
         * presentation.  This routine is much more likely to be
         * successful if you have already called intelligentSimplify().
         *
         * Note that the presentation might be simplified a little
         * during the execution of this routine, although not nearly as
         * much as would be done by intelligentSimplify().
         *
         * \todo \featurelong Make this recognition more effective.
         *
         * @return a simple string representation of the group if it is
         * recognised, or an empty string if the group is not
         * recognised.
         */
        NString recogniseGroup();

        /**
         * Writes this group presentation to the given file.
         *
         * \pre The given file is currently opened for writing.
         *
         * \ifaces Not present.
         *
         * @param out the file to which to write.
         */
        void writeToFile(NFile& out) const;
        /**
         * Reads a group presentation from the given file.
         * The presentation will be newly allocated, and responsibility for
         * its destruction lies with the caller of this routine.
         *
         * \pre The given file is currently opened for reading.
         *
         * \ifaces Not present.
         *
         * @param in the file from which to read.
         * @return a newly allocated presentation read from the given file, or
         * 0 if problems arose.
         */
        static NGroupPresentation* readFromFile(NFile& in);

        virtual void writeTextShort(ostream& out) const;
        virtual void writeTextLong(ostream& out) const;

    protected:
        virtual void readIndividualProperty(NFile& infile, unsigned propType);
        virtual void initialiseAllProperties();
};

// Inline functions for NGroupExpression

inline NGroupExpression::NGroupExpression() {
}

inline const NDoubleList<NGroupExpressionTerm>& NGroupExpression::getTerms()
        const {
    return terms;
}

inline unsigned long NGroupExpression::getNumberOfTerms() const {
    return terms.size();
}

inline const NGroupExpressionTerm& NGroupExpression::getTerm(
        unsigned long index) const {
    return terms[index];
}

inline unsigned long NGroupExpression::getGenerator(unsigned long index)
        const {
    return terms[index].first;
}

inline long NGroupExpression::getExponent(unsigned long index) const {
    return terms[index].second;
}

inline void NGroupExpression::addTermFirst(const NGroupExpressionTerm& term) {
    terms.addFirst(term);
}

inline void NGroupExpression::addTermFirst(unsigned long generator,
        long exponent) {
    terms.addFirst(NGroupExpressionTerm(generator, exponent));
}

inline void NGroupExpression::addTermLast(const NGroupExpressionTerm& term) {
    terms.addLast(term);
}

inline void NGroupExpression::addTermLast(unsigned long generator,
        long exponent) {
    terms.addLast(NGroupExpressionTerm(generator, exponent));
}

// Inline functions for NGroupPresentation

inline NGroupPresentation::NGroupPresentation() : nGenerators(0) {
}

inline NGroupPresentation::~NGroupPresentation() {
    relations.flushAndDelete();
}

inline unsigned long NGroupPresentation::addGenerator(unsigned long num) {
    return (nGenerators += num);
}

inline void NGroupPresentation::addRelation(NGroupExpression* rel) {
    relations.addLast(rel);
}

inline unsigned long NGroupPresentation::getNumberOfGenerators() const {
    return nGenerators;
}

inline unsigned long NGroupPresentation::getNumberOfRelations() const {
    return relations.size();
}

inline const NGroupExpression& NGroupPresentation::getRelation(
        unsigned long index) const {
    return *relations[index];
}

inline void NGroupPresentation::writeTextShort(ostream& out) const {
    out << "Group presentation: " << nGenerators << " generators, "
        << relations.size() << " relations";
}

inline void NGroupPresentation::readIndividualProperty(NFile&, unsigned) {
}

inline void NGroupPresentation::initialiseAllProperties() {
}

#endif

