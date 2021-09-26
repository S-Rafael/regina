
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

#include <QLabel>
#include <QLayout>
#include <QPlainTextEdit>

#include "gluingequations.h"
#include "triangulation/dim3.h"
#include "packettabui.h"

using regina::Packet;
using regina::Triangulation;

Tri3ThEqs::Tri3ThEqs(regina::Triangulation<3>* packet,
                     PacketTabbedViewerTab* useParentUI) :
    PacketViewerTab(useParentUI), tri(packet)
{
    // Construct the objects representing the UI
    ui = new QWidget();
    layout = new QVBoxLayout(ui);
    title = new QLabel(tr("Matrix of Thurston's edge consistency equations:"), ui);
    layout->addWidget(title);
    textarea = new QPlainTextEdit();
    // We try to create a monospace font, which may be actually mapped
    // to different fonts on different target systems.
    QFont font("Monospace", 12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    textarea->document()->setDefaultFont(font);
    textarea->setFont(font);
    textarea->setReadOnly(true);
    textarea->setPlainText("Nothing to see here, at least for now...");
    layout->addWidget(textarea);
}
