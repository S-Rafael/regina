
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

#include "enumerate/enumconstraints.h"
#include "hypersurface/normalhypersurface.h"
#include "maths/matrix.h"
#include "maths/rational.h"
#include "surfaces/normalsurface.h" // for quadDefn[][].
#include "triangulation/dim4.h"

namespace regina {

EnumConstraints makeEmbeddedConstraints(
        const Triangulation<4>& triangulation, HyperCoords coords) {
    const HyperEncoding enc(coords);

    EnumConstraints ans(30 * triangulation.size());

    size_t base = (enc.storesTetrahedra() ? 5 : 0);
    size_t c = 0;
    for (size_t pent = 0; pent < triangulation.size(); ++pent) {
        for (int edge1 = 0; edge1 < 10; ++edge1)
            for (int i = 0; i < 3; ++i) {
                int edge2 = Edge<4>::edgeNumber[
                    Edge<4>::edgeVertex[edge1][0]][
                    Triangle<4>::triangleVertex[edge1][i]];
                if (edge1 < edge2) {
                    ans[c].insert(ans[c].end(), base + edge1);
                    ans[c].insert(ans[c].end(), base + edge2);
                    ++c;
                }
                edge2 = Edge<4>::edgeNumber[
                    Edge<4>::edgeVertex[edge1][1]][
                    Triangle<4>::triangleVertex[edge1][i]];
                if (edge1 < edge2) {
                    ans[c].insert(ans[c].end(), base + edge1);
                    ans[c].insert(ans[c].end(), base + edge2);
                    ++c;
                }
            }
        base += enc.block();
    }

    return ans;
}

std::optional<MatrixInt> makeMatchingEquations(
        const Triangulation<4>& triangulation, HyperCoords coords) {
    switch (coords) {
        case HS_STANDARD:
        {
            const size_t nCoords = 15 * triangulation.size();
            // Seven equations per non-boundary facet.
            // T_boundary + 2 T_internal = 5 P
            const size_t nEquations = 7 * (5 * triangulation.size() -
                triangulation.countTetrahedra());
            MatrixInt ans(nEquations, nCoords);

            // Run through each internal facet and add the corresponding seven
            // equations.
            size_t row = 0;
            for (Tetrahedron<4>* tet : triangulation.tetrahedra()) {
                if (! tet->isBoundary()) {
                    size_t pos0 = 15 * tet->embedding(0).pentachoron()->index();
                    size_t pos1 = 15 * tet->embedding(1).pentachoron()->index();
                    Perm<5> perm0 = tet->embedding(0).vertices();
                    Perm<5> perm1 = tet->embedding(1).vertices();

                    // Triangles:
                    for (int i=0; i<4; i++) {
                        // Tetrahedra that meet this triangle:
                        ++ans.entry(row, pos0 + perm0[i]);
                        --ans.entry(row, pos1 + perm1[i]);

                        // Prisms that meet this triangle:
                        ++ans.entry(row, pos0 + 5 +
                            Edge<4>::edgeNumber[perm0[i]][perm0[4]]);
                        --ans.entry(row, pos1 + 5 +
                            Edge<4>::edgeNumber[perm1[i]][perm1[4]]);
                        ++row;
                    }

                    // Quads:
                    for (int i=0; i<3; i++) {
                        // Prisms that meet this quad:
                        ++ans.entry(row, pos0 + 5 +
                            Edge<4>::edgeNumber[perm0[quadDefn[i][0]]]
                                                [perm0[quadDefn[i][1]]]);
                        ++ans.entry(row, pos0 + 5 +
                            Edge<4>::edgeNumber[perm0[quadDefn[i][2]]]
                                                [perm0[quadDefn[i][3]]]);
                        --ans.entry(row, pos1 + 5 +
                            Edge<4>::edgeNumber[perm1[quadDefn[i][0]]]
                                                [perm1[quadDefn[i][1]]]);
                        --ans.entry(row, pos1 + 5 +
                            Edge<4>::edgeNumber[perm1[quadDefn[i][2]]]
                                                [perm1[quadDefn[i][3]]]);
                        ++row;
                    }
                }
            }
            return ans;
        }
        case HS_PRISM:
        {
        }
        default:
            return std::nullopt;
    }
}

} // namespace regina
