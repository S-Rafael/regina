
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

/*! \file packet/container.h
 *  \brief Contains a packet whose entire life purpose is to contain
 *  other packets.
 */

#ifndef __REGINA_CONTAINER_H
#ifndef __DOXYGEN
#define __REGINA_CONTAINER_H
#endif

#include "regina-core.h"
#include "packet/packet.h"

namespace regina {

class Container;

/**
 * A packet that simply contains other packets.  Such
 * a packet contains no real data.
 *
 * \ingroup packet
 */
class Container : public Packet {
    REGINA_PACKET(Container, PACKET_CONTAINER, "Container")

    public:
        /**
         * Default constructor.
         */
        Container();

        /**
         * Constructs a new container with the given packet label.
         *
         * This constructor is (for example) helpful when you are
         * building a complex packet tree to save to a Regina data file,
         * and you are using containers to organise the data in this tree.
         *
         * @param label the packet label for this new container.
         */
        Container(const std::string& label);

        virtual void writeTextShort(std::ostream& out) const override;

    protected:
        virtual Packet* internalClonePacket(Packet* parent) const override;
        virtual void writeXMLPacketData(std::ostream& out,
            FileFormat format, bool anon, PacketRefs& refs) const override;
};

// Inline functions for Container

inline Container::Container() {
}

inline Container::Container(const std::string& label) {
    setLabel(label);
}

inline void Container::writeTextShort(std::ostream& o) const {
    o << "Container";
}

inline Packet* Container::internalClonePacket(Packet*) const {
    return new Container();
}

inline void Container::writeXMLPacketData(std::ostream& out, FileFormat format,
        bool anon, PacketRefs& refs) const {
    writeXMLHeader(out, "container", format, anon, refs);
    writeXMLFooter(out, "container", format, anon, refs);
}

} // namespace regina

#endif

