
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

#include <sstream>
#include <string>
#include "algebra/binaryA5.h"

namespace regina {

// technically this is just the cocycle on A5 which describes the non-trivial 
// Z_2-central extension of A5 elements of A5 are listed in order of 
// Perm<5>::S5 ie, the i-th A5 element is Perm<5>::S5[2*i].
const unsigned long long BinaryA5::mult_table[60] = { 0x0LL, 
    0x4c591312852a12LL, 0xca186404c32412cLL, 0x6b346c1e81e78ecLL, 
    0x1c498390025c432LL, 0x1a416418806d0b8LL, 0xe014202cc983948LL, 
    0x2804503828e788aLL, 0xc31e2ee4df0274cLL, 0xc40681645c12f00LL, 
    0x860aa7c0c210514LL, 0x6096117d3dd3bc2LL, 0xffb0207138e78e8LL, 
    0xcb1081fb7983bccLL, 0x75fa6c3020fd0baLL, 0x84c4689006fd010LL, 
    0x94c69b18a73ce12LL, 0xe391648008ff02cLL, 0x95e6013123ef882LL, 
    0xc60000912deb908LL, 0xca1140c2cdd254cLL, 0xe0017c82c65c534LL, 
    0x860451928f18700LL, 0x9646811b3f82b42LL, 0x2a19816ef002900LL, 
    0x6a31c0e13853a82LL, 0x1a0ebb2ed32050cLL, 0x28177e1ec7381ecLL, 
    0x9a0981d32903012LL, 0x1a01d09ec702028LL, 0x68bf7f2cd002b4cLL, 
    0x28115c8ed8538c8LL, 0x8a11d0938cdf8baLL, 0x69faa3647843f42LL, 
    0x9f4aa36d3102710LL, 0x9f4e819b8720434LL, 0x2001d7e7dc10a12LL, 
    0x286b9f24c214000LL, 0x1cea2e2ca22d12cLL, 0x60c66c04821c948LL, 
    0x455b1c8e1088aLL, 0xe7ba6c64026d74cLL, 0x4467918a3ad8ecLL, 
    0x1a7b97ff7010432LL, 0x1c67931eb2240b8LL, 0xe09beee4c854f00LL, 
    0xe00458c4ccd8bc2LL, 0x3ffbaee45005514LL, 0x9b3aa03cb12794cLL, 
    0x3878a4642045888LL, 0x60e9d660085c8b2LL, 0xe5ce20600055a02LL, 
    0xd718a4e10027100LL, 0x9b3984d3d3a352cLL, 0xc589a4c14cdc412LL, 
    0x3a79c61ff0070ecLL, 0x1b39c6c14855038LL, 0x20ee292c2127bcaLL, 
    0xd78e202427a3f40LL, 0xd71a20d34fba714LL };

// BinaryA5::names[i] is the cycle decomposition of Perm<5>::S5[i] provided i 
// is even.  If i is odd, it is the notation for the corresponding element of 
// the binary A5 group.
const std::string BinaryA5::names[120] = { "1", "-1", "(234)", "-(234)", 
    "(243)", "-(243)", "(12)(34)", "(34)(12)", "(123)", "-(123)", 
    "(124)", "-(124)", "(132)", "-(132)", "(134)", "-(134)", 
    "(24)(13)", "(13)(24)", "(142)", "-(142)", "(143)", "-(143)", 
    "(14)(23)", "(23)(14)", "(34)(01)", "(01)(34)", "(01)(23)", 
    "(23)(01)", "(24)(01)", "(01)(24)", "(012)", "-(012)", "(01234)", 
    "-(01234)", "(01243)", "-(01243)", "(01342)", "-(01342)", "(013)", 
    "-(013)", "(01324)", "-(01324)", "(01432)", "-(01432)", "(014)", 
    "-(014)", "(01423)", "-(01423)", "(021)", "-(021)", "(02341)",
    "-(02341)", "(02431)", "-(02431)", "(02)(34)", "(34)(02)", 
    "(023)", "-(023)", "(024)", "-(024)", "(13)(02)", "(02)(13)", 
    "(02134)", "-(02134)", "(02413)", "-(02413)", "(14)(02)", 
    "(02)(14)", "(02143)", "-(02143)", "(02314)", "-(02314)", 
    "(03421)", "-(03421)", "(031)", "-(031)", "(03241)", 
    "-(03241)", "(032)", "-(032)", "(034)", "-(034)", "(24)(03)",
    "(03)(24)", "(03412)", "-(03412)", "(12)(03)", "(03)(12)", 
    "(03124)", "-(03124)", "(03142)", "-(03142)", "(03214)", 
    "-(03214)", "(14)(03)", "(03)(14)", "(04321)", "-(04321)",
    "(041)", "-(041)", "(04231)", "-(04231)", "(042)", "-(042)", 
    "(043)", "-(043)", "(04)(23)", "(23)(04)", "(04312)", 
    "-(04312)", "(12)(04)", "(04)(12)", "(04123)", "-(04123)", 
    "(04132)", "-(04132)", "(04213)", "-(04213)", "(04)(13)", "(13)(04)" };

const unsigned long long BinaryA5::floormask = ( ~((unsigned long long) 1) );

const std::string BinaryA5::spinornames[120] = { 
"\\left[1, 1\\right]", "\\left[-1, -1\\right]",
"\\left[\\frac{1}{2}-\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{1}{2}+\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k\\right]", 
"\\left[\\frac{-1}{2}+\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{-1}{2}-\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k\\right]",
"\\left[\\frac{1}{2}+\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{1}{2}-\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k\\right]",
 "\\left[\\frac{-1}{2}-\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{-1}{2}+\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k\\right]",
"\\left[-1k, 1k\\right]", "\\left[1k, -1k\\right]",
"\\left[\\frac{1}{2}+\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{1}{2}-\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k\\right]",
"\\left[\\frac{-1}{2}-\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{-1}{2}+\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k\\right]",
"\\left[\\frac{1}{2}+\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{1}{2}-\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k\\right]",
"\\left[\\frac{-1}{2}-\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{-1}{2}+\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k\\right]",
"\\left[\\frac{1}{2}-\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{1}{2}+\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k\\right]",
"\\left[\\frac{-1}{2}+\\frac{1}{2}i+\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{-1}{2}-\\frac{1}{2}i-\\frac{1}{2}j-\\frac{1}{2}k\\right]",
"\\left[\\frac{1}{2}-\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{1}{2}+\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k\\right]",
"\\left[\\frac{-1}{2}+\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{-1}{2}-\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k\\right]",
"\\left[-1j, 1j\\right]","\\left[1j, -1j\\right]",
"\\left[\\frac{1}{2}-\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{1}{2}+\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k\\right]",
"\\left[\\frac{-1}{2}+\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{-1}{2}-\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k\\right]",
"\\left[\\frac{1}{2}+\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k, "
"\\frac{1}{2}-\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k\\right]",
"\\left[\\frac{-1}{2}-\\frac{1}{2}i+\\frac{1}{2}j-\\frac{1}{2}k, "
"\\frac{-1}{2}+\\frac{1}{2}i-\\frac{1}{2}j+\\frac{1}{2}k\\right]",
"\\left[-1i, 1i\\right]","\\left[1i, -1i\\right]",
"\\left[(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-"
"\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, (\\frac{1}{4}-\\frac{1}{4}"
"\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k\\right]",
"\\left[(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}"
"\\sqrt{5})j+\\frac{1}{2}k, (\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+"
"(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k\\right]",
"\\left[-\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+"
"\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+"
"(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]","\\left[\\frac{1}{2}i+"
"(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, "
"-\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-"
"\\frac{1}{4}\\sqrt{5})k\\right]","\\left[(\\frac{1}{4}-\\frac{1}{4}"
"\\sqrt{5})i+\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, "
"(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j+(\\frac{1}{4}-"
"\\frac{1}{4}\\sqrt{5})k\\right]", "\\left[(\\frac{-1}{4}+\\frac{1}{4}"
"\\sqrt{5})i-\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, "
"(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j+(\\frac{-1}{4}+"
"\\frac{1}{4}\\sqrt{5})k\\right]","\\left[\\frac{1}{2}+(\\frac{1}{4}-"
"\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j, "
"\\frac{1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+"
"\\frac{1}{4}\\sqrt{5})j\\right]", "\\left[\\frac{-1}{2}+(\\frac{-1}{4}+"
"\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j, "
"\\frac{-1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-"
"\\frac{1}{4}\\sqrt{5})j\\right]","\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+"
"(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k, \\frac{1}{4}+"
"\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j-"
"\\frac{1}{2}k\\right]", "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+"
"(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, \\frac{-1}{4}-"
"\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+"
"\\frac{1}{2}k\\right]","\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+"
"(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}k, \\frac{1}{4}-"
"\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+"
"\\frac{1}{2}k\\right]", "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+"
"(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}k, \\frac{-1}{4}+"
"\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i-"
"\\frac{1}{2}k\\right]","\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+"
"(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j, \\frac{1}{4}+"
"\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j"
"\\right]", "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+"
"\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+"
"(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j\\right]",
"\\left[\\frac{1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+"
"\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+"
"(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]", "\\left[\\frac{-1}{2}+"
"(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k,"
" \\frac{-1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-"
"\\frac{1}{4}\\sqrt{5})k\\right]","\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}-"
"\\frac{1}{2}j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}-"
"\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k"
"\\right]", "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+"
"(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}-"
"\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]","\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j\\right]",
"\\left[\\frac{1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
"\\left[\\frac{1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j, \\frac{1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j, \\frac{-1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j\\right]",
"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j\\right]",
"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j\\right]",
"\\left[(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, (\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k\\right]",
 "\\left[(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k, (\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k\\right]",
"\\left[\\frac{1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
"\\left[\\frac{1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
"\\left[(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, (\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, (\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}k, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}k\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}k, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}k\\right]",

"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[-\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, -\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}k, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}k\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}k, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}k\\right]",

"\\left[\\frac{1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j, \\frac{1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j, \\frac{-1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j\\right]",

"\\left[(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, (\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, (\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k\\right]",

"\\left[(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k, (\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k\\right]",
 "\\left[(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, (\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j\\right]",

"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j\\right]",

"\\left[\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, -\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[-\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k\\right]",

"\\left[\\frac{1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{2}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j, \\frac{1}{2}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j\\right]",
 "\\left[\\frac{-1}{2}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j, \\frac{-1}{2}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j\\right]",

"\\left[\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k, -\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[-\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}k, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}k\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}k, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}k\\right]",

"\\left[(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k, (\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k\\right]",
 "\\left[(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j+\\frac{1}{2}k, (\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j-\\frac{1}{2}k\\right]",

"\\left[\\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})j, \\frac{1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})j\\right]",
 "\\left[\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})j, \\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})j\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+\\frac{1}{2}i+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}-\\frac{1}{2}i+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",

"\\left[\\frac{1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j, \\frac{1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j\\right]",
 "\\left[\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5}+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j, \\frac{-1}{4}-\\frac{1}{4}\\sqrt{5}+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j\\right]",

"\\left[(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j+(\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})k, (\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j+(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})k\\right]",
 "\\left[(\\frac{-1}{4}+\\frac{1}{4}\\sqrt{5})i+\\frac{1}{2}j+(\\frac{-1}{4}-\\frac{1}{4}\\sqrt{5})k, (\\frac{1}{4}+\\frac{1}{4}\\sqrt{5})i-\\frac{1}{2}j+(\\frac{1}{4}-\\frac{1}{4}\\sqrt{5})k\\right]"};

} // namespace regina

