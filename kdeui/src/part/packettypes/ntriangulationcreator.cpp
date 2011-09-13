
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
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

// Regina core includes:
#include "manifold/nsfs.h"
#include "maths/numbertheory.h"
#include "split/nsignature.h"
#include "triangulation/nexampletriangulation.h"
#include "triangulation/ntriangulation.h"

// UI includes:
#include "ntriangulationcreator.h"

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qcheckbox.h>
#include <QHBoxLayout>
#include <qlabel.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qvalidator.h>
#include <QStackedWidget>

using regina::NExampleTriangulation;
using regina::NTriangulation;

namespace {
    /**
     * Triangulation type IDs that correspond to indices in the
     * triangulation type combo box.
     */
    enum {
        TRI_EMPTY,
        TRI_LAYERED_LENS_SPACE,
        TRI_SFS_SPHERE,
        TRI_LAYERED_SOLID_TORUS,
        TRI_LAYERED_LOOP,
        TRI_AUG_TRI_SOLID_TORUS,
        TRI_ISOSIG,
        TRI_DEHYDRATION,
        TRI_SPLITTING_SURFACE,
        TRI_EXAMPLE
    };

    /**
     * Example IDs that correspond to indices in the example
     * triangulation combo box.
     */
    enum {
        EXAMPLE_S3_ONETET,
        EXAMPLE_S3_BING,
        EXAMPLE_RP3RP3,
        EXAMPLE_FIGURE8,
        EXAMPLE_GIESEKING,
        EXAMPLE_LENS8_3,
        EXAMPLE_POINCARE,
        EXAMPLE_RP2xS1,
        EXAMPLE_S2xS1,
        EXAMPLE_SOLIDKLEIN,
        EXAMPLE_WEBERSEIFERT,
        EXAMPLE_WHITEHEAD
    };

    /**
     * Regular expressions describing different sets of parameters.
     */
    QRegExp reLensParams("^[^0-9\\-]*(\\d+)[^0-9\\-]+(\\d+)[^0-9\\-]*$");
    QRegExp reLSTParams(
        "^[^0-9\\-]*(\\d+)[^0-9\\-]+(\\d+)[^0-9\\-]+(\\d+)[^0-9\\-]*$");
    QRegExp reSFS3Params(
        "^[^0-9\\-]*(-?\\d+)[^0-9\\-]+(-?\\d+)"
        "[^0-9\\-]+(-?\\d+)[^0-9\\-]+(-?\\d+)"
        "[^0-9\\-]+(-?\\d+)[^0-9\\-]+(-?\\d+)[^0-9\\-]*$");
    QRegExp reSFSAllParams(
        "^[^0-9\\-]*(-?\\d+)[^0-9\\-]+(-?\\d+)"
        "(?:[^0-9\\-]+(-?\\d+)[^0-9\\-]+(-?\\d+))*"
        "[^0-9\\-]*$");
    QRegExp reSFSParamPair("(-?\\d+)[^0-9\\-]+(-?\\d+)");
    QRegExp reIsoSig("^([A-Za-z0-9+-]+)$");
    QRegExp reDehydration("^([A-Za-z]+)$");
    QRegExp reSignature("^([\\(\\)\\.,;:\\|\\-A-Za-z]+)$");
}

