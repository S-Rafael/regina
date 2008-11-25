
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

#include "dim4/dim4exampletriangulation.h"
#include "dim4/dim4triangulation.h"

namespace regina {

Dim4Triangulation* Dim4ExampleTriangulation::fourSphere() {
    // Take two pentachora and join their entire boundaries according to
    // the identity map.
    Dim4Triangulation* ans = new Dim4Triangulation();
    ans->setPacketLabel("4-sphere");

    Dim4Pentachoron* p = new Dim4Pentachoron();
    Dim4Pentachoron* q = new Dim4Pentachoron();
    p->joinTo(0, q, NPerm5());
    p->joinTo(1, q, NPerm5());
    p->joinTo(2, q, NPerm5());
    p->joinTo(3, q, NPerm5());
    p->joinTo(4, q, NPerm5());
    ans->addPentachoron(p);
    ans->addPentachoron(q);

    return ans;
}

Dim4Triangulation* Dim4ExampleTriangulation::rp4() {
    Dim4Triangulation* ans = new Dim4Triangulation();
    ans->setPacketLabel("Real projective 4-space");

    // Thanks Ryan, you rock. :)
    Dim4Pentachoron* p = new Dim4Pentachoron();
    Dim4Pentachoron* q = new Dim4Pentachoron();
    Dim4Pentachoron* r = new Dim4Pentachoron();
    Dim4Pentachoron* s = new Dim4Pentachoron();
    p->joinTo(0, s, NPerm5(1,0,3,2,4));
    p->joinTo(1, s, NPerm5(1,0,3,2,4));
    p->joinTo(2, q, NPerm5());
    p->joinTo(3, q, NPerm5());
    p->joinTo(4, r, NPerm5());
    q->joinTo(0, r, NPerm5(1,0,3,2,4));
    q->joinTo(1, r, NPerm5(1,0,3,2,4));
    q->joinTo(4, s, NPerm5());
    r->joinTo(2, s, NPerm5());
    r->joinTo(3, s, NPerm5());
    ans->addPentachoron(p);
    ans->addPentachoron(q);
    ans->addPentachoron(r);
    ans->addPentachoron(s);

    return ans;
}

} // namespace regina

