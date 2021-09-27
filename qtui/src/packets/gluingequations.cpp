
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Qt User Interface                                                     *
 *  Tab for displaying gluing equations                                   *
 *                                                                        *
 *  Author: Rafael M. Siejakowski <rs@rs-math.net>                        *
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

#include <sstream>

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QPlainTextEdit>
#include <QString>

#include "gluingequations.h"
#include "triangulation/dim3.h"
#include "packettabui.h"

using regina::Packet;
using regina::Triangulation;

Tri3ThEqs::Tri3ThEqs(regina::Triangulation<3>* packet,
                     PacketTabbedViewerTab* useParentUI) :
    PacketViewerTab{useParentUI}, tri{packet}
{
    // Construct the objects representing the UI
    ui = new QWidget();
    layout = new QVBoxLayout(ui);
    title = new QLabel(tr("Coefficients of angle equations about internal edges:"), ui);
    layout->addWidget(title);

    textarea = new QPlainTextEdit(ui);
    // We try to create a 'monospace' font, which may be actually mapped
    // to different fonts on different target systems. For this reason,
    // the "style hint" is probably more important.
    QFont font("Monospace", 12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    textarea->document()->setDefaultFont(font);
    textarea->setFont(font);
    textarea->setReadOnly(true);
    layout->addWidget(textarea);

    latex = new QCheckBox(tr("Show &Latex"), ui);
    layout->addWidget(latex);
    // Bind the checkbox's change of state
    connect(latex, SIGNAL(toggled(bool)), this, SLOT(display(bool)));
    refresh(); // recalculate the matrices
}

QString Tri3ThEqs::matrixToString(const regina::MatrixInt& M)
{
    if (!M.columns() || !M.rows())
        return QString("(empty matrix)");

    std::string text = M.detail();
    // Note: the QString constructor copies the data (rewriting it
    // as UTF-8), so the QString will be good even after the variable
    // `text` goes out of scope:
    return QString(text.c_str());
}

QString Tri3ThEqs::matrixToLatex(const regina::MatrixInt& M)
{
    if (!M.columns() || !M.rows())
        return QString("(empty matrix)");

    std::stringstream buffer;
    // Start matrix environment and print the (0,0)-entry
    buffer << "\\begin{" << LatexMatrixEnvironment << "}\n  " << M.entry(0,0);
    // Print the rest of row 0:
    for (size_t c = 1; c < M.columns(); c++)
        buffer << " & " << M.entry(0,c);
    // Print the remaining rows, if any:
    for (size_t r = 1; r < M.rows(); r++)
    {
        buffer << " \\\\\n  " << M.entry(r, 0);
        for (size_t c = 1; c < M.columns(); c++)
            buffer << " & " << M.entry(r,c);
    }
    // Close the Latex environment
    buffer << "\n\\end{" << LatexMatrixEnvironment << "}";
    std::string text = buffer.str();
    const char* c_string = text.c_str();
    // Note: the QString constructor copies the data (rewriting it
    // as UTF-8), so the QString will be good even after the variable
    // `text` goes out of scope:
    return QString(c_string);
}

void Tri3ThEqs::display(bool latexMode)
{
    textarea->setPlainText(latexMode?
        matrixToLatex(edgeEquations):
       matrixToString(edgeEquations)
                          );
}