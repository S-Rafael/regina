
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

#include <cassert>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>

#include "link/link.h"
#include "triangulation/dim3.h"
#include "utilities/stringutils.h"
#include "utilities/xmlutils.h"

namespace regina {

Triangulation<3>::Triangulation(const std::string& description) :
        strictAngleStructure_(false), generalAngleStructure_(false) {
    Triangulation<3>* attempt;

    if ((attempt = fromIsoSig(description))) {
        swap(*attempt);
        setLabel(description);
    } else if ((attempt = rehydrate(description))) {
        swap(*attempt);
        setLabel(description);
    } else if ((attempt = fromSnapPea(description))) {
        swap(*attempt);
        setLabel(attempt->label());
    }

    delete attempt;
}

Triangulation<3>::Triangulation(const Link& link) :
        strictAngleStructure_(false), generalAngleStructure_(false) {
    Triangulation<3>* ans = link.complement();
    swap(*ans);
    delete ans;
}

void Triangulation<3>::clearAllProperties() {
    clearBaseProperties();

    // Properties of the triangulation:
    zeroEfficient_.reset();
    splittingSurface_.reset();
    strictAngleStructure_ = false; // computation not attempted
    generalAngleStructure_ = false; // computation not attempted
    niceTreeDecomposition_.reset();

    // Properties of the manifold:
    if (! topologyLock_) {
        H1Rel_.reset();
        H1Bdry_.reset();
        H2_.reset();
        twoSphereBoundaryComponents_.reset();
        negativeIdealBoundaryComponents_.reset();
        threeSphere_.reset();
        threeBall_.reset();
        solidTorus_.reset();
        TxI_.reset();
        irreducible_.reset();
        compressingDisc_.reset();
        haken_.reset();
        turaevViroCache_.clear();
    }
}

void Triangulation<3>::swap(Triangulation<3>& other) {
    if (&other == this)
        return;

    ChangeEventSpan span1(*this);
    ChangeEventSpan span2(other);

    // Note: swapBaseData() calls Snapshottable::swap().
    swapBaseData(other);

    // Properties stored directly:
    std::swap(ideal_, other.ideal_);
    std::swap(standard_, other.standard_);

    // Properties stored using std::... helper classes:
    H1Rel_.swap(other.H1Rel_);
    H1Bdry_.swap(other.H1Bdry_);
    H2_.swap(other.H2_);

    twoSphereBoundaryComponents_.swap(other.twoSphereBoundaryComponents_);
    negativeIdealBoundaryComponents_.swap(
        other.negativeIdealBoundaryComponents_);

    zeroEfficient_.swap(other.zeroEfficient_);
    splittingSurface_.swap(other.splittingSurface_);

    threeSphere_.swap(other.threeSphere_);
    threeBall_.swap(other.threeBall_);
    solidTorus_.swap(other.solidTorus_);
    TxI_.swap(other.TxI_);
    irreducible_.swap(other.irreducible_);
    compressingDisc_.swap(other.compressingDisc_);
    haken_.swap(other.haken_);

    strictAngleStructure_.swap(other.strictAngleStructure_);
    generalAngleStructure_.swap(other.generalAngleStructure_);
    niceTreeDecomposition_.swap(other.niceTreeDecomposition_);

    // Properties stored using std::... containers:
    turaevViroCache_.swap(other.turaevViroCache_);
}

void Triangulation<3>::writeTextLong(std::ostream& out) const {
    ensureSkeleton();

    out << "Size of the skeleton:\n";
    out << "  Tetrahedra: " << simplices_.size() << '\n';
    out << "  Triangles: " << countTriangles() << '\n';
    out << "  Edges: " << countEdges() << '\n';
    out << "  Vertices: " << countVertices() << '\n';
    out << '\n';

    Tetrahedron<3>* tet;
    Tetrahedron<3>* adjTet;
    unsigned tetPos;
    int face, vertex, start, end;
    Perm<4> adjPerm;

    out << "Tetrahedron gluing:\n";
    out << "  Tet  |  glued to:      (012)      (013)      (023)      (123)\n";
    out << "  -----+-------------------------------------------------------\n";
    for (tetPos=0; tetPos<simplices_.size(); tetPos++) {
        tet = simplices_[tetPos];
        out << "  " << std::setw(3) << tetPos << "  |           ";
        for (face=3; face>=0; face--) {
            out << "  ";
            adjTet = tet->adjacentTetrahedron(face);
            if (! adjTet)
                out << " boundary";
            else {
                adjPerm = tet->adjacentGluing(face);
                out << std::setw(3) << adjTet->index() << " (";
                for (vertex=0; vertex<4; vertex++) {
                    if (vertex == face) continue;
                    out << adjPerm[vertex];
                }
                out << ")";
            }
        }
        out << '\n';
    }
    out << '\n';

    out << "Vertices:\n";
    out << "  Tet  |  vertex:    0   1   2   3\n";
    out << "  -----+--------------------------\n";
    for (tetPos=0; tetPos<simplices_.size(); tetPos++) {
        tet = simplices_[tetPos];
        out << "  " << std::setw(3) << tetPos << "  |          ";
        for (vertex=0; vertex<4; vertex++)
            out << ' ' << std::setw(3) << tet->vertex(vertex)->index();
        out << '\n';
    }
    out << '\n';

    out << "Edges:\n";
    out << "  Tet  |  edge:   01  02  03  12  13  23\n";
    out << "  -----+--------------------------------\n";
    for (tetPos=0; tetPos<simplices_.size(); tetPos++) {
        tet = simplices_[tetPos];
        out << "  " << std::setw(3) << tetPos << "  |        ";
        for (start=0; start<4; start++)
            for (end=start+1; end<4; end++)
                out << ' ' << std::setw(3)
                    << tet->edge(Edge<3>::edgeNumber[start][end])->index();
        out << '\n';
    }
    out << '\n';

    out << "Triangles:\n";
    out << "  Tet  |  face:  012 013 023 123\n";
    out << "  -----+------------------------\n";
    for (tetPos=0; tetPos<simplices_.size(); tetPos++) {
        tet = simplices_[tetPos];
        out << "  " << std::setw(3) << tetPos << "  |        ";
        for (face=3; face>=0; face--)
            out << ' ' << std::setw(3) << tet->triangle(face)->index();
        out << '\n';
    }
    out << '\n';
}

void Triangulation<3>::writeXMLPacketData(std::ostream& out,
        FileFormat format, bool anon, PacketRefs& refs) const {
    using regina::xml::xmlEncodeSpecialChars;
    using regina::xml::xmlValueTag;

    writeXMLHeader(out, "tri", format, anon, refs, true,
        std::pair("dim", 3), std::pair("size", simplices_.size()),
        std::pair("perm", "index"));

    // Write the tetrahedron gluings.
    if (format == REGINA_XML_GEN_2) {
        out << "  <tetrahedra ntet=\"" << simplices_.size() << "\">\n";
        for (Tetrahedron<3>* t : simplices_) {
            out << "    <tet desc=\"" <<
                xmlEncodeSpecialChars(t->description()) << "\"> ";
            for (int face = 0; face < 4; face++) {
                Tetrahedron<3>* adjTet = t->adjacentTetrahedron(face);
                if (adjTet) {
                    out << adjTet->index() << ' '
                        << static_cast<int>(t->adjacentGluing(face).imagePack())
                        << ' ';
                } else
                    out << "-1 -1 ";
            }
            out << "</tet>\n";
        }
        out << "  </tetrahedra>\n";
    } else {
        for (Tetrahedron<3>* t : simplices_) {
            if (t->description().empty())
                out << "  <simplex> ";
            else
                out << "  <simplex desc=\"" <<
                    xmlEncodeSpecialChars(t->description()) << "\"> ";
            for (int face = 0; face < 4; face++) {
                Tetrahedron<3>* adjTet = t->adjacentTetrahedron(face);
                if (adjTet) {
                    out << adjTet->index() << ' '
                        << static_cast<int>(t->adjacentGluing(face).SnIndex())
                        << ' ';
                } else
                    out << "-1 -1 ";
            }
            out << "</simplex>\n";
        }
    }

    writeXMLBaseProperties(out);

    if (H1Rel_.has_value()) {
        out << "  <H1Rel>";
        H1Rel_->writeXMLData(out);
        out << "</H1Rel>\n";
    }
    if (H1Bdry_.has_value()) {
        out << "  <H1Bdry>";
        H1Bdry_->writeXMLData(out);
        out << "</H1Bdry>\n";
    }
    if (H2_.has_value()) {
        out << "  <H2>";
        H2_->writeXMLData(out);
        out << "</H2>\n";
    }
    if (twoSphereBoundaryComponents_.has_value())
        out << "  " << xmlValueTag("twosphereboundarycomponents",
            *twoSphereBoundaryComponents_) << '\n';
    if (negativeIdealBoundaryComponents_.has_value())
        out << "  " << xmlValueTag("negativeidealboundarycomponents",
            *negativeIdealBoundaryComponents_) << '\n';
    if (zeroEfficient_.has_value())
        out << "  " << xmlValueTag("zeroeff", *zeroEfficient_) << '\n';
    if (splittingSurface_.has_value())
        out << "  " << xmlValueTag("splitsfce", *splittingSurface_) << '\n';
    if (threeSphere_.has_value())
        out << "  " << xmlValueTag("threesphere", *threeSphere_) << '\n';
    if (threeBall_.has_value())
        out << "  " << xmlValueTag("threeball", *threeBall_) << '\n';
    if (solidTorus_.has_value())
        out << "  " << xmlValueTag("solidtorus", *solidTorus_) << '\n';
    if (TxI_.has_value())
        out << "  " << xmlValueTag("txi", *TxI_) << '\n';
    if (irreducible_.has_value())
        out << "  " << xmlValueTag("irreducible", *irreducible_) << '\n';
    if (compressingDisc_.has_value())
        out << "  " << xmlValueTag("compressingdisc", *compressingDisc_) << '\n';
    if (haken_.has_value())
        out << "  " << xmlValueTag("haken", *haken_) << '\n';

    writeXMLFooter(out, "tri", format, anon, refs);
}

Triangulation<3>* Triangulation<3>::enterTextTriangulation(std::istream& in,
        std::ostream& out) {
    Triangulation<3>* triang = new Triangulation<3>();
    Tetrahedron<3>* tet;
    long nTet;

    // Create new tetrahedra.
    out << "Number of tetrahedra: ";
    in >> nTet;
    while (nTet < 0) {
        out << "The number of tetrahedra must be non-negative.\n";
        out << "Number of tetrahedra: ";
        in >> nTet;
    }
    out << '\n';

    for (long i=0; i<nTet; i++)
        triang->newTetrahedron();

    // Read in the joins.
    long tetPos, altPos;
    int face, altFace;
    Tetrahedron<3>* altTet;
    int vertices[6];

    out << "Tetrahedra are numbered from 0 to " << nTet-1 << ".\n";
    out << "Vertices are numbered from 0 to 3.\n";
    out << "Enter in the face gluings one at a time.\n";
    out << '\n';
    while(1) {
        out << "Enter two tetrahedra to glue, separated by a space, or ";
        out << "-1 if finished: ";
        in >> tetPos;
        if (tetPos < 0) break;
        in >> altPos;
        if (altPos < 0) break;
        if (tetPos >= nTet || altPos >= nTet) {
            out << "Tetrahedron identifiers must be between 0 and "
                << nTet-1 << " inclusive.\n";
            continue;
        }
        tet = triang->simplices_[tetPos];
        altTet = triang->simplices_[altPos];
        out << "Enter the three vertices of the first tetrahedron ("
            << tetPos << "), separated by spaces,\n";
        out << "    that will form one face of the gluing: ";
        in >> vertices[0] >> vertices[1] >> vertices[2];
        out << "Enter the corresponding three vertices of the second tetrahedron ("
            << altPos << "): ";
        in >> vertices[3] >> vertices[4] >> vertices[5];

        if (vertices[3] < 0 || vertices[3] > 3 || vertices[4] < 0
                || vertices[4] > 3 || vertices[5] < 0 || vertices[5] > 3
                || vertices[0] < 0 || vertices[0] > 3 || vertices[1] < 0
                || vertices[1] > 3 || vertices[2] < 0 || vertices[2] > 3) {
            out << "Vertices must be between 0 and 3 inclusive.\n";
            continue;
        }
        if (vertices[0] == vertices[1] || vertices[1] == vertices[2]
                || vertices[2] == vertices[0]) {
            out << "The three vertices for tetrahedron " << tetPos
                << " must be different.\n";
            continue;
        }
        if (vertices[3] == vertices[4] || vertices[4] == vertices[5]
                || vertices[5] == vertices[3]) {
            out << "The three vertices for tetrahedron " << altPos
                << " must be different.\n";
            continue;
        }

        face = 6 - vertices[0] - vertices[1] - vertices[2];
        altFace = 6 - vertices[3] - vertices[4] - vertices[5];

        if (face == altFace && tetPos == altPos) {
            out << "You cannot glue a face to itself.\n";
            continue;
        }
        if (tet->adjacentTetrahedron(face) ||
                altTet->adjacentTetrahedron(altFace)) {
            out << "One of these faces is already glued to something else.\n";
            continue;
        }

        tet->join(face, altTet,
            Perm<4>(vertices[0], vertices[3], vertices[1], vertices[4],
                vertices[2], vertices[5], face, altFace));
        out << '\n';
    }

    out << "Finished reading gluings.\n";
    out << "The triangulation has been successfully created.\n";
    out << '\n';

    // Return the completed triangulation.
    return triang;
}

long Triangulation<3>::eulerCharManifold() const {
    // Begin with V - E + F - T.
    // This call to eulerCharTri() also ensures that the skeleton has
    // been calculated.
    long ans = eulerCharTri();

    // Truncate any ideal vertices.
    for (auto bc : boundaryComponents())
        if (bc->isIdeal())
            ans += bc->eulerChar() - 1;

    // If we have an invalid triangulation, we need to locate invalid
    // vertices (i.e., non-standard boundary vertices) and also invalid edges,
    // and truncate those unwanted bits also.
    if (! valid_) {
        for (Vertex<3>* v : vertices())
            if (v->linkType() == Vertex<3>::INVALID)
                ans += v->linkEulerChar() - 1;
        for (Edge<3>* e : edges())
            if (! e->isValid())
                ++ans;
    }

    return ans;
}

Triangulation<3>::Triangulation(const Triangulation<3>& X, bool cloneProps) :
        TriangulationBase<3>(X, cloneProps),
        strictAngleStructure_(false), generalAngleStructure_(false) {
    if (! cloneProps)
        return;

    // Clone properties:
    H1Rel_ = X.H1Rel_;
    H1Bdry_ = X.H1Bdry_;
    H2_ = X.H2_;

    twoSphereBoundaryComponents_ = X.twoSphereBoundaryComponents_;
    negativeIdealBoundaryComponents_ = X.negativeIdealBoundaryComponents_;
    zeroEfficient_ = X.zeroEfficient_;
    splittingSurface_ = X.splittingSurface_;
    threeSphere_ = X.threeSphere_;
    threeBall_ = X.threeBall_;
    solidTorus_ = X.solidTorus_;
    TxI_ = X.TxI_;
    irreducible_ = X.irreducible_;
    compressingDisc_ = X.compressingDisc_;
    haken_ = X.haken_;

    // Any cached angle structures must be remade to live in this triangulation.
    if (std::holds_alternative<AngleStructure>(X.strictAngleStructure_))
        strictAngleStructure_ = AngleStructure(
            std::get<AngleStructure>(X.strictAngleStructure_), *this);
    else
        strictAngleStructure_ = std::get<bool>(X.strictAngleStructure_);
    if (std::holds_alternative<AngleStructure>(X.generalAngleStructure_))
        generalAngleStructure_ = AngleStructure(
            std::get<AngleStructure>(X.generalAngleStructure_), *this);
    else
        generalAngleStructure_ = std::get<bool>(X.generalAngleStructure_);

    turaevViroCache_ = X.turaevViroCache_;
}

std::string Triangulation<3>::snapPea() const {
    std::ostringstream out;
    snapPea(out);
    return out.str();
}

void Triangulation<3>::snapPea(std::ostream& out) const {
    // Sanity checks.
    if ((! isValid()) || hasBoundaryTriangles() || simplices_.empty())
        return;

    // Write header information.
    out << "% Triangulation\n";
    if (label().empty())
        out << "Regina_Triangulation\n";
    else
        out << stringToToken(label()) << '\n';

    // Write general details.
    out << "not_attempted 0.0\n";
    out << "unknown_orientability\n";
    out << "CS_unknown\n";

    // Write cusps.
    out << "0 0\n";

    // Write tetrahedra.
    out << size() << '\n';

    int i, j;
    for (Tetrahedron<3>* tet : tetrahedra()) {
        // Although our precondition states that there are no boundary
        // triangles, we test for this anyway.  If somebody makes a mistake and
        // calls this routine with a bounded triangulation, we don't want
        // to wind up calling nullptr->index() and crashing.
        for (i = 0; i < 4; i++)
            if (tet->adjacentTetrahedron(i))
                out << "   " << tet->adjacentTetrahedron(i)->index() << ' ';
            else
                out << "   -1 ";
        out << '\n';
        for (i = 0; i < 4; i++)
            out << ' ' << tet->adjacentGluing(i).str();
        out << '\n';

        // Incident cusps.
        for (i = 0; i < 4; i++)
            out << "  -1 ";
        out << '\n';

        // Meridians and longitudes.
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 16; j++)
                out << "  0";
            out << '\n';
        }

