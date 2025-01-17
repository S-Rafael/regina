
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

/*! \file packet/xmltreeresolver.h
 *  \brief Support for resolving dangling packet references after a
 *  complete packet tree has been read from file.
 */

#ifndef __REGINA_XMLTREERESOLVER_H
#ifndef __DOXYGEN
#define __REGINA_XMLTREERESOLVER_H
#endif

#include "packet/container.h"
#include <list>
#include <map>

namespace regina {

class Packet;

class XMLTreeResolver;

/**
 * An individual task for resolving dangling packet references after an
 * XML data file has been read.
 *
 * See the XMLTreeResolver class notes for an overview of how dangling
 * references and related issues are resolved, and the role that
 * XMLTreeResolutionTask plays in this process.
 *
 * Specifically, if an individual XMLPacketReader cannot
 * completely flesh out the internal data for a packet as the packet is
 * being read, it should construct a new XMLTreeResolutionTask and
 * queue it to the master XMLTreeResolver.  The XMLTreeResolver will
 * then call resolve() for each queued task after the complete data file
 * has been read, at which point whatever information was missing when the
 * packet was initially read should now be available.
 *
 * Each packet reader that requires this machinery should subclass
 * XMLTreeResolutionTask, and override resolve() to perform whatever
 * "fleshing out" procedure is required for its particular type of packet.
 */
class XMLTreeResolutionTask {
    public:
        /**
         * A default construct that does nothing.
         */
        virtual ~XMLTreeResolutionTask();
        /**
         * Called by XMLTreeResolver after the entire data file has
         * been read.  Subclasses should override this routine to
         * perform whatever "fleshing out" is necessary for a packet
         * whose internal data is not yet complete.
         *
         * @param resolver the master resolver managing the resolution
         * process, as outlined in the XMLTreeResolver class notes.
         */
        virtual void resolve(const XMLTreeResolver& resolver) = 0;
};

/**
 * Provides a mechanism to resolve cross-references between packets in
 * an XML data file, and to manage the anonymous packet pool.
 *
 * This class has three main tasks:
 *
 * - To allow immediate lookups by ID for packets that should have already
 *   been read from the XML data file (e.g., a normal surface list
 *   that needs to reference its enclosing triangulation);
 *
 * - To allow delayed lookups by ID after the complete packet tree has
 *   been read from the XML data file, thereby resolving "dangling references"
 *   where an ID is referenced before the corresonding packet appears
 *   (e.g. a script packet referencing its variables);
 *
 * - To manage an "anonymous pool" of packets that appear with an
 *   \<anon\> block within the XML data file.
 *
 * The complete process of reading an XML data file works as follows:
 *
 * - The top-level routine managing the file I/O should construct a new
 *   XMLTreeResolver.  This resolver is then passed to each XMLPacketReader
 *   in turn as each individual packet is read.
 *
 * - If a packet appears with an ID in the data file, this should be
 *   registered via storeID().
 *
 * - If a packet appears within an anonymousl block, this should be
 *   registered via storeAnon() (note that such packets are typically
 *   useless unless storeID() was called also).
 *
 * - If an XMLPacketReader needs to perform an immediate lookup, it
 *   should call resolve().
 *
 * - If an XMLPacketReader is not able to fully flesh out its data
 *   because it references packets that may not yet have been read, it
 *   should create a new XMLTreeResolutionTask and queue this as a
 *   "delayed lookup task" via queueTask().  Each such task should be an
 *   instance of a subclass of XMLTreeResolutionTask, whose virtual resolve()
 *   function is overridden to perform whatever "fleshing out" work is
 *   required for the type of packet under consideration.
 *
 * - Once the entire packet tree has been read, the top-level file I/O
 *   manager should call resolveDelayed().  This will run
 *   XMLTreeResolutionTask::resolve() for each task in turn, whereby any
 *   missing data for individual packets can be resolved.
 *
 * - At any time, a packet can be "extracted" from the anonymous pool
 *   by calling Packet::makeOrphan().  It will still remain registered by
 *   its ID.  After the file reading is complete, any packets that were stored
 *   in the anonymous pool and not subsequently extracted will be destroyed.
 */
class XMLTreeResolver {
    public:
        typedef std::map<std::string, Packet*> IDMap;
            /**< A type that maps internal IDs from the data file to the
                 corresponding packets.  See ids() for details. */

