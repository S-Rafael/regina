
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

#include <list>
#include <sstream>

#include "enumerate/treetraversal.h"
#include "packet/container.h"
#include "subcomplex/snappedball.h"
#include "surfaces/normalsurface.h"
#include "surfaces/normalsurfaces.h"
#include "triangulation/dim3.h"

namespace regina {

std::vector<std::unique_ptr<Triangulation<3>>> Triangulation<3>::summands(
        bool setLabels) const {
    // Precondition checks.
    if (! (isValid() && isClosed() && isConnected()))
        return { };

    // Do we already know the answer?
    if (threeSphere_.has_value() && *threeSphere_)
        return { };

    bool initOrientable = isOrientable();

    // Make a working copy, simplify and record the initial homology.
    auto working = std::make_unique<Triangulation<3>>(*this, false);
    working->intelligentSimplify();

    unsigned long initZ, initZ2, initZ3;
    {
        const AbelianGroup& homology = working->homology();
        initZ = homology.rank();
        initZ2 = homology.torsionRank(2);
        initZ3 = homology.torsionRank(3);
    }

    // Start crushing normal spheres.
    std::stack<std::unique_ptr<Triangulation<3>>> toProcess;
    toProcess.push(std::move(working));

    std::vector<std::unique_ptr<Triangulation<3>>> primeComponents;

    while (! toProcess.empty()) {
        // INV: Our triangulation is the connected sum of all the
        // children of toProcess, all the elements of primeComponents
        // and possibly some copies of S2xS1, S2x~S1, RP3, and/or L(3,1).

        auto processing = std::move(toProcess.top());
        toProcess.pop();

        // Find a normal 2-sphere to crush.
        auto sphere = processing->nonTrivialSphereOrDisc();
        if (sphere) {
            std::unique_ptr<Triangulation<3>> crushed(sphere->crush());

            if (! crushed->isValid()) {
                // We must have had an embedded two-sided projective plane.
                // Abort.
                //
                // All elements of toProcess will be deleted automatically
                // with toProcess itself (which is on the stack).
                throw regina::UnsolvedCase("Found an embedded two-sided "
                    "projective plane");
            }

            crushed->intelligentSimplify();

            // Insert each component of the crushed triangulation back
            // into the list to process.
            if (! crushed->isEmpty()) {
                if (crushed->isConnected())
                    toProcess.push(std::move(crushed));
                else {
                    for (auto& comp : crushed->triangulateComponents())
                        toProcess.push(std::move(comp));
                }
            }
        } else {
            // We have no non-trivial normal 2-spheres!
            // The triangulation is 0-efficient (and prime).
            // Is it a 3-sphere?
            if (! processing->isOrientable()) {
                // Definitely not a sphere.
                primeComponents.push_back(std::move(processing));
            } else {
                // Orientable, and so possibly a sphere.  Test this precisely.
                if (processing->countVertices() > 1) {
                    // Proposition 5.1 of Jaco & Rubinstein's 0-efficiency
                    // paper:  If a closed orientable triangulation T is
                    // 0-efficient then either T has one vertex or T is a
                    // 3-sphere with precisely two vertices.
                    //
                    // It follows then that this is a 3-sphere.
                    // Toss it away.
                } else {
                    // Now we have a closed orientable one-vertex 0-efficient
                    // triangulation.
                    // We have to look for an almost normal sphere.
                    //
                    // From the proof of Proposition 5.12 in Jaco & Rubinstein's
                    // 0-efficiency paper, we see that we can restrict our
                    // search to octagonal almost normal surfaces.
                    // Furthermore, from the result in the quadrilateral-octagon
                    // coordinates paper, we can restrict this search further
                    // to vertex octagonal almost normal surfaces in
                    // quadrilateral-octagonal space.
                    if (processing->octagonalAlmostNormalSphere()) {
                        // It's a 3-sphere.  Toss this component away.
                    } else {
                        // It's a non-trivial prime component!
                        primeComponents.push_back(std::move(processing));
                    }
                }
            }
        }
    }

    // Run a final homology check and put back our missing S2xS1, S2x~S1,
    // RP3 and L(3,1) terms.
    unsigned long finalZ = 0, finalZ2 = 0, finalZ3 = 0;
    for (const auto& c : primeComponents) {
        const AbelianGroup& homology = c->homology();
        finalZ += homology.rank();
        finalZ2 += homology.torsionRank(2);
        finalZ3 += homology.torsionRank(3);
    }

    while (finalZ++ < initZ) {
        working = std::make_unique<Triangulation<3>>();
        if (initOrientable) {
            // Build S2 x S1.
            working->insertLayeredLensSpace(0, 1);
        } else {
            // Build S2 x~ S1.
            Tetrahedron<3>* t0 = working->newTetrahedron();
            Tetrahedron<3>* t1 = working->newTetrahedron();
            t0->join(0, t1, Perm<4>(0, 1, 3, 2));
            t0->join(1, t1, Perm<4>(0, 1, 3, 2));
            t0->join(2, t1, Perm<4>(1, 3, 2, 0));
            t0->join(3, t1, Perm<4>(2, 0, 1, 3));
        }
        primeComponents.push_back(std::move(working));
        irreducible_ = false; // Implied by the S2xS1 or S2x~S1 summand.
        zeroEfficient_ = false; // Implied by the S2xS1 or S2x~S1 summand.
    }
    while (finalZ2++ < initZ2) {
        working = std::make_unique<Triangulation<3>>();
        working->insertLayeredLensSpace(2, 1);
        primeComponents.push_back(std::move(working));
        zeroEfficient_ = false; // Implied by the RP3 summand.
    }
    while (finalZ3++ < initZ3) {
        working = std::make_unique<Triangulation<3>>();
        working->insertLayeredLensSpace(3, 1);
        primeComponents.push_back(std::move(working));
    }

    // All done!
    if (setLabels) {
        size_t whichComp = 1;
        for (const auto& c : primeComponents) {
            std::ostringstream label;
            label << "Summand #" << (whichComp++);
            c->setLabel(adornedLabel(label.str()));
        }
    }

    // Set irreducibility while we're at it.
    if (primeComponents.size() > 1) {
        threeSphere_ = false;
        irreducible_ = false;
        zeroEfficient_ = false;
    } else if (primeComponents.size() == 1) {
        threeSphere_ = false;
        if (! irreducible_.has_value()) {
            // If our manifold is S2xS1 or S2x~S1 then it is *not* irreducible;
            // however, in this case we will have already set irreducible
            // to false when putting back the S2xS1 or S2x~S1 summands above
            // (and therefore irreducible.known() will be true).
            irreducible_ = true;
        }
    } else if (primeComponents.size() == 0) {
        threeSphere_ = true;
        irreducible_ = true;
        haken_ = false;
    }

    return primeComponents;
}

bool Triangulation<3>::isSphere() const {
    if (threeSphere_.has_value())
        return *threeSphere_;

    // Basic property checks.
    if (! (isValid() && isClosed() && isOrientable() && isConnected() &&
            (! isEmpty()))) {
        return *(threeSphere_ = false);
    }

    // Check homology and fundamental group.
    // Better simplify first, which means we need a clone.
    auto working = std::make_unique<Triangulation<3>>(*this, false);
    working->intelligentSimplify();

    // The Poincare conjecture!
    if (working->fundamentalGroup().countGenerators() == 0) {
        threeSphere_ = true;

        // Some other things that come for free:
        irreducible_ = true;
        haken_ = false;

        return true;
    }

    // We could still have a trivial group but not know it.
    // At least we can at least check homology precisely.
    if (! working->homology().isTrivial()) {
        return *(threeSphere_ = false);
    }

    // Time for some more heavy machinery.  On to normal surfaces.
    std::stack<std::unique_ptr<Triangulation<3>>> toProcess;;
    toProcess.push(std::move(working));

    while (! toProcess.empty()) {
        // INV: Our triangulation is the connected sum of all the
        // children of toProcess.  Each of these children has trivial
        // homology (and therefore we have no S2xS1 / RP3 / L(3,1)
        // summands to worry about).

        auto processing = std::move(toProcess.top());
        toProcess.pop();

        // Find a normal 2-sphere to crush.
        auto sphere = processing->nonTrivialSphereOrDisc();
        if (sphere) {
            std::unique_ptr<Triangulation<3>> crushed(sphere->crush());

            crushed->intelligentSimplify();

            // Insert each component of the crushed triangulation in the
            // list to process.
            if (! crushed->isEmpty()) {
                if (crushed->isConnected())
                    toProcess.push(std::move(crushed));
                else {
                    for (auto& comp : crushed->triangulateComponents())
                        toProcess.push(std::move(comp));
                }
            }
        } else {
            // We have no non-trivial normal 2-spheres!
            // The triangulation is 0-efficient.
            // We can now test directly whether we have a 3-sphere.
            if (processing->countVertices() > 1) {
                // Proposition 5.1 of Jaco & Rubinstein's 0-efficiency
                // paper:  If a closed orientable triangulation T is
                // 0-efficient then either T has one vertex or T is a
                // 3-sphere with precisely two vertices.
                //
                // It follows then that this is a 3-sphere.
                // Toss it away.
            } else {
                // Now we have a closed orientable one-vertex 0-efficient
                // triangulation.
                // We have to look for an almost normal sphere.
                //
                // From the proof of Proposition 5.12 in Jaco & Rubinstein's
                // 0-efficiency paper, we see that we can restrict our
                // search to octagonal almost normal surfaces.
                // Furthermore, from the result in the quadrilateral-octagon
                // coordinates paper, we can restrict this search further
                // to vertex octagonal almost normal surfaces in
                // quadrilateral-octagonal space.
                if (processing->octagonalAlmostNormalSphere()) {
                    // It's a 3-sphere.  Toss this component away.
                } else {
                    // It's not a 3-sphere.  We're done!
                    return *(threeSphere_ = false);
                }
            }
        }
    }

    // Our triangulation is the connected sum of 0 components!
    threeSphere_ = true;

    // Some other things that we get for free:
    irreducible_ = true;
    haken_ = false;

    return true;
}

bool Triangulation<3>::knowsSphere() const {
    if (threeSphere_.has_value())
        return true;

    // Run some very fast prelimiary tests before we give up and say no.
    if (! (isValid() && isClosed() && isOrientable() && isConnected())) {
        threeSphere_ = false;
        return true;
    }

    // More work is required.
    return false;
}

bool Triangulation<3>::isBall() const {
    if (threeBall_.has_value())
        return *threeBall_;

    // Basic property checks.
    if (! (isValid() && hasBoundaryTriangles() && isOrientable() && isConnected()
            && countBoundaryComponents() == 1
            && boundaryComponents().front()->eulerChar() == 2)) {
        threeBall_ = false;
        return false;
    }

    // Pass straight to isSphere (which in turn will check faster things
    // like homology before pulling out the big guns).
    //
    // Cone the boundary to a point (i.e., fill it with a ball), then
    // call isSphere() on the resulting closed triangulation.

    Triangulation<3> working(*this, false);
    working.intelligentSimplify();
    working.finiteToIdeal();

    // Simplify again in case our coning was inefficient.
    working.intelligentSimplify();

    threeBall_ = working.isSphere();
    return *threeBall_;
}

bool Triangulation<3>::knowsBall() const {
    if (threeBall_.has_value())
        return true;

    // Run some very fast prelimiary tests before we give up and say no.
    if (! (isValid() && hasBoundaryTriangles() && isOrientable() && isConnected()
            && countBoundaryComponents() == 1
            && boundaryComponents().front()->eulerChar() == 2)) {
        threeBall_ = false;
        return true;
    }

    // More work is required.
    return false;
}

bool Triangulation<3>::isSolidTorus() const {
    if (solidTorus_.has_value())
        return *solidTorus_;

    // Basic property checks.
    if (! (isValid() && isOrientable() && isConnected() &&
            countBoundaryComponents() == 1 &&
            boundaryComponents().front()->eulerChar() == 0 &&
            boundaryComponents().front()->isOrientable()))
        return *(solidTorus_ = false);

    // If it's ideal, make it a triangulation with real boundary.
    // If it's not ideal, clone it anyway so we can modify it.
    auto working = std::make_unique<Triangulation<3>>(*this, false);
    working->intelligentSimplify();
    if (working->isIdeal()) {
        working->idealToFinite();
        working->intelligentSimplify();
    }

    // Check homology.
    if (! (working->homology().isZ()))
        return *(solidTorus_ = false);

    // So:
    // We are valid, orientable, compact and connected, with H1 = Z.
    // There is exactly one boundary component, and this is a torus.

    // Note that the homology results imply that this is not a connected
    // sum of something with S2xS1 (otherwise we would have two Z terms
    // in the homology: one from the torus boundary and one from the S2xS1).
    // This observation simplifies the crushing cases later on.

    // Pull out the big guns: normal surface time.
    while (true) {
        // INVARIANT: working is homeomorphic to our original manifold.
        if (working->countVertices() > 1) {
            // Try *really* hard to get to a 1-vertex triangulation,
            // since this will make nonTrivialSphereOrDisc() much
            // faster (it will be able to use linear programming).
            working->intelligentSimplify();
            if (working->countVertices() > 1) {
                working->barycentricSubdivision();
                working->intelligentSimplify();
                working->intelligentSimplify();
            }
        }

        // Find a non-trivial normal disc or sphere.
        auto s = working->nonTrivialSphereOrDisc();
        if (! s) {
            // No non-trivial normal disc.  This cannot be a solid torus.
            return *(solidTorus_ = false);
        }

        // Crush it and see what happens.
        // Given what we know about the manifold so far, the only things
        // that can happen during crushing are:
        // - undo connected sum decompositions;
        // - cut along properly embedded discs;
        // - gain and/or lose 3-balls and/or 3-spheres.
        std::unique_ptr<Triangulation<3>> crushed(s->crush());
        working.reset();

        crushed->intelligentSimplify();

        for (auto& comp : crushed->triangulateComponents()) {
            // Examine each connected component after crushing.
            if (comp->isClosed()) {
                // A closed piece.
                // Must be a 3-sphere, or else we didn't have a solid torus.
                if (! comp->isSphere()) {
                    return *(solidTorus_ = false);
                }
            } else if (comp->countBoundaryComponents() > 1) {
                // Multiple boundaries on the same component.
                // This should never happen, since it implies there was
                // an S2xS1 summand.
                std::cerr << "ERROR: S2xS1 summand detected in "
                    "isSolidTorus() that should not exist." << std::endl;

                // At any rate, it means we did not have a solid torus.
                return *(solidTorus_ = false);
            } else if (comp->boundaryComponent(0)->eulerChar() == 2) {
                // A component with sphere boundary.
                // Must be a 3-ball, or else we didn't have a solid torus.
                if (! comp->isBall()) {
                    return *(solidTorus_ = false);
                }
            } else {
                // The only other possibility is a component with torus
                // boundary.  We should only see at most one of these.
                //
                // Unless some other non-trivial component was split off
                // (i.e., a non-ball and/or non-sphere that will be
                // detected separately in the tests above), this
                // component must be identical to our original manifold.
                if (working) {
                    std::cerr << "ERROR: Multiple torus boundary "
                        "components detected in isSolidTorus(), which "
                        "should not be possible." << std::endl;
                }
                working = std::move(comp);
            }
        }

        if (! working) {
            // We have reduced everything down to balls and spheres.
            // The only way this can happen is if we had a solid torus
            // (and we crushed and/or cut along a compressing disc
            // during the crushing operation).
            return *(solidTorus_ = true);
        }

        // We have the original manifold in working, but this time with
        // fewer tetrahedra.  Around we go again.
    }
}

bool Triangulation<3>::knowsSolidTorus() const {
    if (solidTorus_.has_value())
        return true;

    // Run some very fast prelimiary tests before we give up and say no.
    if (! (isValid() && isOrientable() && isConnected())) {
        solidTorus_ = false;
        return true;
    }

    if (countBoundaryComponents() != 1) {
        solidTorus_ = false;
        return true;
    }

    if (boundaryComponents().front()->eulerChar() != 0 ||
            (! boundaryComponents().front()->isOrientable())) {
        solidTorus_ = false;
        return true;
    }

    // More work is required.
    return false;
}


bool Triangulation<3>::isTxI() const {
    // This call to knowsTxI checks basic things including validity and also
    // the number topology of the boundary components.
    if (knowsTxI())
        return *TxI_;

    Triangulation<3> working(*this, false);
    working.intelligentSimplify();
    working.idealToFinite();
    working.intelligentSimplify();

    // If it's not a homology T2xI, we're done.
    if ((! working.homology().isFree(2)) || (! working.homologyRel().isZ())) {
        return *(TxI_ = false);
    }

    // At this point we should already have boundary components with
    // one vertex each.
    // But out of an abundance of caution, we ensure this is the case.
    working.minimiseBoundary();

    // We have a homology T2xI with a pair of two-triangle boundaries.
    // So we should move on to the meat of the algorithm, testing Dehn fillings.

    // First we collect a boundary edge, which lets us fetch the two triangles
    // on either side along with permutations that show how they glue together.
    BoundaryComponent<3>* bc = working.boundaryComponents().front();
    Edge<3>* e = bc->edge(0);
    const FaceEmbedding<3, 1>& front = e->embedding(0);
    const FaceEmbedding<3, 1>& back = e->embedding(e->degree() - 1);
    Tetrahedron<3>* t0 = front.tetrahedron();
    Tetrahedron<3>* t1 = back.tetrahedron();
    Perm<4> p0 = front.vertices();
    Perm<4> p1 = back.vertices();
    p1 = p1 * Perm<4>(1, 0, 3, 2);

    // Now p0, p1 map {0,1,2} to the vertices of t0, t1 in a symmetric way.
    // Each boundary edge is the image of (i,j) on t0 and (j,i) on t1 for
    // distinct i,j in {0,1,2}.
    //
    // To do the three fillings, we fold the two triangles together in each
    // of the three possible ways (each of which involves swapping one of the
    // pairs 01, 12, 20 in the preimage of permutations p0, p1).
    for (int i = 0; i < 3; ++i) {
        t0->join(p0[3], t1, p1 * Perm<4>(i, (i+1) % 3) * p0.inverse());
        if (! working.isSolidTorus())
            return *(TxI_ = false);
        t0->unjoin(p0[3]);
    }

    // All three fillings give a solid torus!
    return *(TxI_ = true);
}

bool Triangulation<3>::knowsTxI() const {
    if (TxI_.has_value())
        return true;

    // Run some very fast prelimiary tests before we give up and say no.
    if (! (isValid() && isOrientable() && isConnected())) {
        TxI_ = false;
        return true;
    }

    if (countBoundaryComponents() != 2) {
        TxI_ = false;
        return true;
    }

    if (boundaryComponents().front()->eulerChar() != 0 ||
            (! boundaryComponents().front()->isOrientable()) ||
            boundaryComponents().back()->eulerChar() != 0 ||
            (! boundaryComponents().back()->isOrientable())) {
        TxI_ = false;
        return true;
    }

    // More work is required.
    return false;
}

Packet* Triangulation<3>::makeZeroEfficient() {
    // Extract a connected sum decomposition.
    auto ans = summands(true);
    if (ans.size() > 1) {
        // Composite!
        Container* connSum = new Container();
        connSum->setLabel(adornedLabel("Decomposition"));
        for (auto& s : ans)
            connSum->insertChildLast(s.release());
        return connSum;
    } else if (ans.size() == 1) {
        // Prime.
        if (! isIsomorphicTo(*ans.front()))
            swap(*ans.front());
        return nullptr;
    } else {
        // 3-sphere.
        if (size() > 1) {
            removeAllTetrahedra();
            insertLayeredLensSpace(1,0);
        }
        return nullptr;
    }
}

bool Triangulation<3>::isIrreducible() const {
    if (irreducible_.has_value())
        return *irreducible_;

    // Precondition checks.
    if (! (isValid() && isClosed() && isOrientable() && isConnected()))
        return false;

    // We will essentially carry out a connected sum decomposition, but
    // instead of keeping prime summands we will just count them and
    // throw them away.
    unsigned long summands = 0;

    // Make a working copy, simplify and record the initial homology.
    auto working = std::make_unique<Triangulation<3>>(*this, false);
    working->intelligentSimplify();

    unsigned long Z, Z2, Z3;
    {
        const AbelianGroup& homology = working->homology();
        Z = homology.rank();
        Z2 = homology.torsionRank(2);
        Z3 = homology.torsionRank(3);
    }

    // Start crushing normal spheres.
    std::stack<std::unique_ptr<Triangulation<3>>> toProcess;
    toProcess.push(std::move(working));

    while (! toProcess.empty()) {
        // INV: Our triangulation is the connected sum of all the
        // children of toProcess, all the prime components that we threw away,
        // and possibly some copies of S2xS1, RP3 and/or L(3,1).

        // Work with the last child.
        auto processing = std::move(toProcess.top());
        toProcess.pop();

        // Find a normal 2-sphere to crush.
        auto sphere = processing->nonTrivialSphereOrDisc();
        if (sphere) {
            std::unique_ptr<Triangulation<3>> crushed(sphere->crush());

            crushed->intelligentSimplify();

            // Insert each component of the crushed triangulation back
            // into the list to process.
            if (! crushed->isEmpty()) {
                if (crushed->isConnected())
                    toProcess.push(std::move(crushed));
                else {
                    for (auto& comp : crushed->triangulateComponents())
                        toProcess.push(std::move(comp));
                }
            }
        } else {
            // We have no non-trivial normal 2-spheres!
            // The triangulation is 0-efficient (and prime).
            // Is it a 3-sphere?
            if (processing->countVertices() > 1) {
                // Proposition 5.1 of Jaco & Rubinstein's 0-efficiency
                // paper:  If a closed orientable triangulation T is
                // 0-efficient then either T has one vertex or T is a
                // 3-sphere with precisely two vertices.
                //
                // It follows then that this is a 3-sphere.
                // Toss it away.
            } else {
                // Now we have a closed orientable one-vertex 0-efficient
                // triangulation.
                // We have to look for an almost normal sphere.
                //
                // From the proof of Proposition 5.12 in Jaco & Rubinstein's
                // 0-efficiency paper, we see that we can restrict our
                // search to octagonal almost normal surfaces.
                // Furthermore, from the result in the quadrilateral-octagon
                // coordinates paper, we can restrict this search further
                // to vertex octagonal almost normal surfaces in
                // quadrilateral-octagonal space.
                if (processing->octagonalAlmostNormalSphere()) {
                    // It's a 3-sphere.  Toss this component away.
                } else {
                    // It's a non-trivial prime component!
                    // Note that this will never be an S2xS1 summand;
                    // those get crushed away entirely (we account for
                    // them later).
                    if (summands > 0) {
                        // We have found more than one prime component.
                        threeSphere_ = false; // Implied by reducibility.
                        zeroEfficient_ = false; // Implied by reducibility.
                        return *(irreducible_ = false);
                    }
                    ++summands;

                    // Note which parts of our initial homology we have
                    // now accounted for.
                    const AbelianGroup& h1 = processing->homology();
                    Z -= h1.rank();
                    Z2 -= h1.torsionRank(2);
                    Z3 -= h1.torsionRank(3);

                    // Toss away our prime summand and keep going.
                }
            }
        }
    }

    // Run a final homology check: were there any additional S2xS1, RP3
    // or L(3,1) terms?
    if (Z > 0) {
        // There were S2xS1 summands that were crushed away.
        // The manifold must be reducible.
        threeSphere_ = false; // Implied by reducibility.
        zeroEfficient_ = false; // Implied by reducibility.
        return *(irreducible_ = false);
    }
    if (summands + Z2 + Z3 > 1) {
        // At least two summands were found and/or crushed away: the
        // manifold must be composite.
        threeSphere_ = false; // Implied by reducibility.
        zeroEfficient_ = false; // Implied by reducibility.
        return *(irreducible_ = false);
    }

    // There are no S2xS1 summands, and the manifold is prime.
    return *(irreducible_ = true);
}

bool Triangulation<3>::knowsIrreducible() const {
    return irreducible_.has_value();
}

bool Triangulation<3>::hasCompressingDisc() const {
    if (compressingDisc_.has_value())
        return *compressingDisc_;

    // Some sanity checks; also enforce preconditions.
    if (! hasBoundaryTriangles())
        return *(compressingDisc_ = false);
    if ((! isValid()) || isIdeal())
        return *(compressingDisc_ = false);

    long minBdryEuler = 2;
    for (auto bc : boundaryComponents()) {
        if (bc->eulerChar() < minBdryEuler)
            minBdryEuler = bc->eulerChar();
    }
    if (minBdryEuler == 2)
        return *(compressingDisc_ = false);

    // Off we go.
    // Work with a simplified triangulation.
    // We keep this as a pointer because we will be switching and changing
    // triangulations.
    std::unique_ptr<Triangulation<3>> use(new Triangulation<3>(*this, false));
    use->intelligentSimplify();

    // Try for a fast answer first.
    if (use->hasSimpleCompressingDisc())
        return *(compressingDisc_ = true);

    // Nope.  Decide whether we can use the fast linear programming
    // machinery or whether we need to do a full vertex surface enumeration.
    if (use->isOrientable() && use->countBoundaryComponents() == 1) {
        Triangulation<3>* crush;
        Triangulation<3>* comp;

        while (true) {
            use->intelligentSimplify();

            if (use->countVertices() > 1) {
                // Try harder.
                use->barycentricSubdivision();
                use->intelligentSimplify();
                if (use->countVertices() > 1) {
                    // Fall back to a full vertex enumeration.
                    // This mirrors the code for non-orientable
                    // triangulations; see that later block for details.
                    NormalSurfaces q(*use, NS_STANDARD);

                    for (const NormalSurface& s : q) {
                        if (s.isCompressingDisc(true))
                            return *(compressingDisc_ = true);
                    }

                    // No compressing discs!
                    return *(compressingDisc_ = false);
                }
            }

            TreeSingleSoln<LPConstraintEulerPositive> search(*use, NS_STANDARD);
            if (! search.find()) {
                // No compressing discs!
                return *(compressingDisc_ = false);
            }

            crush = search.buildSurface().crush();
            use.reset();

            for (auto& comp : crush->triangulateComponents()) {
                if (comp->countBoundaryComponents() == 1 &&
                        comp->boundaryComponent(0)->eulerChar()
                        == minBdryEuler) {
                    // This must be our original manifold.
                    use = std::move(comp);
                    break;
                }
            }
            delete crush;

            if (! use) {
                // We must have compressed.
                return *(compressingDisc_ = true);
            }

            // Around we go again, but this time with a smaller triangulation.
        }
    } else {
        // Sigh.  Enumerate all vertex normal surfaces.
        //
        // Hum, are we allowed to do this in quad space?  Jaco and Tollefson
        // use standard coordinates.  Jaco, Letscher and Rubinstein mention
        // quad space, but don't give details (which I'd prefer to see).
        // Leave it in standard coordinates for now.
        NormalSurfaces q(*use, NS_STANDARD);

        // Run through all vertex surfaces looking for a compressing disc.
        for (const NormalSurface& s : q) {
            // Use the fact that all vertex normal surfaces are connected.
            if (s.isCompressingDisc(true))
                return *(compressingDisc_ = true);
        }

        // No compressing discs!
        return *(compressingDisc_ = false);
    }
}

bool Triangulation<3>::knowsCompressingDisc() const {
    if (compressingDisc_.has_value())
        return true;

    // Quickly check for non-spherical boundary components before we give up.
    for (auto bc : boundaryComponents())
        if (bc->eulerChar() < 2)
            return false;

    // All boundary components are 2-spheres.
    compressingDisc_ = false;
    return true;
}

bool Triangulation<3>::hasSimpleCompressingDisc() const {
    // Some sanity checks; also enforce preconditions.
    if (! hasBoundaryTriangles())
        return false;
    if ((! isValid()) || isIdeal())
        return false;

    // Off we go.
    // Work with a simplified triangulation.
    Triangulation<3> use(*this, false);
    use.intelligentSimplify();

    // Check to see whether any component is a one-tetrahedron solid torus.
    for (Component<3>* c : use.components())
        if (c->size() == 1 && c->countTriangles() == 3 &&
                c->countVertices() == 1) {
            // Because we know the triangulation is valid, this rules out
            // all one-tetrahedron triangulations except for LST(1,2,3).
            return *(compressingDisc_ = true);
        }

    // Open up as many boundary triangles as possible (to make it easier to
    // find simple compressing discs).
    bool opened = true;
    while (opened) {
        opened = false;
        for (Triangle<3>* t : use.triangles())
            if (use.openBook(t, true, true)) {
                opened = true;
                break;
            }
    }

    // How many boundary spheres do we currently have?
    // This is important because we test whether a disc is a compressing
    // disc by cutting along it and looking for any *new* boundary
    // spheres that might result.
    unsigned long origSphereCount = 0;
    for (auto bc : use.boundaryComponents())
        if (bc->eulerChar() == 2)
            ++origSphereCount;

    // Look for a single internal triangle surrounded by three boundary edges.
    // It doesn't matter whether the edges and/or vertices are distinct.
    Edge<3> *e0, *e1, *e2;
    unsigned long newSphereCount;
    for (Triangle<3>* t : use.triangles()) {
        if (t->isBoundary())
            continue;

        e0 = t->edge(0);
        e1 = t->edge(1);
        e2 = t->edge(2);
        if (! (e0->isBoundary() && e1->isBoundary() && e2->isBoundary()))
            continue;

        // This could be a compressing disc.
        // Cut along the triangle to be sure.
        const TriangleEmbedding<3>& emb = t->front();

        Triangulation<3> cut(use, false);
        cut.tetrahedron(emb.tetrahedron()->markedIndex())->unjoin(
            emb.triangle());

        // If we don't see a new boundary component, the disc boundary is
        // non-separating in the manifold boundary and is therefore a
        // non-trivial curve.
        if (cut.countBoundaryComponents() == use.countBoundaryComponents())
            return *(compressingDisc_ = true);

        newSphereCount = 0;
        for (auto bc : cut.boundaryComponents())
            if (bc->eulerChar() == 2)
                ++newSphereCount;

        // Was the boundary of the disc non-trivial?
        if (newSphereCount == origSphereCount)
            return *(compressingDisc_ = true);
    }

    // Look for a tetrahedron with two faces folded together, giving a
    // degree-one edge on the inside and a boundary edge on the outside.
    // The boundary edge on the outside will surround a disc that cuts
    // right through the tetrahedron.
    for (Tetrahedron<3>* t : use.simplices_) {
        std::optional<SnappedBall> ball = SnappedBall::recognise(t);
        if (! ball)
            continue;

        int equator = ball->equatorEdge();
        if (! t->edge(equator)->isBoundary())
            continue;

        // This could be a compressing disc.
        // Cut through the tetrahedron to be sure.
        // We do this by removing the tetrahedron, and then plugging
        // both holes on either side of the disc with new copies of the
        // tetrahedron.
        int upper = ball->boundaryFace(0);

        Tetrahedron<3>* adj = t->adjacentTetrahedron(upper);
        if (! adj) {
            // The disc is trivial.
            continue;
        }

        Triangulation<3> cut(use, false);
        cut.tetrahedron(t->markedIndex())->unjoin(upper);
        Tetrahedron<3>* tet = cut.newTetrahedron();
        tet->join(Edge<3>::edgeVertex[equator][0], tet, Perm<4>(
            Edge<3>::edgeVertex[equator][0], Edge<3>::edgeVertex[equator][1]));
        tet->join(upper, cut.tetrahedron(adj->markedIndex()),
            t->adjacentGluing(upper));

        // If we don't see a new boundary component, the disc boundary is
        // non-separating in the manifold boundary and is therefore a
        // non-trivial curve.
        if (cut.countBoundaryComponents() == use.countBoundaryComponents())
            return *(compressingDisc_ = true);

        newSphereCount = 0;
        for (auto bc : cut.boundaryComponents())
            if (bc->eulerChar() == 2)
                ++newSphereCount;

        // Was the boundary of the disc non-trivial?
        if (newSphereCount == origSphereCount)
            return *(compressingDisc_ = true);
    }

    // Nothing found.
    return false;
}

namespace {
    /**
     * Used to sort candidate incompressible surfaces by Euler characteristic.
     * Surfaces with smaller genus (i.e., larger Euler characteristic)
     * are to be processed first.
     */
    struct SurfaceID {
        long index;
            /**< Which surface in the list are we referring to? */
        long euler;
            /**< What is its Euler characteristic? */

