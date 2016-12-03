
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Qt User Interface                                                     *
 *                                                                        *
 *  Copyright (c) 1999-2016, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This file is modified from the KDE syntax-highlighting framework,     *
 *  which is copyright (C) 2016 Volker Krause <vkrause@kde.org>           *
 *  and is distributed under the GNU Library General Public License as    *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
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

#include "syntax/abstracthighlighter.h"
#include "syntax/context_p.h"
#include "syntax/definition_p.h"
#include "syntax/format.h"
#include "syntax/qtmatcher.h"
#include "syntax/repository.h"
#include "syntax/rule_p.h"
#include "syntax/state.h"
#include "syntax/state_p.h"
#include "syntax/theme.h"

#include <cassert>
#include <iostream>

namespace regina {
namespace syntax {

void AbstractHighlighter::ensureDefinitionLoaded()
{
    auto defData = DefinitionData::get(m_definition);
    if (!m_definition.isValid() && defData->repo && !m_definition.name().empty()) {
        std::cerr << "Definition became invalid, trying re-lookup." << std::endl;
        m_definition = defData->repo->definitionForName(m_definition.name());
        defData = DefinitionData::get(m_definition);
    }

    if (!defData->repo && !defData->name.empty())
        std::cerr << "Repository got deleted while a highlighter is still active!" << std::endl;

    if (m_definition.isValid())
        defData->load();
}

/**
 * Returns the index of the first non-space character. If the line is empty,
 * or only contains white spaces, -1 is returned.
 */
static inline int firstNonSpaceChar(const QString & text)
{
    for (int i = 0; i < text.length(); ++i) {
        if (!text[i].isSpace()) {
            return i;
        }
    }
    return -1;
}

Definition AbstractHighlighter::definition() const
{
    return m_definition;
}

void AbstractHighlighter::setDefinition(const Definition &def)
{
    m_definition = def;
}

Theme AbstractHighlighter::theme() const
{
    return m_theme;
}

void AbstractHighlighter::setTheme(const Theme &theme)
{
    m_theme = theme;
}

State AbstractHighlighter::highlightLine(const QString& text, const State &state)
{
    // verify definition, deal with no highlighting being enabled
    ensureDefinitionLoaded();
    if (!m_definition.isValid()) {
        applyFormat(0, text.size(), Format());
        return State();
    }

    // verify/initialize state
    auto defData = DefinitionData::get(m_definition);
    auto newState = state.copy();
    auto stateData = StateData::get(newState);
    if (stateData->m_defData && defData != stateData->m_defData) {
        std::cerr << "Got invalid state, resetting." << std::endl;
        stateData->clear();
    }
    if (stateData->isEmpty()) {
        stateData->push(defData->initialContext());
        stateData->m_defData = defData;
    }

    // process empty lines
    if (text.isEmpty()) {
        while (!stateData->topContext()->lineEmptyContext().isStay())
            switchContext(stateData, stateData->topContext()->lineEmptyContext());
        applyFormat(0, 0, Format());
        return newState;
    }

    assert(!stateData->isEmpty());
    int firstNonSpace = firstNonSpaceChar(text);
    if (firstNonSpace < 0) {
        firstNonSpace = text.size();
    }
    int offset = 0, beginOffset = 0;
    auto currentLookupContext = stateData->topContext();
    auto currentFormat = currentLookupContext->attribute();
    bool lineContinuation = false;
    std::map<Rule*, int> skipOffsets;

    do {
        bool isLookAhead = false;
        int newOffset = 0;
        std::string newFormat;
        auto newLookupContext = currentLookupContext;
        for (const auto &rule : stateData->topContext()->rules()) {
            auto it = skipOffsets.find(rule.get());
            if (it != skipOffsets.end() && it->second > offset)
                continue;

            // filter out rules that only match for leading whitespace
            if (rule->firstNonSpace() && (offset > firstNonSpace)) {
                continue;
            }

            // filter out rules that require a specific column
            if ((rule->requiredColumn() >= 0) && (rule->requiredColumn() != offset)) {
                continue;
            }

            QtMatcher m(text);
            const auto newResult = rule->match(m, offset);
            newOffset = newResult.offset();
            if (newResult.skipOffset() > newOffset)
                skipOffsets.insert(std::make_pair(rule.get(), newResult.skipOffset()));
            if (newOffset <= offset)
                continue;

            if (rule->isLookAhead()) {
                assert(!rule->context().isStay());
                switchContext(stateData, rule->context());
                isLookAhead = true;
                break;
            }

            newLookupContext = stateData->topContext();
            switchContext(stateData, rule->context());
            newFormat = rule->attribute().empty() ? stateData->topContext()->attribute() : rule->attribute();
            if (newOffset == text.size() && std::dynamic_pointer_cast<LineContinue>(rule))
                lineContinuation = true;
            break;
        }
        if (isLookAhead)
            continue;

        if (newOffset <= offset) { // no matching rule
            if (stateData->topContext()->fallthrough()) {
                switchContext(stateData, stateData->topContext()->fallthroughContext());
                continue;
            }

            newOffset = offset + 1;
            newLookupContext = stateData->topContext();
            newFormat = newLookupContext->attribute();
        }

        if (newFormat != currentFormat /*|| currentLookupDef != newLookupDef*/) {
            if (offset > 0)
                applyFormat(beginOffset, offset - beginOffset, currentLookupContext->formatByName(currentFormat));
            beginOffset = offset;
            currentFormat = newFormat;
            currentLookupContext = newLookupContext;
        }
        assert(newOffset > offset);
        offset = newOffset;

    } while (offset < text.size());

    if (beginOffset < offset)
        applyFormat(beginOffset, text.size() - beginOffset, currentLookupContext->formatByName(currentFormat));

    while (!stateData->topContext()->lineEndContext().isStay() && !lineContinuation) {
        if (!switchContext(stateData, stateData->topContext()->lineEndContext()))
            break;
    }

    return newState;
}

bool AbstractHighlighter::switchContext(StateData *data, const ContextSwitch &contextSwitch)
{
    for (int i = 0; i < contextSwitch.popCount(); ++i) {
        // don't pop the last context if we can't push one
        if (data->size() == 1 && !contextSwitch.context())
            return false;
        if (data->size() == 0)
            break;
        data->pop();
    }

    if (contextSwitch.context())
        data->push(contextSwitch.context());

    assert(!data->isEmpty());
    return true;
}

} } // namespace regina::syntax
