
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

/*! \file gluingequations.h
 *  \brief Provides a UI tab for displaying gluing equations
 *  of a 3-manifold triangulation.
 */

#ifndef __GLUINGEQUATIONS_H__
#define __GLUINGEQUATIONS_H__

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QPlainTextEdit>
#include <QString>

#include "maths/matrix.h" // For MatrixInt
#include "triangulation/dim3.h"
#include "packettabui.h"

/**
 * The GUI tab for reading off Thurston's gluing equations.
 */
class Tri3ThEqs : public QObject, public PacketViewerTab
{
    Q_OBJECT
private:
    /**
     * Packet details
     */
    regina::Triangulation<3>* tri;

    /**
     * Internal UI components
     */
    QCheckBox* latex;
    QBoxLayout* layout;
    QPlainTextEdit* textarea;
    QLabel* title;
    QWidget* ui;

    /**
     * Neumann-Zagier matrices
     */
    regina::MatrixInt edgeEquations;

public:
    /**
     * Class constructor.
     */
    Tri3ThEqs(regina::Triangulation<3>* packet,
              PacketTabbedViewerTab* useParentUI);

    /**
     * @brief getPacket - get the associated triangulation
     * @return a pointer to the associated Triangulation<3> object
     */
    inline regina::Packet* getPacket() final override { return tri; }

    /**
     * @brief getInterface - get the UI
     * @return a pointer to the QWidget representing the UI
     */
    inline QWidget* getInterface() final override { return ui; }

    inline void refresh() final override { recalculate(); };

private:
    /**
     * Converts a matrix of integers to a QString.
     * If the matrix is empty, the return value contains the
     * text "(empty matrix)".
     * TODO: perhaps this should be a member function of the Matrix class?
     */
    QString matrixToString(const regina::MatrixInt&) const;

    /**
     * Converts a matrix to a Latex source string representing it.
     * TODO: perhaps this should be a member function of the Matrix class?
     */
    QString matrixToLatex(const regina::MatrixInt&) const;

    /**
     * Recalculates the gluing matrices
     */
    void recalculate();

public slots:
    /**
     * Populates the text area with a text representation of
     * the matrix
     */
    void display(bool latexMode);
};

#endif // __GLUINGEQUATIONS_H__