        inline bool operator < (const SurfaceID& rhs) const {
            return (euler > rhs.euler ||
                (euler == rhs.euler && index < rhs.index));
        }
    };
}

bool Triangulation<3>::isHaken() const {
    if (haken_.has_value())
        return *haken_;

    // Check basic preconditions.
    if (! (isValid() && isOrientable() && isClosed() && isConnected()))
        return false;

    // Irreducibility is not a precondition, but we promise to return
    // false immediately if the triangulation is not irreducible.
    // Do not set the property in this situation.
    if (! isIrreducible())
        return false;

    // Okay: we are closed, connected, orientable and irreducible.
    // Move to a copy of this triangulation, which we can mess with.
    Triangulation<3> t(*this, false);
    t.intelligentSimplify();

    // First check for an easy answer via homology:
    if (t.homology().rank() > 0) {
        threeSphere_ = false; // Implied by Hakenness.
        return *(haken_ = true);
    }

    // Enumerate vertex normal surfaces in quad coordinates.
    // std::cout << "Enumerating surfaces..." << std::endl;
    NormalSurfaces list(t, NS_QUAD);

    // Run through each surface, one at a time.
    // Sort them first however, so we process the (easier) smaller genus
    // surfaces first.
    SurfaceID* id = new SurfaceID[list.size()];
    unsigned i;
    for (i = 0; i < list.size(); ++i) {
        id[i].index = i;
        id[i].euler = list.surface(i).eulerChar().longValue();
    }
    std::sort(id, id + list.size());

    for (i = 0; i < list.size(); ++i) {
        // std::cout << "Testing surface " << i << "..." << std::endl;
        if (list.surface(id[i].index).isIncompressible()) {
            delete[] id;
            threeSphere_ = false; // Implied by Hakenness.
            return *(haken_ = true);
        }
    }

    delete[] id;
    return *(haken_ = false);
}

bool Triangulation<3>::knowsHaken() const {
    return haken_.has_value();
}

} // namespace regina

