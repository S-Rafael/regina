
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
 *                                                                        *
 *  Copyright (c) 1999-2003, Ben Burton                                   *
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
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,        *
 *  MA 02111-1307, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include "progress/nprogress.h"
#include <boost/python.hpp>

using namespace boost::python;
using regina::NProgress;

void addNProgress() {
    class_<NProgress, bases<regina::ShareableObject>,
            std::auto_ptr<NProgress>, boost::noncopyable>
            ("NProgress", no_init)
        .def("hasChanged", &NProgress::hasChanged)
        .def("isFinished", &NProgress::isFinished)
        .def("setFinished", &NProgress::setFinished)
        .def("cancel", &NProgress::cancel)
        .def("isCancelled", &NProgress::isCancelled)
        .def("getDescription", &NProgress::getDescription)
        .def("isPercent", &NProgress::isPercent)
        .def("getPercent", &NProgress::getPercent)
    ;
}

