
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2008, Ben Burton                                   *
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
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include "algebra/nabeliangroup.h"
#include "dim4/dim4triangulation.h"
#include "maths/nmatrixint.h"

namespace regina {

const NAbelianGroup& Dim4Triangulation::getHomologyH1() const {
    if (H1_.known())
        return *H1_.value();

    if (getNumberOfPentachora() == 0)
        return *(H1_ = new NAbelianGroup());

    // Calculate the first homology.
    if (! calculatedSkeleton_)
        calculateSkeleton();

    // Build a presentation matrix.
    // Each non-boundary not-in-forest tetrahedron is a generator.
    // Each non-boundary face is a relation.
    long nBdryTets = 0;
    long nBdryFaces = 0;
    for (BoundaryComponentIterator bit = boundaryComponents_.begin();
            bit != boundaryComponents_.end(); ++bit) {
        nBdryTets += (*bit)->tetrahedra_.size();
        nBdryFaces += (*bit)->faces_.size();
    }

    // Cast away all unsignedness in case we run into problems subtracting.
    long nGens = static_cast<long>(tetrahedra_.size()) - nBdryTets
        - static_cast<long>(pentachora_.size())
        + static_cast<long>(components_.size());
    long nRels = static_cast<long>(faces_.size()) - nBdryFaces;

    NMatrixInt pres(nRels, nGens);

    // Find out which tetrahedron corresponds to which generator.
    long* genIndex = new long[tetrahedra_.size()];
    long i = 0;
    for (TetrahedronIterator tit = tetrahedra_.begin();
            tit != tetrahedra_.end(); ++tit)
        if (! ((*tit)->isBoundary() || (*tit)->inDualMaximalForest()))
            genIndex[tit - tetrahedra_.begin()] = i++;

    // Run through each face and put the corresponding relations into
    // the matrix.
    std::deque<Dim4FaceEmbedding>::const_iterator embit;
    Dim4Pentachoron* pent;
    int facet;
    Dim4Tetrahedron* tet;
    i = 0;
    for (FaceIterator fit = faces_.begin(); fit != faces_.end(); ++fit) {
        if ((*fit)->isBoundary())
            continue;

        // Put in the relation corresponding to this face.
        for (embit = (*fit)->emb_.begin();
                embit != (*fit)->emb_.end(); ++embit) {
            pent = (*embit).getPentachoron();
            facet = (*embit).getVertices()[3];

            tet = pent->tet_[facet];
            if (tet->inDualMaximalForest())
                continue;

            // We define the "direction" for this dual edge to point
            // from embedding tet->emb_[0] to embedding tet->emb_[1].
            //
            // Test whether we are traversing this dual edge forwards or
            // backwards as we walk around the face (*fit).
            if ((tet->emb_[0].getPentachoron() == pent) &&
                    (tet->emb_[0].getTetrahedron() == facet))
                pres.entry(i, genIndex[tet->markedIndex()]) += 1;
            else
                pres.entry(i, genIndex[tet->markedIndex()]) -= 1;
        }

        ++i;
    }

    delete[] genIndex;

    // Build the group from the presentation matrix and tidy up.
    NAbelianGroup* ans = new NAbelianGroup();
    ans->addGroup(pres);
    return *(H1_ = ans);
}

const NAbelianGroup& Dim4Triangulation::getHomologyH2() const {
    if (H2_.known())
        return *H2_.value();

    // TODO: Implement this..!

    return *(H2_ = new NAbelianGroup());
}

} // namespace regina
