
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
 *                                                                        *
 *  Copyright (c) 1999-2014, Ben Burton                                   *
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

#include "face-bindings.h"

void addFace() {
    def("binomSmall", regina::binomSmall);

    // Note: FaceEmbedding<4, subdim> does not get the aliases
    // pentachoron() or getPentachoron().
    // This is fixed in the 4-manifolds branch, since dim=4 is handled
    // separately.
    addFace<4, 0>("Face4_0", "FaceEmbedding4_0");
    addFace<4, 1>("Face4_1", "FaceEmbedding4_1");
    addFace<4, 2>("Face4_2", "FaceEmbedding4_2");
    addFace<4, 3>("Face4_3", "FaceEmbedding4_3");

    addFace<5, 0>("Face5_0", "FaceEmbedding5_0");
    addFace<5, 1>("Face5_1", "FaceEmbedding5_1");
    addFace<5, 2>("Face5_2", "FaceEmbedding5_2");
    addFace<5, 3>("Face5_3", "FaceEmbedding5_3");
    addFace<5, 4>("Face5_4", "FaceEmbedding5_4");

    addFace<6, 0>("Face6_0", "FaceEmbedding6_0");
    addFace<6, 1>("Face6_1", "FaceEmbedding6_1");
    addFace<6, 2>("Face6_2", "FaceEmbedding6_2");
    addFace<6, 3>("Face6_3", "FaceEmbedding6_3");
    addFace<6, 4>("Face6_4", "FaceEmbedding6_4");
    addFace<6, 5>("Face6_5", "FaceEmbedding6_5");

    addFace<7, 0>("Face7_0", "FaceEmbedding7_0");
    addFace<7, 1>("Face7_1", "FaceEmbedding7_1");
    addFace<7, 2>("Face7_2", "FaceEmbedding7_2");
    addFace<7, 3>("Face7_3", "FaceEmbedding7_3");
    addFace<7, 4>("Face7_4", "FaceEmbedding7_4");
    addFace<7, 5>("Face7_5", "FaceEmbedding7_5");
    addFace<7, 6>("Face7_6", "FaceEmbedding7_6");

    addFace<8, 0>("Face8_0", "FaceEmbedding8_0");
    addFace<8, 1>("Face8_1", "FaceEmbedding8_1");
    addFace<8, 2>("Face8_2", "FaceEmbedding8_2");
    addFace<8, 3>("Face8_3", "FaceEmbedding8_3");
    addFace<8, 4>("Face8_4", "FaceEmbedding8_4");
    addFace<8, 5>("Face8_5", "FaceEmbedding8_5");
    addFace<8, 6>("Face8_6", "FaceEmbedding8_6");
    addFace<8, 7>("Face8_7", "FaceEmbedding8_7");

    addFace<9, 0>("Face9_0", "FaceEmbedding9_0");
    addFace<9, 1>("Face9_1", "FaceEmbedding9_1");
    addFace<9, 2>("Face9_2", "FaceEmbedding9_2");
    addFace<9, 3>("Face9_3", "FaceEmbedding9_3");
    addFace<9, 4>("Face9_4", "FaceEmbedding9_4");
    addFace<9, 5>("Face9_5", "FaceEmbedding9_5");
    addFace<9, 6>("Face9_6", "FaceEmbedding9_6");
    addFace<9, 7>("Face9_7", "FaceEmbedding9_7");
    addFace<9, 8>("Face9_8", "FaceEmbedding9_8");

    // The bindings for dim >= 10 are in separate files,
    // to help reduce compiler memory consumption.
}

