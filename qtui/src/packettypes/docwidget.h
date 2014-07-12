
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
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

/*! \file docregistry.h
 *  \brief Provides a way for multiple viewers of a text-based packet to use
 *  the same underlying QTextDocument.
 */

#ifndef __DOCREGISTRY_H
#define __DOCREGISTRY_H

#include <QHash>
#include <QPlainTextDocumentLayout>
#include <QTextDocument>

class QTextDocument;

/**
 * A widget for displaying and editing the text contents of a text-based
 * packet.  The template argument PacketType should be one of Regina's
 * text-based packet types, such as NText or NScript.  In particular, it
 * must have the text-based member functions getText() and setText().
 *
 * This widget keeps an internal registry, through which each packet
 * is associated with a single QTextDocument.  This QTextDocument is
 * "global" for the packet, in that multiple DocWidget viewers for the
 * same packet will work with the same underlying document, thus keeping
 * all viewers in sync.
 */
template <class PacketType>
class DocWidget : public QPlainTextEdit {
    private:
        struct Details {
            QTextDocument* doc;
            int users;
        };
        typedef QHash<PacketType*, Details> Registry;
        static Registry registry_;

        PacketType* packet_;

    public:
        DocWidget(PacketType* packet, QWidget* parent);
        ~DocWidget();

        /**
         * Refresh this widget with the contents of the packet from the
         * calculation engine.
         */
        void refresh();

    protected:
        /**
         * QWidget overrides.
         */
        virtual void focusOutEvent(QFocusEvent*);
};

template <class PacketType>
typename DocWidget<PacketType>::Registry DocWidget<PacketType>::registry_;

template <class PacketType>
DocWidget<PacketType>::DocWidget(PacketType* packet, QWidget* parent) :
        QPlainTextEdit(parent), packet_(packet) {
    // Find the QTextDocument in the registry for this packet, or create
    // a new document if this packet is not yet registered.
    typename Registry::Iterator it = registry_.find(packet);
    if (it != registry_.end()) {
        ++it->users;
        setDocument(it->doc);
    } else {
        Details d;
        d.doc = new QTextDocument(packet->getText().c_str());
        d.doc->setDocumentLayout(new QPlainTextDocumentLayout(d.doc));
        d.users = 1;
        registry_.insert(packet, d);
        setDocument(d.doc);
    }
}

template <class PacketType>
DocWidget<PacketType>::~DocWidget() {
    // Push any outstanding changes to the calculation engine.
    packet_->setText(toPlainText().toAscii().constData());

    // If we are the last DocWidget registered for this packet, delete
    // the underlying QTextDocument.
    typename Registry::Iterator it = registry_.find(packet_);
    if (it != registry_.end()) { // Should always be true, but just in case..
        --it->users;
        if (it->users == 0) {
            delete it->doc;
            registry_.erase(it);
        }
    }
}

template <class PacketType>
void DocWidget<PacketType>::refresh() {
    setPlainText(packet_->getText().c_str());
    moveCursor(QTextCursor::Start);
}

template <class PacketType>
void DocWidget<PacketType>::focusOutEvent(QFocusEvent*) {
    // Push the pending edit to the calculation engine.
    packet_->setText(toPlainText().toAscii().constData());
}

#endif