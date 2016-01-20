
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
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

#include <boost/python.hpp>
#include "dim4/dim4component.h"
#include "dim4/dim4edge.h"
#include "dim4/dim4pentachoron.h"
#include "dim4/dim4tetrahedron.h"
#include "dim4/dim4triangle.h"
#include "dim4/dim4triangulation.h"
#include "dim4/dim4vertex.h"
#include "../helpers.h"
#include "../generic/facehelper.h"

using namespace boost::python;
using regina::Dim4Pentachoron;

void addDim4Pentachoron() {
    class_<regina::Simplex<4>, std::auto_ptr<regina::Simplex<4>>,
            boost::noncopyable>("Simplex4", no_init)
        .def("description", &Dim4Pentachoron::description,
            return_value_policy<return_by_value>())
        .def("getDescription", &Dim4Pentachoron::getDescription,
            return_value_policy<return_by_value>())
        .def("setDescription", &Dim4Pentachoron::setDescription)
        .def("index", &Dim4Pentachoron::index)
        .def("adjacentPentachoron", &Dim4Pentachoron::adjacentPentachoron,
            return_value_policy<reference_existing_object>())
        .def("adjacentSimplex", &Dim4Pentachoron::adjacentSimplex,
            return_value_policy<reference_existing_object>())
        .def("adjacentGluing", &Dim4Pentachoron::adjacentGluing)
        .def("adjacentFacet", &Dim4Pentachoron::adjacentFacet)
        .def("hasBoundary", &Dim4Pentachoron::hasBoundary)
        .def("joinTo", &Dim4Pentachoron::joinTo)
        .def("join", &Dim4Pentachoron::join)
        .def("unjoin", &Dim4Pentachoron::unjoin,
            return_value_policy<reference_existing_object>())
        .def("isolate", &Dim4Pentachoron::isolate)
        .def("triangulation", &Dim4Pentachoron::triangulation,
            return_value_policy<reference_existing_object>())
        .def("getTriangulation", &Dim4Pentachoron::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("component", &Dim4Pentachoron::component,
            return_value_policy<reference_existing_object>())
        .def("getComponent", &Dim4Pentachoron::getComponent,
            return_value_policy<reference_existing_object>())
        .def("face", &regina::python::face<Dim4Pentachoron, 4, int>)
        .def("getFace", &regina::python::face<Dim4Pentachoron, 4, int>)
        .def("vertex", &Dim4Pentachoron::vertex,
            return_value_policy<reference_existing_object>())
        .def("getVertex", &Dim4Pentachoron::getVertex,
            return_value_policy<reference_existing_object>())
        .def("edge", &Dim4Pentachoron::edge,
            return_value_policy<reference_existing_object>())
        .def("getEdge", &Dim4Pentachoron::getEdge,
            return_value_policy<reference_existing_object>())
        .def("triangle", &Dim4Pentachoron::triangle,
            return_value_policy<reference_existing_object>())
        .def("getTriangle", &Dim4Pentachoron::getTriangle,
            return_value_policy<reference_existing_object>())
        .def("tetrahedron", &Dim4Pentachoron::tetrahedron,
            return_value_policy<reference_existing_object>())
        .def("getTetrahedron", &Dim4Pentachoron::getTetrahedron,
            return_value_policy<reference_existing_object>())
        .def("faceMapping", &regina::python::faceMapping<Dim4Pentachoron, 4>)
        .def("getFaceMapping", &regina::python::faceMapping<Dim4Pentachoron, 4>)
        .def("vertexMapping", &Dim4Pentachoron::vertexMapping)
        .def("getVertexMapping", &Dim4Pentachoron::getVertexMapping)
        .def("edgeMapping", &Dim4Pentachoron::edgeMapping)
        .def("getEdgeMapping", &Dim4Pentachoron::getEdgeMapping)
        .def("triangleMapping", &Dim4Pentachoron::triangleMapping)
        .def("getTriangleMapping", &Dim4Pentachoron::getTriangleMapping)
        .def("tetrahedronMapping", &Dim4Pentachoron::tetrahedronMapping)
        .def("getTetrahedronMapping", &Dim4Pentachoron::getTetrahedronMapping)
        .def("orientation", &Dim4Pentachoron::orientation)
        .def("str", &Dim4Pentachoron::str)
        .def("toString", &Dim4Pentachoron::toString)
        .def("detail", &Dim4Pentachoron::detail)
        .def("toStringLong", &Dim4Pentachoron::toStringLong)
        .def("__str__", &Dim4Pentachoron::str)
        .def(regina::python::add_eq_operators())
    ;

    scope().attr("Dim4Pentachoron") = scope().attr("Simplex4");
}
