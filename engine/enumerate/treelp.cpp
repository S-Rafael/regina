
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 2011-2021, Ben Burton                                   *
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

#include "angle/anglestructure.h"
#include "enumerate/treeconstraint.h"
#include "enumerate/treelp-impl.h"
#include "surfaces/normalsurface.h"

namespace regina {

// Instantiate templates for standard combinations of template arguments.
template class LPMatrix<Integer>;
template class LPMatrix<NativeLong>;

template class LPInitialTableaux<LPConstraintNone>;
template class LPInitialTableaux<LPConstraintEulerPositive>;
template class LPInitialTableaux<LPConstraintEulerZero>;
template class LPInitialTableaux<LPConstraintNonSpun>;

template class LPData<LPConstraintNone, Integer>;
template void LPData<LPConstraintNone, Integer>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintNone, Integer>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintNone, NativeLong>;
template void LPData<LPConstraintNone, NativeLong>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintNone, NativeLong>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintEulerPositive, Integer>;
template void LPData<LPConstraintEulerPositive, Integer>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintEulerPositive, Integer>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintEulerPositive, NativeLong>;
template void LPData<LPConstraintEulerPositive, NativeLong>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintEulerPositive, NativeLong>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintEulerZero, Integer>;
template void LPData<LPConstraintEulerZero, Integer>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintEulerZero, Integer>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintEulerZero, NativeLong>;
template void LPData<LPConstraintEulerZero, NativeLong>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintEulerZero, NativeLong>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintNonSpun, Integer>;
template void LPData<LPConstraintNonSpun, Integer>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintNonSpun, Integer>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintNonSpun, NativeLong>;
template void LPData<LPConstraintNonSpun, NativeLong>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintNonSpun, NativeLong>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

#ifdef INT128_AVAILABLE
template class LPData<LPConstraintNone, NativeInteger<16>>;
template void LPData<LPConstraintNone, NativeInteger<16>>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintNone, NativeInteger<16>>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintEulerPositive, NativeInteger<16>>;
template void LPData<LPConstraintEulerPositive, NativeInteger<16>>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintEulerPositive, NativeInteger<16>>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintEulerZero, NativeInteger<16>>;
template void LPData<LPConstraintEulerZero, NativeInteger<16>>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintEulerZero, NativeInteger<16>>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;

template class LPData<LPConstraintNonSpun, NativeInteger<16>>;
template void LPData<LPConstraintNonSpun, NativeInteger<16>>::
    extractSolution<VectorInt>(VectorInt&, const char*) const;
template void LPData<LPConstraintNonSpun, NativeInteger<16>>::
    extractSolution<Vector<LargeInteger>>(
    Vector<LargeInteger>&, const char*) const;
#endif

} // namespace regina