        // Tetrahedron shape.
        out << "0.0 0.0\n";
    }
}

bool Triangulation<3>::saveSnapPea(const char* filename) const {
    // Sanity checks.
    if ((! isValid()) || hasBoundaryTriangles() || simplices_.empty())
        return false;

    std::ofstream out(filename);
    if (!out)
        return false;
    snapPea(out);
    return true;
}

std::string Triangulation<3>::recogniser() const {
    std::ostringstream out;
    recogniser(out);
    return out.str();
}

std::string Triangulation<3>::recognizer() const {
    std::ostringstream out;
    recogniser(out);
    return out.str();
}

void Triangulation<3>::recogniser(std::ostream& out) const {
    // Sanity checks.
    if ((! isValid()) || hasBoundaryTriangles())
        return;

    // Write the header.
    out << "triangulation" << std::endl;

    // Write face gluings.
    Triangle<3>* f;
    Tetrahedron<3>* tet;
    Perm<4> vert;
    for (unsigned i = 0; i < countTriangles(); ++i) {
        f = triangle(i);

        tet = f->embedding(0).tetrahedron();
        vert = f->embedding(0).vertices();
        out << 't' << (tet->index() + 1)
            << '(' << (vert[0] + 1)
            << ',' << (vert[1] + 1)
            << ',' << (vert[2] + 1) << ") - ";

        tet = f->embedding(1).tetrahedron();
        vert = f->embedding(1).vertices();
        out << 't' << (tet->index() + 1)
            << '(' << (vert[0] + 1)
            << ',' << (vert[1] + 1)
            << ',' << (vert[2] + 1) << ')';

        if (i != countTriangles() - 1)
            out << ',';
        out << std::endl;
    }

    // Write the footer.
    out << "end" << std::endl;
}

bool Triangulation<3>::saveRecogniser(const char* filename) const {
    // Sanity checks.
    if ((! isValid()) || hasBoundaryTriangles())
        return false;

    // Write to file or stdout as appropriate.
    std::ofstream out(filename);
    if (! out)
        return false;
    recogniser(out);
    return true;
}

} // namespace regina

