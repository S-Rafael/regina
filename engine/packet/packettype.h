
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2021, Ben Burton                                   *
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

/*! \file packet/packettype.h
 *  \brief Defines constants for the various packet types known to Regina.
 */

#ifndef __REGINA_PACKETTYPE_H
#ifndef __DOXYGEN
#define __REGINA_PACKETTYPE_H
#endif

#include "regina-core.h"

namespace regina {

/**
 * Represents the different types of packet that are available in Regina.
 *
 * IDs 0-9999 are reserved for future use by Regina.  If you are extending
 * Regina to include your own packet type, you should choose an ID >= 10000.
 *
 * \ingroup packet
 */
enum PacketType {
    /**
     * Represents a container packet, of class Container.
     */
    PACKET_CONTAINER = 1,
    /**
     * Represents a text packet, of class Text.
     */
    PACKET_TEXT = 2,
    /**
     * Represents a normal surface list, of class NormalSurfaces.
     */
    PACKET_NORMALSURFACES = 6,
    /**
     * Represents a script packet, of class Script.
     */
    PACKET_SCRIPT = 7,
    /**
     * Represents a normal surface filter, of class SurfaceFilter or
     * one of its descendant classes.
     */
    PACKET_SURFACEFILTER = 8,
    /**
     * Represents an angle structure list, of class AngleStructures.
     */
    PACKET_ANGLESTRUCTURES = 9,
    /**
     * Represents a PDF document, of class PDF.
     */
    PACKET_PDF = 10,
    /**
     * Represents a normal hypersurface list, of class NormalHypersurfaces.
     */
    PACKET_NORMALHYPERSURFACES = 13,
    /**
     * Represents a triangulation in the embedded SnapPea kernel, of
     * class SnapPeaTriangulation.
     */
    PACKET_SNAPPEATRIANGULATION = 16,
    /**
     * Represents a knot or link in the 3-sphere, of class Link.
     */
    PACKET_LINK = 17,
    /**
     * Represents a 2-dimensional triangulation, of class Triangulation<2>.
     */
    PACKET_TRIANGULATION2 = 15,
    /**
     * Represents a 3-dimensional triangulation, of class Triangulation<3>.
     */
    PACKET_TRIANGULATION3 = 3,
    /**
     * Represents a 4-dimensional triangulation, of class Triangulation<4>.
     */
    PACKET_TRIANGULATION4 = 11,
#ifndef REGINA_LOWDIMONLY
    /**
     * Represents a 5-dimensional triangulation, of class Triangulation<5>.
     */
    PACKET_TRIANGULATION5 = 105,
    /**
     * Represents a 6-dimensional triangulation, of class Triangulation<6>.
     */
    PACKET_TRIANGULATION6 = 106,
    /**
     * Represents a 7-dimensional triangulation, of class Triangulation<7>.
     */
    PACKET_TRIANGULATION7 = 107,
    /**
     * Represents a 8-dimensional triangulation, of class Triangulation<8>.
     */
    PACKET_TRIANGULATION8 = 108,
    /**
     * Represents a 9-dimensional triangulation, of class Triangulation<9>.
     */
    PACKET_TRIANGULATION9 = 109,
    /**
     * Represents a 10-dimensional triangulation, of class Triangulation<10>.
     */
    PACKET_TRIANGULATION10 = 110,
    /**
     * Represents a 11-dimensional triangulation, of class Triangulation<11>.
     */
    PACKET_TRIANGULATION11 = 111,
    /**
     * Represents a 12-dimensional triangulation, of class Triangulation<12>.
     */
    PACKET_TRIANGULATION12 = 112,
    /**
     * Represents a 13-dimensional triangulation, of class Triangulation<13>.
     */
    PACKET_TRIANGULATION13 = 113,
    /**
     * Represents a 14-dimensional triangulation, of class Triangulation<14>.
     */
    PACKET_TRIANGULATION14 = 114,
    /**
     * Represents a 15-dimensional triangulation, of class Triangulation<15>.
     */
    PACKET_TRIANGULATION15 = 115
#endif /* ! REGINA_LOWDIMONLY */
};

/**
 * A class used to query general information about different packet types.
 *
 * This class has become much simpler since Regina 7.0.  It is no longer
 * templated, and instead just offers a name() function that is accessible at
 * both compile time and runtime.
 *
 * \ingroup packet
 */
class PacketInfo {
    public:
        /**
         * Returns a human-readable name for the given packet type.
         *
         * The first letter of the returned string will be upper-case,
         * and all subsequent letters will be lower-case (except for
         * special words such as "PDF" and "SnapPea", which will retain
         * their internal upper-case letters where required).
         *
         * This routine is guaranteed to return a non-null string, even
         * if \a packetType is not one of the PacketType enum values.
         *
         * @param packetType the packet type being queried.
         * @return the name of the given packet type.
         */
        constexpr static const char* name(PacketType packetType) {
            switch (packetType) {
                case PACKET_CONTAINER:
                    return "Container";
                case PACKET_TEXT:
                    return "Text";
                case PACKET_NORMALSURFACES:
                    return "Normal surface list";
                case PACKET_SCRIPT:
                    return "Script";
                case PACKET_SURFACEFILTER:
                    return "Surface filter";
                case PACKET_ANGLESTRUCTURES:
                    return "Angle structure list";
                case PACKET_PDF:
                    return "PDF";
                case PACKET_NORMALHYPERSURFACES:
                    return "Normal hypersurface list";
                case PACKET_SNAPPEATRIANGULATION:
                    return "SnapPea triangulation";
                case PACKET_LINK:
                    return "Link";
                case PACKET_TRIANGULATION2:
                    return "2-manifold triangulation";
                case PACKET_TRIANGULATION3:
                    return "3-manifold triangulation";
                case PACKET_TRIANGULATION4:
                    return "4-manifold triangulation";
            #ifndef REGINA_LOWDIMONLY
                case PACKET_TRIANGULATION5:
                    return "5-manifold triangulation";
                case PACKET_TRIANGULATION6:
                    return "6-manifold triangulation";
                case PACKET_TRIANGULATION7:
                    return "7-manifold triangulation";
                case PACKET_TRIANGULATION8:
                    return "8-manifold triangulation";
                case PACKET_TRIANGULATION9:
                    return "9-manifold triangulation";
                case PACKET_TRIANGULATION10:
                    return "10-manifold triangulation";
                case PACKET_TRIANGULATION11:
                    return "11-manifold triangulation";
                case PACKET_TRIANGULATION12:
                    return "12-manifold triangulation";
                case PACKET_TRIANGULATION13:
                    return "13-manifold triangulation";
                case PACKET_TRIANGULATION14:
                    return "14-manifold triangulation";
                case PACKET_TRIANGULATION15:
                    return "15-manifold triangulation";
            #endif /* ! REGINA_LOWDIMONLY */
                default:
                    return "Unknown";
            }
        }
};

} // namespace regina

#endif

