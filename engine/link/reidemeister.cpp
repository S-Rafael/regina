
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2016, Ben Burton                                   *
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

#include "link/link.h"

namespace regina {

bool Link::r1(Crossing* crossing, bool check, bool perform) {
    // Note that, for a planar knot projection, if crossing->next(1)
    // returns to the same crossing then it must be the lower strand.

    if (! crossing) {
        // The move cannot be performed.
        // We should just return false, but only if check is true.
        return ! check;
    }

    if (crossing->next(1).crossing() == crossing) {
        // The move is legal.
        if (! perform)
            return true;

        if (crossing->prev(1).crossing() == crossing) {
            // This is a 1-crossing component, and we will convert it to a
            // zero-crossing unknot component.
            for (auto it = components_.begin(); it != components_.end(); ++it)
                if (it->crossing() == crossing) {
                    // We found the component!
                    *it = StrandRef();
                    break;
                }
        } else {
            // The twist runs from the upper strand to the lower.
            StrandRef from = crossing->prev_[1];
            StrandRef to = crossing->next_[0];
            from.crossing()->next_[from.strand()] = to;
            to.crossing()->prev_[to.strand()] = from;

            // Ensure that no component uses this crossing as it starting point.
            for (auto it = components_.begin(); it != components_.end(); ++it)
                if (it->crossing() == crossing) {
                    *it = to;
                    break;
                }
        }
    } else if (crossing->prev(1).crossing() == crossing) {
        // The move is legal.
        if (! perform)
            return true;

        // The twist runs from the lower strand to the upper.
        StrandRef from = crossing->prev_[0];
        StrandRef to = crossing->next_[1];
        from.crossing()->next_[from.strand()] = to;
        to.crossing()->prev_[to.strand()] = from;

        // Ensure that no component uses this crossing as it starting point.
        for (auto it = components_.begin(); it != components_.end(); ++it)
            if (it->crossing() == crossing) {
                *it = to;
                break;
            }
    } else {
        // The move cannot be performed.
        return ! check;
    }

    // Destroy the crossing entirely.
    crossings_.erase(crossings_.begin() + crossing->index());
    delete crossing;

    return true;
}

bool Link::r1(StrandRef arc, int side, int sign, bool check, bool perform) {
    if (! arc) {
        // A null reference.  Find the first zero-crossing component.
        for (auto it = components_.begin(); it != components_.end(); ++it)
            if (! *it) {
                // Found it!
                if (perform) {
                    Crossing* c = new Crossing(sign);
                    c->next_[0] = c->prev_[0] = StrandRef(c, 1);
                    c->next_[1] = c->prev_[1] = StrandRef(c, 0);

                    crossings_.push_back(c);
                    *it = StrandRef(c, 1);
                }

                return true;
            }

        // We didn't find a zero-crossing component.
        // This is an invalid move.
        // We should just return false, but only if check is true.
        return ! check;
    }

    // We have an actual arc, which means the move is always legal.
    if (! perform)
        return true;

    // Insert the twist.
    Crossing* c = new Crossing(sign);
    StrandRef to = arc.next();
    if ((side == 0 && sign > 0) || (side == 1 && sign < 0)) {
        // The link follows (c, lower) then (c, upper).
        c->prev_[0] = arc;
        c->next_[1] = to;
        to.crossing()->prev_[to.strand()] = c->next_[0] = StrandRef(c, 1);
        arc.crossing()->next_[arc.strand()] = c->prev_[1] = StrandRef(c, 0);
    } else {
        // The link follows (c, upper) then (c, lower).
        c->prev_[1] = arc;
        c->next_[0] = to;
        to.crossing()->prev_[to.strand()] = c->next_[1] = StrandRef(c, 0);
        arc.crossing()->next_[arc.strand()] = c->prev_[0] = StrandRef(c, 1);
    }
    crossings_.push_back(c);

    return true;
}

bool Link::r2(StrandRef arc, bool check, bool perform) {
    if (! arc) {
        // The move cannot be performed.
        // We should just return false, but only if check is true.
        return ! check;
    }

    StrandRef to = arc.next();

    // The following test also ensures (by planarity) that [arc] and [to]
    // represent different crossings.
    if (arc.strand() != to.strand())
        return ! check;

    StrandRef arc2 = arc;
    arc2.jump();

    // Does the second arc run forwards or backwards?
    // Note that, for a planar knot diagram, we are guaranteed that if
    // the other strand of [arc] *does* also connect with [to], then it does
    // so on the other strand of [to].
    bool forward = (arc2.next().crossing() == to.crossing());
    bool backward = (arc2.prev().crossing() == to.crossing());

    if (! (forward || backward)) {
        // The move cannot be performed.
        return ! check;
    }

    // The move can be performed!
    if (! perform)
        return true;

    // The situation: (arc, arc2) represent opposite strands of one crossing,
    // and (to, to2) represent opposite strands of another crossing.
    // (The variable to2 has not yet been set, but we will do this shortly.)
    //
    // If forward is true, then we have:
    //
    //   arc   ->  to
    //   arc2  ->  to2
    //
    // If backward is true, then we have:
    //
    //   arc   ->  to
    //   arc2  <-  to2
    //
    // Of course it is possible to have both situations simultaneously;
    // this happens iff, after the move, the second arc becomes a
    // zero-crossing unknot component.

    // When we strip crossings out, there are some pathological cases
    // where it's not just (essentially) pulling two items out of a linked
    // list:
    //
    // (i)   Both arcs represent different components and the first arc
    //       becomes a zero-crossing component; i.e., we have to -> arc.
    //       By planarity, this is true iff
    //       arc.prev().crossing() == to.crossing().
    //
    // (ii)  Both arcs represent different components and the second arc
    //       becomes a zero-crossing component; i.e., we have to2 -> arc2.
    //       Again by planarity, this is true iff (forward && backward).
    //
    // (iii) Both (i) and (ii) occur simultaneously; i.e., we have two
    //       two-crossing components that both become zero-crossing components.
    //
    // (iv)  Both arcs represent the same component, and are directly
    //       linked together as arc -> to -> to2 -> arc2.
    //       By planarity, this is true iff
    //       to.next().crossing() == to.crossing().
    //
    // (v)   Both arcs represent the same component, and are directly
    //       linked together as to2 -> arc2 -> arc -> to.
    //       By planarity, this is true iff
    //       arc.prev().crossing() == arc.crossing().
    //
    // (vi)  Both (iv) and (v) occur simultaneously; i.e., we have a
    //       four-crossing component that becomes a zero-crossing component.
    //
    // Note that, again by planarity, the only way to link both arcs
    // together directly is by method (iv) or (v) above.  That is, we
    // cannot have to joined with arc2, or to2 joined with arc.

    // The following booleans track whether the first and/or second arcs
    // become unknot components.  This is needed so that we can do the
    // right thing when we update components_ later on.
    bool unknot1 = false;
    bool unknot2 = false;

    // Strip the two crossings out of the link.
    StrandRef x, y;

    // First we handle cases (iv, v, vi) separately.
    if (to.next().crossing() == to.crossing()) {
        if (arc.prev().crossing() == arc.crossing()) {
            // Case (vi) == (iv) && (v)
            // Nothing to do here - the entire component disappears.
            unknot1 = unknot2 = true;
        } else {
            // Case (iv) only
            // x -> arc -> to -> to2 -> arc2 -> y
            x = arc.prev();
            y = arc2.next();
            x.crossing()->next_[x.strand()] = y;
            y.crossing()->prev_[y.strand()] = x;
        }
    } else if (arc.prev().crossing() == arc.crossing()) {
        // Case (v) only
        // x -> to2 -> arc2 -> arc -> to -> y
        x = arc2.prev().prev();
        y = to.next();
        x.crossing()->next_[x.strand()] = y;
        y.crossing()->prev_[y.strand()] = x;
    } else {
        // We are not in any of cases (iv, v, vi).

        // In the code below, we don't handle case (iii) separately.
        // Instead we just process both cases (i) and (ii), since these
        // can be dealt with independently.

        // Strip the two crossings out of the first arc.
        // x -> arc -> to -> y
        x = arc.prev();
        if (x.crossing() == to.crossing()) {
            // Case (i)
            unknot1 = true;
        } else {
            y = to.next();
            x.crossing()->next_[x.strand()] = y;
            y.crossing()->prev_[y.strand()] = x;
        }

        // Now strip the two crossings out of the second arc.
        if (forward) {
            if (backward) {
                // Case (ii)
                unknot2 = true;
            } else {
                // x -> arc2 -> to2 -> y
                x = arc2.prev();
                y = arc2.next().next();
                x.crossing()->next_[x.strand()] = y;
                y.crossing()->prev_[y.strand()] = x;
            }
        } else {
            // x -> to2 -> arc2 -> y
            x = arc2.prev().prev();
            y = arc2.next();
            x.crossing()->next_[x.strand()] = y;
            y.crossing()->prev_[y.strand()] = x;
        }
    }

    // Now fix components_.
    // Here we ensure that any zero-crossing unknots are marked as such,
    // and that any other components that start at one of the to-be-deleted
    // crossings have their start points moved.
    int fixed = 0;
    for (auto it = components_.begin(); it != components_.end(); ++it) {
        if (it->crossing() == arc.crossing() ||
                it->crossing() == to.crossing()) {
            // In the test below, we use the fact that arc.strand() is
            // known to be the same as to.strand().
            if ((unknot1 && (it->strand() == arc.strand())) ||
                    (unknot2 && (it->strand() == arc2.strand()))) {
                // This component becomes a zero-crossing unknot.
                *it = StrandRef();
            } else {
                // This component still has crossings; we just need to
                // advance the start point out of the crossings that are
                // being removed.
                do
                    ++(*it);
                while (it->crossing() == arc.crossing() ||
                        it->crossing() == to.crossing());
            }

            // There are at most two components that need fixing.
            if (++fixed == 2)
                break;
        }
    }

    // Finally: destroy the two crossings entirely.
    crossings_.erase(crossings_.begin() + arc.crossing()->index());
    // Note that to.crossing() may have been reindexed.  This is okay,
    // since we still hold the pointer to the crossing.
    crossings_.erase(crossings_.begin() + to.crossing()->index());

    delete arc.crossing();
    delete to.crossing();

    return true;
}

bool Link::r2(StrandRef upperArc, int upperSide, StrandRef lowerArc,
        int lowerSide, bool check, bool perform) {
    // TODO: Implement r2
    return false;
}

bool Link::r3(StrandRef arc, int side, bool check, bool perform) {
    if (! arc) {
        // The move cannot be performed.
        // We should just return false, but only if check is true.
        return ! check;
    }

    // Find the three crossings at the vertices of the triangle, and
    // determine whether the three edges of the triangle leave them in the
    // forward or backward directions.
    StrandRef s[4];
    bool forward[4];

    s[0] = arc;
    forward[0] = true;

    int i;
    for (i = 1; i < 4; ++i) {
        if (forward[i-1]) {
            s[i] = s[i-1].next();
            s[i].jump();

            // forward[i] is true for (side, sign, strand):
            // 0, +, 0
            // 1, -, 0
            // 0, -, 1
            // 1, +, 1
            if (s[i].crossing()->sign() > 0)
                forward[i] = (side == s[i].strand());
            else
                forward[i] = (side != s[i].strand());
        } else {
            s[i] = s[i-1].prev();
            s[i].jump();

            // forward[i] is true for (side, sign, strand):
            // 1, +, 0
            // 0, -, 0
            // 1, -, 1
            // 0, +, 1
            if (s[i].crossing()->sign() > 0)
                forward[i] = (side != s[i].strand());
            else
                forward[i] = (side == s[i].strand());
        }
    }

    if (check) {
        if (s[3] != s[0] || forward[3] != forward[0])
            return false;
        if (s[0].crossing() == s[1].crossing() ||
                s[1].crossing() == s[2].crossing() ||
                s[1].crossing() == s[2].crossing())
            return false;
        if (s[0].strand() == s[1].strand() && s[1].strand() == s[2].strand())
            return false;
    }

    if (! perform)
        return true;

    // Reorder the two crossings on each of the three edges.
    StrandRef x, first, second, y;
    for (i = 0; i < 3; ++i) {
        if (forward[i]) {
            first = s[i];
            second = first.next();
        } else {
            second = s[i];
            first = second.prev();
        }

        x = first.prev();
        if (x.crossing() == second.crossing()) {
            // By planarity, this means that (first, second) is a
            // 2-crossing cycle.  Swapping the crossings will have no effect.
            continue;
        }
        y = second.next();

        // We have: x -> first -> second -> y
        // We want: x -> second -> first -> y

        x.crossing()->next_[x.strand()] = second;
        second.crossing()->next_[second.strand()] = first;
        first.crossing()->next_[first.strand()] = y;

        y.crossing()->prev_[y.strand()] = first;
        first.crossing()->prev_[first.strand()] = second;
        second.crossing()->prev_[second.strand()] = x;
    }

    return true;
}

} // namespace regina

