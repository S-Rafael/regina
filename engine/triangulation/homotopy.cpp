
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

#include "triangulation/ntriangulation.h"

namespace regina {

NGroupPresentation* NTriangulation::computeFundamentalGroup() const {

    NGroupPresentation* ans = new NGroupPresentation();

    if (getNumberOfTetrahedra() == 0)
        return ans;

    // Find a maximal forest in the dual 1-skeleton.
    // Note that this will ensure the skeleton has been calculated.
    std::set<NTriangle*> forest;
    maximalForestInDualSkeleton(forest);

    // Each non-boundary not-in-forest triangle is a generator.
    // Each non-boundary edge is a relation.
    unsigned long nBdryTri = 0;
    for (BoundaryComponentIterator bit = boundaryComponents_.begin();
            bit != boundaryComponents_.end(); bit++)
        nBdryTri += (*bit)->getNumberOfTriangles();
    long nGens = getNumberOfTriangles() - nBdryTri - forest.size();

    // Insert the generators.
    ans->addGenerator(nGens);

    // Find out which triangle corresponds to which generator.
    long *genIndex = new long[getNumberOfTriangles()];
    long i = 0;
    for (TriangleIterator fit = triangles_.begin(); fit != triangles_.end(); fit++)
        if ((*fit)->isBoundary() || forest.count(*fit))
            genIndex[fit - triangles_.begin()] = -1;
        else
            genIndex[fit - triangles_.begin()] = i++;

    // Run through each edge and put the relations in the matrix.
    std::deque<NEdgeEmbedding>::const_iterator embit;
    NTetrahedron* currTet;
    NTriangle* triangle;
    int currTetFace;
    long triGenIndex;
    NGroupExpression* rel;
    for (EdgeIterator eit = edges_.begin(); eit != edges_.end(); eit++)
        if (! (*eit)->isBoundary()) {
            // Put in the relation corresponding to this edge.
            rel = new NGroupExpression();
            for (embit = (*eit)->getEmbeddings().begin();
                    embit != (*eit)->getEmbeddings().end(); embit++) {
                currTet = (*embit).getTetrahedron();
                currTetFace = (*embit).getVertices()[2];
                triangle = currTet->getTriangle(currTetFace);
                triGenIndex = genIndex[triangleIndex(triangle)];
                if (triGenIndex >= 0) {
                    if ((triangle->getEmbedding(0).getTetrahedron() == currTet) &&
                            (triangle->getEmbedding(0).getTriangle() == currTetFace))
                        rel->addTermLast(triGenIndex, 1);
                    else
                        rel->addTermLast(triGenIndex, -1);
                }
            }
            ans->addRelation(rel);
        }

    // Tidy up.
    delete[] genIndex;

    return ans;
}


const NGroupPresentation& NTriangulation::getFundamentalGroup(
                                                         bool simplify) const {

    if (simplify) {
	if (fundamentalGroup_.known())
	    return *fundamentalGroup_.value();
    } else {
	if (unsimplifiedFundamentalGroup_.known())
	    return *unsimplifiedFundamentalGroup_.value();
    }

    NGroupPresentation* ans = computeFundamentalGroup();

    if (simplify)
	ans->intelligentSimplify();

    if (simplify)
	return *(fundamentalGroup_ = ans);
    else
	return *(unsimplifiedFundamentalGroup_ = ans);
}

} // namespace regina

