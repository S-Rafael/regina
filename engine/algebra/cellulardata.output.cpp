
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

#include "maths/matrixops.h"
#include "maths/nprimes.h"
#include "algebra/cellulardata.h"

#include <map>
#include <list>
#include <cmath>

#include <iostream>
#include <sstream>

namespace regina {

void CellularData::writeTextShort(std::ostream& out) const 
{
bool written=false;
// run through list of everything computed, and print. 
 std::map< GroupLocator, NAbelianGroup* >::const_iterator abi;
 for (abi = abelianGroups.begin(); abi != abelianGroups.end(); abi++)
	{
	 if (written) out<<" ";
	 abi->first.writeTextShort(out);
	 out<<" is ";
	 abi->second->writeTextShort(out);
	 written = true;
        }
 std::map< GroupLocator, NMarkedAbelianGroup* >::const_iterator mabi;
 for (mabi = markedAbelianGroups.begin(); 
      mabi != markedAbelianGroups.end(); mabi++)
	{
	 if (written) out<<" ";
	 mabi->first.writeTextShort(out);
	 out<<" is ";
	 mabi->second->writeTextShort(out);
	 written = true;
        }
 std::map< HomLocator, NHomMarkedAbelianGroup* >::const_iterator hmabi;
 for (hmabi = homMarkedAbelianGroups.begin(); 
      hmabi != homMarkedAbelianGroups.end(); hmabi++)
	{
	 if (written) out<<" ";
	 hmabi->first.writeTextShort(out);
	 out<<" is ";
	 hmabi->second->writeTextShort(out);
	 written = true;
        }
 std::map< FormLocator, NBilinearForm* >::const_iterator fmit;
 for (fmit = bilinearForms.begin(); fmit != bilinearForms.end(); fmit++)
 	{
	 if (written) out<<" ";
	 fmit->first.writeTextShort(out);
	 out<<" is ";
	 fmit->second->writeTextShort(out);
	 written = true;
	}
 std::map< GroupPresLocator, NGroupPresentation* >::const_iterator git;
 for (git = groupPresentations.begin(); git != groupPresentations.end(); git++)
 	{
	 if (written) out<<" ";
	 git->first.writeTextShort(out);
	 out<<" is ";
         out<<git->second->toString();
//	 git->second->writeTextShort(out);
	 written = true;
	}
 std::map< HomGroupPresLocator, NHomGroupPresentation* >::const_iterator hit;
 for (hit = homGroupPresentations.begin(); 
      hit != homGroupPresentations.end(); hit++)
 	{
	 if (written) out<<" ";
	 hit->first.writeTextShort(out);
	 out<<" is ";
         out<<hit->second->toString();
//	 hit->second->writeTextShort(out);
	 written = true;
	}
}

// TODO go through this again using coordinate systems as the index, 
//  not markedAbelianGroups, 
void CellularData::writeTextLong(std::ostream& out) const
{
// list basics of manifold -- dimension, boundary types, orientable, etc.
if (tri3) out<<"3"; else out<<"4";
out<<"-manifold triangulation";
if (tri3) { 
 out<<" named \""; tri3->writeTextShort(out); 
 out<<"\" is"; // use writeTextLong?
 if (tri3->isOrientable()) out<<" orientable"; else out<<" non-orientable"; 
 if (tri3->isIdeal() && tri3->hasBoundaryFaces()) 
    out<<" has ideal and standard boundary";
 else if (tri3->isIdeal()) out<<" has ideal boundary";
 else if (tri3->hasBoundaryFaces()) out<<" has standard boundary";
 else out<<" no boundary";
}
else 
{
 out<<" named \""; tri4->writeTextShort(out); out<<"\" is"; 
 if (tri4->isOrientable()) out<<" orientable"; 
  else out<<" non-orientable"; 
 if (tri4->isIdeal() && tri4->hasBoundaryTetrahedra()) 
  out<<" has ideal and standard boundary";
 else if (tri4->isIdeal()) out<<" has ideal boundary";
 else if (tri4->hasBoundaryTetrahedra()) out<<" has standard boundary";
 else out<<" no boundary";
}
// run through list of everything computed, and print. 
 std::map< GroupLocator, NAbelianGroup* >::const_iterator abi;
 for (abi = abelianGroups.begin(); abi != abelianGroups.end(); abi++)
  {
   out<<" ";
   abi->first.writeTextShort(out); out<<" is ";
   abi->second->writeTextShort(out); out<<", ";
  }
 std::map< GroupLocator, NMarkedAbelianGroup* >::const_iterator mabi;
 for (mabi = markedAbelianGroups.begin(); 
      mabi != markedAbelianGroups.end(); mabi++)
  {
   out<<" "; mabi->first.writeTextShort(out);
   out<<" is "; mabi->second->writeTextShort(out);
   out<<", ";
  }
 std::map< HomLocator, NHomMarkedAbelianGroup* >::const_iterator hmabi;
 for (hmabi = homMarkedAbelianGroups.begin(); 
      hmabi != homMarkedAbelianGroups.end(); hmabi++)
  {
   out<<" "; hmabi->first.writeTextShort(out);
   out<<" is "; hmabi->second->writeTextShort(out);
   out<<", ";
  }
 std::map< FormLocator, NBilinearForm* >::const_iterator fmit;
 for (fmit = bilinearForms.begin(); fmit != bilinearForms.end(); fmit++)
  {
   out<<" ";
   fmit->first.writeTextShort(out);
   out<<" is ";
   fmit->second->writeTextShort(out);
   out<<", ";
  }
 std::map< GroupPresLocator, NGroupPresentation* >::const_iterator git;
 for (git = groupPresentations.begin(); git != groupPresentations.end(); git++)
  {
   out<<" ";
   git->first.writeTextShort(out);
   out<<" is ";
   git->second->writeTextLong(out);
  }
 std::map< HomGroupPresLocator, NHomGroupPresentation* >::const_iterator hit;
 for (hit = homGroupPresentations.begin(); 
      hit != homGroupPresentations.end(); hit++)
  {
   out<<" ";
   hit->first.writeTextShort(out);
   out<<" is ";
   hit->second->writeTextLong(out);
  }

 out<<" Euler Char == "<<eulerChar();
 out<<" Poincare Polynomial == "<<poincarePolynomial();
}



} // namespace regina