    private:
        IDMap ids_;
            /**< Maps internal IDs from the data file to the
                 corresponding packets. */
        std::list<XMLTreeResolutionTask*> tasks_;
            /**< The list of tasks that have been queued for processing. */
        Container anonPool;
            /**< Stores all packets that were registered via storeAnon()
                 as appearing within anonymous blocks. */

    public:
        /**
         * Constructs a resolver with no tasks queued.
         */
        XMLTreeResolver();
        /**
         * Destroys any tasks that were queued but not performed.
         */
        ~XMLTreeResolver();

        /**
         * Queues a task for processing.  When the file I/O manager
         * calls resolve(), this will call XMLTreeResolutionTask::resolve()
         * for each task that has been queued.
         *
         * This object will claim ownership of the given task, and will
         * destroy it after resolve() has been called (or, if resolve()
         * is never called, when this XMLTreeResolver is destroyed).
         *
         * @param task the task to be queued.
         */
        void queueTask(XMLTreeResolutionTask* task);
        /**
         * Stores the fact that the given packet is stored in the data
         * file using the given internal ID.  Associations between IDs
         * and packets can be queried through the resolve() function.
         * See resolve() for further information on internal IDs.
         *
         * This will be called automatically by XMLPacketReader as it
         * processes packet elements in the data file.  Users and/or subclasses
         * of XMLPacketReader do not need to call this function themselves.
         *
         * @param id the internal ID of the given packet, as stored in
         * the data file.
         * @param packet the corresponding packet.
         */
        void storeID(const std::string& id, Packet* packet);
        /**
         * Stores the given packet in the "anonymous pool".
         *
         * This will be called automatically by XMLPacketReader as it
         * processes packets that appear within an \<anon\> block.
         * Users and/or subclasses of XMLPacketReader do not need to
         * call this function themselves.
         *
         * @param packet the packet to insert into the anonymous pool.
         */
        void storeAnon(Packet* packet);

        /**
         * Identifies if some packet has been registered as having the given ID
         * within the the XML data file.
         *
         * Packets in a data file may have individual string IDs stored
         * alongside them, in the \a id attribute of the corresponding XML tag.
         * These strings are optional, and do not need to be human-readable.
         * Although packets are not required to have IDs, any IDs that \e are
         * stored must be unique (i.e., two different packets cannot
         * share the same ID).
         *
         * If a packet has an ID in the XML data file but the packet has not
         * yet been read, it will not be located by this routine (though
         * this is not a problem for "delayed resolution" tasks, which
         * are only performed once the entire file has been read).
         * If a packet does not have an ID in the XML data file, it cannot
         * be located via this routine at all.
         *
         * Note that IDs read from the data file need not bear any relation
         * to the IDs that are returned from Packet::internalID(), although
         * this is typically how they are constructed when a file is saved.
         *
         * @param id the string ID to query.
         * @return the packet with the given ID, or \c null is no such
         * packet has been registered so far.
         */
        Packet* resolve(const std::string& id) const;

        /**
         * Calls XMLTreeResolutionTask::resolve() for all queued tasks.
         *
         * The tasks will then be destroyed and removed from the queue
         * (so subsequent calls to resolveDelayed() are safe and will do
         * nothing).
         */
        void resolveDelayed();

        // Make this class non-copyable.
        XMLTreeResolver(const XMLTreeResolver&) = delete;
        XMLTreeResolver& operator = (const XMLTreeResolver&) = delete;
};

// Inline functions for XMLTreeResolutionTask

inline XMLTreeResolutionTask::~XMLTreeResolutionTask() {
}

// Inline functions for XMLTreeResolver

inline XMLTreeResolver::XMLTreeResolver() {
}

inline XMLTreeResolver::~XMLTreeResolver() {
    for (XMLTreeResolutionTask* task : tasks_)
        delete task;

    // All packets in the anonymous pool will be deleted at this point also.
}

inline void XMLTreeResolver::queueTask(XMLTreeResolutionTask* task) {
    tasks_.push_back(task);
}

inline void XMLTreeResolver::storeID(const std::string& id, Packet* packet) {
    ids_.insert(std::make_pair(id, packet));
}

inline void XMLTreeResolver::storeAnon(Packet* packet) {
    anonPool.insertChildLast(packet);
}

inline Packet* XMLTreeResolver::resolve(const std::string& id) const {
    auto pos = ids_.find(id);
    return (pos == ids_.end() ? nullptr : pos->second);
}

inline void XMLTreeResolver::resolveDelayed() {
    for (XMLTreeResolutionTask* task : tasks_) {
        task->resolve(*this);
        delete task;
    }
    tasks_.clear();
}

} // namespace regina

#endif