NTriangulationCreator::NTriangulationCreator() {
    // Set up the basic layout.
    ui = new QWidget();
    QBoxLayout* layout = new QVBoxLayout(ui);

    QBoxLayout* typeArea = new QHBoxLayout();//layout, 5);
    layout->addLayout(typeArea);
    QString expln = i18n("Specifies what type of triangulation to create.");
    QLabel* label = new QLabel(i18n("Type of triangulation:"), ui);
    label->setWhatsThis(expln);
    typeArea->addWidget(label);
    type = new QComboBox(ui);
    type->setWhatsThis(expln);
    typeArea->addWidget(type, 1);

    layout->addSpacing(5);

    details = new QStackedWidget(ui);
    layout->addWidget(details, 1);

    // Set up the individual types of triangulation.
    // Note that the order in which these options are added to the combo
    // box must correspond precisely to the type IDs defined at the head
    // of this file.
    QWidget* hArea;

    type->insertItem(TRI_EMPTY,i18n("Empty"));
    details->addWidget(new QWidget());

    type->insertItem(TRI_LAYERED_LENS_SPACE,i18n("Layered lens space"));
    hArea = new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>The (p,q) parameters of the new "
        "lens space.  These integers must be relatively prime.  Example "
        "parameters are <i>8,3</i>.</qt>");
    label = new QLabel(i18n("<qt>Parameters (<i>p</i>,<i>q</i>):</qt>"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    lensParams = new KLineEdit();
    lensParams->setValidator(new QRegExpValidator(reLensParams, hArea));
    lensParams->setWhatsThis(expln);
    hLayout->addWidget(lensParams, 1);
    details->addWidget(hArea);//, TRI_LAYERED_LENS_SPACE);

    type->insertItem(TRI_SFS_SPHERE,i18n("Seifert fibred space over 2-sphere"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>The parameters "
        "(<i>a<sub>1</sub></i>,<i>b<sub>1</sub></i>) "
        "(<i>a<sub>2</sub></i>,<i>b<sub>2</sub></i>) ... "
        "(<i>a<sub>n</sub></i>,<i>b<sub>n</sub></i>) "
        "describe the exceptional fibres of the new Seifert fibred space.  "
        "The two integers in each pair must be relatively prime, and none of "
        "<i>a<sub>1</sub></i>, <i>a<sub>2</sub></i>, ..., "
        "<i>a<sub>n</sub></i> may be zero.<p>"
        "Each pair of parameters (<i>a</i>,<i>b</i>) does not need to be "
        "normalised, i.e., the parameters may be positive or negative and "
        "<i>b</i> may lie outside the range [0,<i>a</i>).  There is no "
        "separate twisting parameter; each additional twist can be "
        "incorporated into the existing parameters by replacing some pair "
        "(<i>a</i>,<i>b</i>) with the pair (<i>a</i>,<i>a</i>+<i>b</i>).  "
        "Including pairs of the form (1,<i>k</i>) and even (1,0) is "
        "acceptable.<p>"
        "An example set of parameters is <i>(2,-1) (3,4) (5,-4)</i>, "
        "representing the Poincar&eacute; homology sphere.</qt>");
    label = new QLabel(i18n("<qt>Parameters "
        "(<i>a</i><sub>1</sub>,<i>b</i><sub>1</sub>) "
        "... (<i>a<sub>n</sub></i>,<i>b<sub>n</sub></i>):</qt>"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    sfsParams = new KLineEdit();
    sfsParams->setValidator(new QRegExpValidator(reSFSAllParams, hArea));
    sfsParams->setWhatsThis(expln);
    hLayout->addWidget(sfsParams, 1);
    details->addWidget(hArea);//, TRI_SFS_SPHERE);

    type->insertItem(TRI_LAYERED_SOLID_TORUS,i18n("Layered solid torus"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>The three parameters of the new "
        "layered solid torus.  These must be relatively prime non-negative "
        "integers, and two of them must add to give the third.  Example "
        "parameters are <i>3,4,7</i>.</qt>");
    label = new QLabel(i18n("<qt>Parameters "
        "(<i>a</i>,<i>b</i>,<i>c</i>):</qt>"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    lstParams = new KLineEdit();
    lstParams->setValidator(new QRegExpValidator(reLSTParams, hArea));
    lstParams->setWhatsThis(expln);
    hLayout->addWidget(lstParams, 1);
    details->addWidget(hArea);//, TRI_LAYERED_SOLID_TORUS);

    type->insertItem(TRI_LAYERED_LOOP,i18n("Layered loop"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("The number of tetrahedra in the new layered loop.");
    label = new QLabel(i18n("Length:"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    loopLen = new KLineEdit();
    QIntValidator* val = new QIntValidator(hArea);
    val->setBottom(1);
    loopLen->setValidator(val);
    loopLen->setWhatsThis(expln);
    hLayout->addWidget(loopLen, 1);
    loopTwisted = new QCheckBox(i18n("Twisted"));
    loopTwisted->setChecked(true);
    loopTwisted->setWhatsThis(i18n("Specifies whether or not the "
        "new layered loop is twisted."));
    hLayout->addWidget(loopTwisted);
    details->addWidget(hArea);//, TRI_LAYERED_LOOP);

    type->insertItem(TRI_AUG_TRI_SOLID_TORUS,
        i18n("Augmented triangular solid torus"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>The six parameters "
        "(<i>a<sub>1</sub></i>,<i>b<sub>1</sub></i>) "
        "(<i>a<sub>2</sub></i>,<i>b<sub>2</sub></i>) "
        "(<i>a<sub>3</sub></i>,<i>b<sub>3</sub></i>) "
        "of the new augmented triangular solid torus.  The two integers "
        "in each pair must be relatively prime, and both "
        "positive and negative integers are allowed.<p>"
        "Example parameters are <i>(2,1) (3,-2) (5,-4)</i>.</qt>");
    label = new QLabel(i18n("<qt>Parameters "
        "(<i>a</i><sub>1</sub>,<i>b</i><sub>1</sub>) "
        "(<i>a</i><sub>2</sub>,<i>b</i><sub>2</sub>) "
        "(<i>a</i><sub>3</sub>,<i>b</i><sub>3</sub>):</qt>"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    augParams = new KLineEdit();
    augParams->setValidator(new QRegExpValidator(reSFS3Params, hArea));
    augParams->setWhatsThis(expln);
    hLayout->addWidget(augParams, 1);
    details->addWidget(hArea);//, TRI_AUG_TRI_SOLID_TORUS);

    type->insertItem(TRI_ISOSIG,i18n("From isomorphism signature"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>The isomorphism signature "
        "from which the new triangulation will be created.  An example "
        "isomorphism signature is <i>bkaagj</i>.<p>"
        "Isomorphism signatures identify triangulations uniquely "
        "up to combinatorial isomorphism.  For more information, "
        "see <i>The Pachner graph and the simplification of 3-sphere "
        "triangulations</i>, B.&nbsp;Burton, published in "
        "<i>SCG '11: Proceedings of the Twenty-Seventh Annual Symposium "
        "on Computational Geometry</i>, ACM, 2011, "
        "pp.&nbsp;153&ndash;162.</qt>");
    label = new QLabel(i18n("Isomorphism signature:"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    isoSig = new KLineEdit();
    isoSig->setValidator(new QRegExpValidator(reIsoSig, hArea));
    isoSig->setWhatsThis(expln);
    hLayout->addWidget(isoSig, 1);
    details->addWidget(hArea);//, TRI_ISOSIG);

    type->insertItem(TRI_DEHYDRATION,i18n("From dehydration"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>The dehydration string "
        "from which the new triangulation will be created.  An example "
        "dehydration string is <i>baaaade</i>.<p>"
        "Dehydration strings are described in detail in "
        "<i>A census of cusped hyperbolic 3-manifolds</i>, "
        "Callahan, Hildebrand and Weeks, published in "
        "<i>Mathematics of Computation</i> <b>68</b>, 1999.</qt>");
    label = new QLabel(i18n("Dehydration string:"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    dehydrationString = new KLineEdit(hArea);
    dehydrationString->setValidator(new QRegExpValidator(reDehydration, hArea));
    dehydrationString->setWhatsThis(expln);
    hLayout->addWidget(dehydrationString, 1);
    details->addWidget(hArea);//, TRI_DEHYDRATION);

    type->insertItem(TRI_SPLITTING_SURFACE,i18n("From splitting surface"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>The signature of the "
        "splitting surface from which the new triangulation will be "
        "created.  An example signature is <i>(abb)(ac)(c)</i>.<p>"
        "Splitting surface signatures are described in detail in "
        "<i>Minimal triangulations and normal surfaces</i>, "
        "Burton, PhD thesis, available from the Regina website.</qt>");
    label = new QLabel(i18n("Signature:"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    splittingSignature = new KLineEdit(hArea);
    splittingSignature->setValidator(new QRegExpValidator(reSignature, hArea));
    splittingSignature->setWhatsThis(expln);
    hLayout->addWidget(splittingSignature, 1);
    details->addWidget(hArea);//, TRI_SPLITTING_SURFACE);

    type->insertItem(type->count(),i18n("Example triangulation"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = i18n("<qt>Specifies which particular example triangulation to "
        "create.<p>"
        "A selection of ready-made 3-manifold triangulations is offered "
        "here to help you experiment and see how Regina works.</qt>");
    label = new QLabel(i18n("Example:"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    exampleWhich = new QComboBox(hArea);
    exampleWhich->insertItem(0, i18n("3-sphere (1 tetrahedron)"));
    exampleWhich->insertItem(1, i18n("3-sphere (dual to Bing's house)"));
    exampleWhich->insertItem(2, i18n("Connected sum RP3 # RP3"));
    exampleWhich->insertItem(3, i18n("Figure eight knot complement"));
    exampleWhich->insertItem(4, i18n("Gieseking manifold"));
    exampleWhich->insertItem(5, i18n("Lens space L(8,3)"));
    exampleWhich->insertItem(6, i18n("Poincaré homology sphere"));
    exampleWhich->insertItem(7, i18n("Product RP2 x S1"));
    exampleWhich->insertItem(8, i18n("Product S2 x S1"));
    exampleWhich->insertItem(9, i18n("Solid Klein bottle"));
    exampleWhich->insertItem(10, i18n("Weber-Seifert dodecahedral space"));
    exampleWhich->insertItem(11, i18n("Whitehead link complement"));
    exampleWhich->setCurrentIndex(0);
    exampleWhich->setWhatsThis(expln);
    hLayout->addWidget(exampleWhich, 1);
    details->addWidget(hArea);//, TRI_EXAMPLE);

    // Tidy up.
    type->setCurrentIndex(0);
    details->setCurrentIndex((int)0);

    QObject::connect(type, SIGNAL(activated(int)), details,
        SLOT(setCurrentIndex(int)));
}

QWidget* NTriangulationCreator::getInterface() {
    return ui;
}

regina::NPacket* NTriangulationCreator::createPacket(regina::NPacket*,
        QWidget* parentWidget) {
    int typeId = type->currentIndex();
    if (typeId == TRI_EMPTY)
        return new NTriangulation();
    else if (typeId == TRI_LAYERED_LENS_SPACE) {
        if (! reLensParams.exactMatch(lensParams->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The lens space "
                "parameters (p,q) must be two non-negative integers.  "
                "Example parameters are <i>8,3</i>.</qt>"));
            return 0;
        }

        unsigned long p = reLensParams.cap(1).toULong();
        unsigned long q = reLensParams.cap(2).toULong();

        if (p == 0 && q == 0) {
            KMessageBox::error(parentWidget, i18n("At least one of the "
                "two lens space parameters must be strictly positive."));
            return 0;
        }
        if (p < q && ! (p == 0 && q == 1)) {
            KMessageBox::error(parentWidget, i18n("<qt>The second lens space "
                "parameter must be smaller than the first.  For instance, "
                "the parameters <i>8,3</i> are valid whereas <i>3,8</i> "
                "are not."));
            return 0;
        }
        if (regina::gcd(p, q) != 1) {
            KMessageBox::error(parentWidget, i18n("The two lens space "
                "parameters must be relatively prime."));
            return 0;
        }

        NTriangulation* ans = new NTriangulation();
        ans->insertLayeredLensSpace(p, q);
        return ans;
    } else if (typeId == TRI_LAYERED_LOOP) {
        unsigned long len = loopLen->text().toULong();
        if (len == 0) {
            KMessageBox::error(parentWidget, i18n("The layered loop length "
                "must be a strictly positive integer."));
            return 0;
        }

        NTriangulation* ans = new NTriangulation();
        ans->insertLayeredLoop(len, loopTwisted->isChecked());
        return ans;
    } else if (typeId == TRI_LAYERED_SOLID_TORUS) {
        if (! reLSTParams.exactMatch(lstParams->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The layered solid "
                "torus parameters (a,b,c) must be three non-negative "
                "integers.  Example parameters are <i>3,4,7</i>.</qt>"));
            return 0;
        }

        unsigned long a = reLSTParams.cap(1).toULong();
        unsigned long b = reLSTParams.cap(2).toULong();
        unsigned long c = reLSTParams.cap(3).toULong();

        if (a == 0 && b == 0 && c == 0) {
            KMessageBox::error(parentWidget, i18n("At least one of the "
                "three layered solid torus parameters must be strictly "
                "positive."));
            return 0;
        }
        if (regina::gcd(a, b) != 1) {
            KMessageBox::error(parentWidget, i18n("The three layered "
                "solid torus parameters must be relatively prime."));
            return 0;
        }

        if (a + b == c) {
            NTriangulation* ans = new NTriangulation();
            if (a <= b)
                ans->insertLayeredSolidTorus(a, b);
            else
                ans->insertLayeredSolidTorus(b, a);
            return ans;
        } else if (a + c == b) {
            NTriangulation* ans = new NTriangulation();
            if (a <= c)
                ans->insertLayeredSolidTorus(a, c);
            else
                ans->insertLayeredSolidTorus(c, a);
            return ans;
        } else if (b + c == a) {
            NTriangulation* ans = new NTriangulation();
            if (b <= c)
                ans->insertLayeredSolidTorus(b, c);
            else
                ans->insertLayeredSolidTorus(c, b);
            return ans;
        } else {
            KMessageBox::error(parentWidget, i18n("<qt>Two of the layered "
                "solid torus parameters must add to give the third.  "
                "For instance, the parameters <i>3,4,7</i> are valid "
                "whereas the parameters <i>3,4,5</i> are not.</qt>"));
            return 0;
        }
    } else if (typeId == TRI_SFS_SPHERE) {
        if (! reSFSAllParams.exactMatch(sfsParams->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>All 2<i>n</i> Seifert "
                "fibred space parameters "
                "(<i>a<sub>1</sub></i>,<i>b<sub>1</sub></i>) "
                "(<i>a<sub>2</sub></i>,<i>b<sub>2</sub></i>) ... "
                "(<i>a<sub>n</sub></i>,<i>b<sub>n</sub></i>) "
                "must be supplied.<p>"
                "These <i>n</i> pairs of integers describe the <i>n</i> "
                "exceptional fibres of the new Seifert fibred space.  "
                "The two integers in each pair must be relatively prime, and "
                "none of <i>a<sub>1</sub></i>, <i>a<sub>2</sub></i>, ..., "
                "<i>a<sub>n</sub></i> may be zero.<p>"
                "Each pair of parameters (<i>a</i>,<i>b</i>) does not need "
                "to be normalised, i.e., the parameters may be positive or "
                "negative and <i>b</i> may lie outside the range "
                "[0,<i>a</i>).  There is no "
                "separate twisting parameter; each additional twist can be "
                "incorporated into the existing parameters by replacing some "
                "pair (<i>a</i>,<i>b</i>) with the pair "
                "(<i>a</i>,<i>a</i>+<i>b</i>).  "
                "Including pairs of the form (1,<i>k</i>) and even (1,0) is "
                "acceptable.<p>"
                "An example set of parameters is <i>(2,-1) (3,4) (5,-4)</i>, "
                "representing the Poincar&eacute; homology sphere.</qt>"));
            return 0;
        }

        // Build the Seifert fibred space.
        regina::NSFSpace sfs;
        long a, b;
        long d, u, v;
        long pos = 0;
        long whichPair = 1;

        while ((pos = reSFSParamPair.indexIn(sfsParams->text(), pos)) >= 0) {
            a = reSFSParamPair.cap(1).toLong();
            b = reSFSParamPair.cap(2).toLong();

            if (a == 0) {
                KMessageBox::error(parentWidget,
                    i18n("<qt>None of the parameters "
                    "<i>a<sub>1</sub></i>, <i>a<sub>2</sub></i>, ..., "
                    "<i>a<sub>n</sub></i> may be zero.</qt>"));
                return 0;
            }

            // For gcd calculations, use gcdWithCoeffs() which can cope with
            // negatives.
            d = regina::gcdWithCoeffs(a, b, u, v);
            if (d != 1 && d != -1) {
                KMessageBox::error(parentWidget, i18n("<qt>The two parameters "
                    "<i>a<sub>%1</sub> = %2</i> and "
                    "<i>b<sub>%3</sub> = %4</i> must be "
                    "relatively prime.</qt>").
                    arg(whichPair).arg(a).arg(whichPair).arg(b));
                return 0;
            }

            if (a < 0)
                sfs.insertFibre(-a, -b);
            else
                sfs.insertFibre(a, b);

            pos += reSFSParamPair.matchedLength();
            whichPair++;
        }

        NTriangulation* ans = sfs.construct();
        return ans;
    } else if (typeId == TRI_AUG_TRI_SOLID_TORUS) {
        if (! reSFS3Params.exactMatch(augParams->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>All six integer "
                "parameters (<i>a<sub>1</sub></i>,<i>b<sub>1</sub></i>) "
                "(<i>a<sub>2</sub></i>,<i>b<sub>2</sub></i>) "
                "(<i>a<sub>3</sub></i>,<i>b<sub>3</sub></i>) "
                "must be supplied.  The two integers "
                "in each pair must be relatively prime, and both "
                "positive and negative integers are allowed.<p>"
                "Example parameters are <i>(2,1) (3,-2) (5,-4)</i>.</qt>"));
            return 0;
        }

        long a1 = reSFS3Params.cap(1).toLong();
        long b1 = reSFS3Params.cap(2).toLong();
        long a2 = reSFS3Params.cap(3).toLong();
        long b2 = reSFS3Params.cap(4).toLong();
        long a3 = reSFS3Params.cap(5).toLong();
        long b3 = reSFS3Params.cap(6).toLong();

        // For gcd calculations, use gcdWithCoeffs() which can cope with
        // negatives.
        long d, u, v;
        d = regina::gcdWithCoeffs(a1, b1, u, v);
        if (d != 1 && d != -1) {
            KMessageBox::error(parentWidget, i18n("<qt>The two parameters "
                "<i>a<sub>1</sub></i> and <i>b<sub>1</sub></i> must be "
                "relatively prime.</qt>"));
            return 0;
        }
        d = regina::gcdWithCoeffs(a2, b2, u, v);
        if (d != 1 && d != -1) {
            KMessageBox::error(parentWidget, i18n("<qt>The two parameters "
                "<i>a<sub>2</sub></i> and <i>b<sub>2</sub></i> must be "
                "relatively prime.</qt>"));
            return 0;
        }
        d = regina::gcdWithCoeffs(a3, b3, u, v);
        if (d != 1 && d != -1) {
            KMessageBox::error(parentWidget, i18n("<qt>The two parameters "
                "<i>a<sub>3</sub></i> and <i>b<sub>3</sub></i> must be "
                "relatively prime.</qt>"));
            return 0;
        }

        // All okay.
        NTriangulation* ans = new NTriangulation();
        ans->insertAugTriSolidTorus(a1, b1, a2, b2, a3, b3);
        return ans;
    } else if (typeId == TRI_ISOSIG) {
        if (! reIsoSig.exactMatch(isoSig->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The isomorphism "
                "signature must be a sequence of symbols, which may include "
                "letters, digits, plus and/or minus but nothing else.  "
                "An example isomorphism signature is <i>bkaagj</i>.</qt>"));
            return 0;
        }

        NTriangulation* ans = NTriangulation::fromIsoSig(
            reIsoSig.cap(1).toAscii().constData());
        if (ans)
            return ans;
        KMessageBox::error(parentWidget, i18n("<qt>The given "
            "isomorphism signature was not valid.</qt>"));
        return 0;
    } else if (typeId == TRI_DEHYDRATION) {
        if (! reDehydration.exactMatch(dehydrationString->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The dehydration "
                "string must be a sequence of letters of the alphabet.  "
                "An example dehydration string is <i>baaaade</i>.<p>"
                "Dehydration strings are described in detail in "
                "<i>A census of cusped hyperbolic 3-manifolds</i>, "
                "Callahan, Hildebrand and Weeks, published in "
                "<i>Mathematics of Computation</i> <b>68</b>, 1999.</qt>"));
            return 0;
        }

        NTriangulation* ans = new NTriangulation();
        if (! ans->insertRehydration(
                reDehydration.cap(1).toAscii().constData())) {
            delete ans;
            KMessageBox::error(parentWidget, i18n("<qt>The given "
                "dehydration string was not valid.<p>"
                "Dehydration strings are described in detail in "
                "<i>A census of cusped hyperbolic 3-manifolds</i>, "
                "Callahan, Hildebrand and Weeks, published in "
                "<i>Mathematics of Computation</i> <b>68</b>, 1999.</qt>"));
            return 0;
        }
        return ans;
    } else if (typeId == TRI_SPLITTING_SURFACE) {
        if (! reSignature.exactMatch(splittingSignature->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The splitting "
                "surface signature must be a sequence of cycles.  "
                "Cycles should consist of letters of the alphabet and "
                "should be separated by brackets, periods or commas.  "
                "An example splitting surface signature is "
                "<i>(abb)(ac)(c)</i>.<p>"
                "Splitting surface signatures are described in detail in "
                "<i>Minimal triangulations and normal surfaces</i>, "
                "Burton, PhD thesis, available from the Regina website.</qt>"));
            return 0;
        }

        regina::NSignature* sig = regina::NSignature::parse(
            reSignature.cap(1).toAscii().constData());
        if (! sig) {
            KMessageBox::error(parentWidget, i18n("<qt>The given "
                "splitting surface signature was not valid.<p>"
                "Splitting surface signatures are described in detail in "
                "<i>Minimal triangulations and normal surfaces</i>, "
                "Burton, PhD thesis, available from the Regina website.</qt>"));
            return 0;
        }
        NTriangulation* ans = sig->triangulate();
        delete sig;
        return ans;
    } else if (typeId == TRI_EXAMPLE) {
        switch (exampleWhich->currentIndex()) {
            case EXAMPLE_S3_ONETET:
                return NExampleTriangulation::threeSphere();
            case EXAMPLE_S3_BING:
                return NExampleTriangulation::bingsHouse();
            case EXAMPLE_RP3RP3:
                return NExampleTriangulation::rp3rp3();
            case EXAMPLE_FIGURE8:
                return NExampleTriangulation::figureEightKnotComplement();
            case EXAMPLE_GIESEKING:
                return NExampleTriangulation::gieseking();
            case EXAMPLE_LENS8_3:
                return NExampleTriangulation::lens8_3();
            case EXAMPLE_POINCARE:
                return NExampleTriangulation::poincareHomologySphere();
            case EXAMPLE_RP2xS1:
                return NExampleTriangulation::rp2xs1();
            case EXAMPLE_S2xS1:
                return NExampleTriangulation::s2xs1();
            case EXAMPLE_SOLIDKLEIN:
                return NExampleTriangulation::solidKleinBottle();
            case EXAMPLE_WEBERSEIFERT:
                return NExampleTriangulation::weberSeifert();
            case EXAMPLE_WHITEHEAD:
                return NExampleTriangulation::whiteheadLinkComplement();
        }

        KMessageBox::error(parentWidget, i18n("No example triangulation has "
            "been selected."));
        return 0;
    }

    KMessageBox::error(parentWidget, i18n("No triangulation type has "
        "been selected."));
    return 0;
}

