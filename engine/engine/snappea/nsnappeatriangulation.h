
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2004, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
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

/*! \file nsnappeatriangulation.h
 *  \brief Allows Regina triangulations to interact with the SnapPea kernel.
 */

#ifndef __NSNAPPEATRIANGULATION_H
#ifndef __DOXYGEN
#define __NSNAPPEATRIANGULATION_H
#endif

#include "shareableobject.h"

// Forward declaration of SnapPea structures.
struct Triangulation;

namespace regina {

class NTriangulation;

/**
 * \weakgroup triangulation
 * @{
 */

/**
 * Offers direct access to the SnapPea kernel from within Regina.
 *
 * An object of this class represents a 3-manifold triangulation, stored in
 * SnapPea's internal format.  Such an object may be constructed by cloning
 * either a standard Regina triangulation or another SnapPea triangulation.
 *
 * Note that not all Regina triangulations can be represented in SnapPea
 * format.  You should always call isNull() to test whether any
 * Regina-to-SnapPea conversion was successful.
 *
 * Portions of the SnapPea kernel have been built into Regina as of
 * version 4.2.  SnapPea is copyright (c) 1991-2000 by Jeff Weeks, and is
 * distributed under the terms of the GNU General Public License.
 *
 * See http://www.geometrygames.org/SnapPea/ for further information on
 * SnapPea.
 */
class NSnapPeaTriangulation : public ShareableObject {
    private:
        ::Triangulation* snappeaData;
            /**< The triangulation stored in SnapPea's native format. */
        static bool kernelMessages;
            /**< Should the SnapPea kernel write diagnostic messages to
                 standard output? */

    public:
        /**
         * Creates a SnapPea clone of the given SnapPea triangulation.
         * This SnapPea triangulation stands independent of \a tri,
         * so this triangulation will not be affected if \a tri is later
         * changed or destroyed.
         *
         * If \a tri is a null triangulation then this will be a null
         * triangulation also.  See isNull() for further details.
         *
         * Note that the tetrahedron and vertex numbers might be changed
         * in the new SnapPea triangulation.
         *
         * @param tri the SnapPea triangulation to clone.
         */
        NSnapPeaTriangulation(const NSnapPeaTriangulation& tri);

        /**
         * Creates a SnapPea clone of the given Regina triangulation.
         * This SnapPea triangulation stands independent of \a tri,
         * so this triangulation will not be affected if \a tri is later
         * changed or destroyed.
         *
         * Note that, since Regina is written with a different purpose
         * from SnapPea, not all Regina triangulations can be
         * represented in SnapPea format.  If the conversion is
         * unsuccessful, this will be marked as a null triangulation.
         * You should always test isNull() to determine whether the
         * conversion was successful.
         *
         * Note also that the tetrahedron and vertex numbers might be changed
         * in the new SnapPea triangulation.
         *
         * @param tri the Regina triangulation to clone.
         */
        NSnapPeaTriangulation(const NTriangulation& tri);

        /**
         * Destroys this triangulation.  All internal SnapPea data will
         * also be destroyed.
         */
        ~NSnapPeaTriangulation();

        /**
         * Determines whether this triangulation contains valid SnapPea
         * data.
         *
         * A null SnapPea triangulation can occur when converting unusual
         * types of Regina triangulation into SnapPea format, since
         * Regina is written to deal with different types of triangulations
         * from SnapPea.
         *
         * @return \c true if this is a null triangulation, or \c false
         * if this triangulation contains valid SnapPea data.
         */
        bool isNull() const;

        /**
         * Computes the volume of the underlying 3-manifold.
         *
         * @return the volume of the underlying 3-manifold, or 0 if this
         * is a null triangulation.
         */
        double volume() const;

        /**
         * Dumps the underlying SnapPea data to standard output.
         *
         * This routine should be regarded primarily as a diagnostic tool
         * for investigating how the SnapPea kernel has modified and/or
         * analysed a triangulation.
         *
         * Note that the SnapPea data is written using C-style output
         * (i.e., using the \a stdout file pointer), which may or may not
         * cause unexpected behaviour when used in conjunction with
         * \a std::cout.
         *
         * If this triangulation does not contain any valid SnapPea
         * data, this routine will do nothing.  See isNull() for further
         * details.
         *
         * @param filename the name of the SnapPea file to write.
         */
        void dump() const;

        /**
         * Saves the underlying triangulation as a native SnapPea file.
         * Like dump(), this routine is provided primarily as a diagnostic
         * tool.
         *
         * Passing an empty string as the filename will cause the
         * SnapPea data to be written to standard output, just like
         * dump().  See the dump() documentation for caveats when
         * combining the C-style output of this routine with \a std::cout.
         *
         * If this triangulation does not contain any valid SnapPea
         * data, this routine will do nothing.  See isNull() for further
         * details.
         *
         * @param filename the name of the SnapPea file to write.
         */
        void saveAsSnapPea(const char* filename) const;

        /**
         * Computes the volume of the underlying 3-manifold and
         * estimates the accuracy of the answer.
         *
         * @param precision used to return an estimate of the number of
         * decimal places of accuracy in the calculated volume.
         *
         * @return the volume of the underlying 3-manifold, or 0 if this
         * is a null triangulation.
         */
        double volume(int& precision) const;

        virtual void writeTextShort(std::ostream& out) const;

        /**
         * Returns whether or not the SnapPea kernel writes diagnostic
         * messages to standard output.
         *
         * By default such diagnostic messages are enabled.  To disable
         * them, call disableKernelMessages().
         *
         * @return \c true if and only if diagonstic messages are enabled.
         */
        static bool kernelMessagesEnabled();

        /**
         * Configures whether or not the SnapPea kernel should write
         * diagnostic messages to standard output.
         *
         * By default such diagnostic messages are enabled.
         *
         * @param enabled \c true if diagnostic messages should be
         * enabled, or \c false otherwise.
         */
        static void enableKernelMessages(bool enabled = true);

        /**
         * Specifies that the SnapPea kernel should not write diagnostic
         * messages to standard output.
         *
         * Calling this routine is equivalent to calling
         * enableKernelMessages(false).
         *
         * Note that diagnostic messages are enabled by default.
         */
        static void disableKernelMessages();

    private:
        /**
         * Creates a new raw SnapPea structure mirroring the given Regina
         * triangulation.
         *
         * Note that the tetrahedron and vertex numbers might be changed
         * in the new SnapPea triangulation.
         *
         * The resulting structure should be destroyed by calling
         * free_triangulation() in the SnapPea kernel.
         *
         * Note that not all Regina triangulations can be successfully
         * converted into SnapPea triangulations.  If the conversion is
         * unsuccessful, 0 will be returned.
         *
         * @param tri the Regina triangulation to clone.
         * @return a corresponding SnapPea structure, or 0 if the
         * conversion was unsuccessful.
         */
        static ::Triangulation* NSnapPeaTriangulation::reginaToSnapPea(
            const NTriangulation& tri);
};

/*@}*/

// Inline functions for NSnapPeaTriangulation

inline bool NSnapPeaTriangulation::isNull() const {
    return (snappeaData == 0);
}

inline void NSnapPeaTriangulation::dump() const {
    saveAsSnapPea("");
}

inline bool NSnapPeaTriangulation::kernelMessagesEnabled() {
    return kernelMessages;
}

inline void NSnapPeaTriangulation::enableKernelMessages(bool enabled) {
    kernelMessages = enabled;
}

inline void NSnapPeaTriangulation::disableKernelMessages() {
    kernelMessages = false;
}

} // namespace regina

#endif

