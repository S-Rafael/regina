
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

#include <list>
#include "file/xml/xmlfilterreader.h"
#include "utilities/stringutils.h"

namespace regina {

XMLElementReader* XMLLegacyFilterReader::startContentSubElement(
        const std::string& subTagName,
        const regina::xml::XMLPropertyDict& props) {
    if (! filter_)
        if (subTagName == "filter") {
            int type;
            // Run through all the filter types that the file format supports.
            if (valueOf(props.lookup("typeid"), type)) {
                switch (static_cast<SurfaceFilterType>(type)) {
                    case NS_FILTER_DEFAULT:
                        return dataReader_ =
                            new XMLPlainFilterReader(resolver_,
                                parent_, anon_, label_, id_);
                    case NS_FILTER_PROPERTIES:
                        return dataReader_ =
                            new XMLLegacyPropertiesFilterReader(resolver_,
                                parent_, anon_, label_, id_);
                    case NS_FILTER_COMBINATION:
                        return dataReader_ =
                            new XMLLegacyCombinationFilterReader(resolver_,
                                parent_, anon_, label_, id_);
                    default:
                        return new XMLPacketReader(resolver_,
                            parent_, anon_, label_, id_);
                }
            }
        }
    return new XMLElementReader();
}

void XMLLegacyFilterReader::endContentSubElement(const std::string& subTagName,
        XMLElementReader* subReader) {
    if (dataReader_)
        filter_ = dataReader_->packetToCommit();
}

XMLCombinationFilterReader::XMLCombinationFilterReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id,
        const regina::xml::XMLPropertyDict& props) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        filter_(nullptr) {
    std::string type = props.lookup("op");
    if (type == "and") {
        filter_ = new SurfaceFilterCombination();
        filter_->setUsesAnd(true);
    } else if (type == "or") {
        filter_ = new SurfaceFilterCombination();
        filter_->setUsesAnd(false);
    }
}

XMLElementReader* XMLLegacyCombinationFilterReader::startContentSubElement(
        const std::string& subTagName,
        const regina::xml::XMLPropertyDict& props) {
    if (! filter_)
        if (subTagName == "op") {
            std::string type = props.lookup("type");
            if (type == "and") {
                filter_ = new SurfaceFilterCombination();
                filter_->setUsesAnd(true);
            } else if (type == "or") {
                filter_ = new SurfaceFilterCombination();
                filter_->setUsesAnd(false);
            }
        }
    return new XMLElementReader();
}

XMLPropertiesFilterReader::XMLPropertiesFilterReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id,
        const regina::xml::XMLPropertyDict& props) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        filter_(nullptr) {
    BoolSet orbl, compact, realbdry;

    auto prop = props.find("orbl");
    if (prop != props.end()) {
        if (! valueOf(prop->second, orbl))
            return;
    } else
        orbl.fill();

    prop = props.find("compact");
    if (prop != props.end()) {
        if (! valueOf(prop->second, compact))
            return;
    } else
        compact.fill();

    prop = props.find("realbdry");
    if (prop != props.end()) {
        if (! valueOf(prop->second, realbdry))
            return;
    } else
        realbdry.fill();

    // We have successfully parsed the BoolSet attributes.
    filter_ = new SurfaceFilterProperties();
    filter_->setOrientability(orbl);
    filter_->setCompactness(compact);
    filter_->setRealBoundary(realbdry);

    std::string euler = props.lookup("euler");
    if (! euler.empty()) {
        std::istringstream str(euler);

        std::string val;
        LargeInteger i;
        while (std::getline(str, val, ',')) {
            if (valueOf(val, i))
                filter_->addEulerChar(i);
            else {
                // We cannot parse the list of Euler characteristics.  Abort.
                delete filter_;
                filter_ = nullptr;
                return;
            }
        }
    }
}

XMLElementReader* XMLLegacyPropertiesFilterReader::startContentSubElement(
        const std::string& subTagName,
        const regina::xml::XMLPropertyDict& props) {
    if (subTagName == "euler") {
        return new XMLCharsReader();
    } else if (subTagName == "orbl") {
        BoolSet b;
        if (valueOf(props.lookup("value"), b))
            filter_->setOrientability(b);
    } else if (subTagName == "compact") {
        BoolSet b;
        if (valueOf(props.lookup("value"), b))
            filter_->setCompactness(b);
    } else if (subTagName == "realbdry") {
        BoolSet b;
        if (valueOf(props.lookup("value"), b))
            filter_->setRealBoundary(b);
    }
    return new XMLElementReader();
}

void XMLLegacyPropertiesFilterReader::endContentSubElement(
        const std::string& subTagName, XMLElementReader* subReader) {
    if (subTagName == "euler") {
        std::list<std::string> tokens;
        basicTokenise(back_inserter(tokens),
            dynamic_cast<XMLCharsReader*>(subReader)->chars());

        LargeInteger val;
        for (std::list<std::string>::const_iterator it =
                tokens.begin(); it != tokens.end(); it++)
            if (valueOf(*it, val))
                filter_->addEulerChar(val);
    }
}

} // namespace regina

