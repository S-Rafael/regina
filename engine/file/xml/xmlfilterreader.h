
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

/*! \file surfaces/xmlfilterreader.h
 *  \brief Deals with parsing XML data for normal surface filters.
 */

#ifndef __REGINA_XMLFILTERREADER_H
#ifndef __DOXYGEN
#define __REGINA_XMLFILTERREADER_H
#endif

#include "regina-core.h"
#include "file/xml/xmlpacketreader.h"
#include "surfaces/surfacefilter.h"

namespace regina {

/**
 * An XML packet reader that reads a single surface filter using the
 * older second-generation file format.
 * The filter type will be determined by this class and an appropriate
 * subclassed filter reader will be used to process the type-specific details.
 *
 * \ifacespython Not present.
 */
class XMLLegacyFilterReader : public XMLPacketReader {
    private:
        XMLPacketReader* dataReader_;
            /**< The subclassed filter reader responsible for reading
                 the "real" content. */
        Packet* filter_;
            /**< The filter that was read by \a dataReader_. */

    public:
        /**
         * Creates a new surface filter packet reader.
         *
         * All parameters are the same as for the parent class XMLPacketReader.
         */
        XMLLegacyFilterReader(XMLTreeResolver& resolver, Packet* parent,
            bool anon, std::string label, std::string id);

        virtual Packet* packetToCommit() override;
        virtual XMLElementReader* startContentSubElement(
            const std::string& subTagName,
            const regina::xml::XMLPropertyDict& subTagProps) override;
        virtual void endContentSubElement(const std::string& subTagName,
            XMLElementReader* subReader) override;
};

/**
 * An XML packet reader that reads a plain (non-subclassed) SurfaceFilter.
 *
 * \ifacespython Not present.
 */
class XMLPlainFilterReader : public XMLPacketReader {
    private:
        SurfaceFilter* filter_;
            /**< The filter currently being read. */

    public:
        /**
         * Creates a new surface filter packet reader.
         *
         * All parameters are the same as for the parent class XMLPacketReader.
         */
        XMLPlainFilterReader(XMLTreeResolver& resolver, Packet* parent,
            bool anon, std::string label, std::string id);

        virtual Packet* packetToCommit() override;
};

/**
 * An XML packet reader that reads a single SurfaceFilterCombination filter.
 *
 * \ifacespython Not present.
 */
class XMLCombinationFilterReader : public XMLPacketReader {
    private:
        SurfaceFilterCombination* filter_;
            /**< The filter currently being read. */

    public:
        /**
         * Creates a new surface filter packet reader.
         *
         * All parameters not explained here are the same as for the
         * parent class XMLPacketReader.
         *
         * @param props the attributes of the \c surfaces XML element.
         */
        XMLCombinationFilterReader(XMLTreeResolver& resolver, Packet* parent,
            bool anon, std::string label, std::string id,
            const regina::xml::XMLPropertyDict& props);

        virtual Packet* packetToCommit() override;
};

/**
 * An XML packet reader that reads a single SurfaceFilterCombination filter
 * using the older second-generation file format.
 *
 * \ifacespython Not present.
 */
class XMLLegacyCombinationFilterReader : public XMLPacketReader {
    private:
        SurfaceFilterCombination* filter_;
            /**< The filter currently being read. */

    public:
        /**
         * Creates a new surface filter packet reader.
         *
         * All parameters are the same as for the parent class XMLPacketReader.
         */
        XMLLegacyCombinationFilterReader(XMLTreeResolver& resolver,
            Packet* parent, bool anon, std::string label, std::string id);

        virtual Packet* packetToCommit() override;
        XMLElementReader* startContentSubElement(const std::string& subTagName,
                const regina::xml::XMLPropertyDict& props) override;
};

/**
 * An XML packet reader that reads a single SurfaceFilterProperties filter.
 *
 * \ifacespython Not present.
 */
class XMLPropertiesFilterReader : public XMLPacketReader {
    private:
        SurfaceFilterProperties* filter_;
            /**< The filter currently being read. */

    public:
        /**
         * Creates a new surface filter packet reader.
         *
         * All parameters not explained here are the same as for the
         * parent class XMLPacketReader.
         *
         * @param props the attributes of the \c surfaces XML element.
         */
        XMLPropertiesFilterReader(XMLTreeResolver& resolver, Packet* parent,
            bool anon, std::string label, std::string id,
            const regina::xml::XMLPropertyDict& props);

        virtual Packet* packetToCommit() override;
};

/**
 * An XML packet reader that reads a single SurfaceFilterProperties filter
 * using the older second-generation file format.
 *
 * \ifacespython Not present.
 */
class XMLLegacyPropertiesFilterReader : public XMLPacketReader {
    private:
        SurfaceFilterProperties* filter_;
            /**< The filter currently being read. */

    public:
        /**
         * Creates a new surface filter packet reader.
         *
         * All parameters are the same as for the parent class XMLPacketReader.
         */
        XMLLegacyPropertiesFilterReader(XMLTreeResolver& resolver,
            Packet* parent, bool anon, std::string label, std::string id);

        virtual Packet* packetToCommit() override;
        XMLElementReader* startContentSubElement(const std::string& subTagName,
                const regina::xml::XMLPropertyDict& props) override;
        void endContentSubElement(const std::string& subTagName,
                XMLElementReader* subReader) override;
};

// Inline functions for XMLLegacyFilterReader

inline XMLLegacyFilterReader::XMLLegacyFilterReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        filter_(nullptr) {
}

inline Packet* XMLLegacyFilterReader::packetToCommit() {
    return filter_;
}

// Inline functions for XMLPlainFilterReader:

inline XMLPlainFilterReader::XMLPlainFilterReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        filter_(new SurfaceFilter()) {
}

inline Packet* XMLPlainFilterReader::packetToCommit() {
    return filter_;
}

// Inline functions for XMLCombinationFilterReader:

inline Packet* XMLCombinationFilterReader::packetToCommit() {
    return filter_;
}

// Inline functions for XMLLegacyCombinationFilterReader:

inline XMLLegacyCombinationFilterReader::XMLLegacyCombinationFilterReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        filter_(nullptr) {
}

inline Packet* XMLLegacyCombinationFilterReader::packetToCommit() {
    return filter_;
}

// Inline functions for XMLPropertiesFilterReader:

inline Packet* XMLPropertiesFilterReader::packetToCommit() {
    return filter_;
}

// Inline functions for XMLLegacyPropertiesFilterReader:

inline XMLLegacyPropertiesFilterReader::XMLLegacyPropertiesFilterReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        filter_(new SurfaceFilterProperties()) {
}

inline Packet* XMLLegacyPropertiesFilterReader::packetToCommit() {
    return filter_;
}

} // namespace regina

#endif

