
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
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

#include <boost/python.hpp>
#include "manifold/nsimplesurfacebundle.h"
#include "../helpers.h"

using namespace boost::python;
using regina::NSimpleSurfaceBundle;

void addNSimpleSurfaceBundle() {
    scope s = class_<NSimpleSurfaceBundle, bases<regina::Manifold>,
            std::auto_ptr<NSimpleSurfaceBundle>, boost::noncopyable>
            ("NSimpleSurfaceBundle", init<int>())
        .def(init<const NSimpleSurfaceBundle&>())
        .def("type", &NSimpleSurfaceBundle::type)
        .def(regina::python::add_eq_operators())
    ;

    s.attr("S2xS1") = NSimpleSurfaceBundle::S2xS1;
    s.attr("S2xS1_TWISTED") = NSimpleSurfaceBundle::S2xS1_TWISTED;
    s.attr("RP2xS1") = NSimpleSurfaceBundle::RP2xS1;

    implicitly_convertible<std::auto_ptr<NSimpleSurfaceBundle>,
        std::auto_ptr<regina::Manifold> >();
}

