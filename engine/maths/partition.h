
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

/*! \file maths/npartition.h
 *  \brief Provides utility class for iterating through partitions of subsets
 *   of a set.
 */

#ifndef __NPartition_H
#ifndef __DOXYGEN
#define __NPartition_H
#endif

#include <sstream>

#include "regina-core.h"
#include "utilities/bitmask.h"
#include <ostream>

namespace regina {

/**
 * \weakgroup maths
 * @{
 */

/**
 *  Class for dealing with partitions of sets of integers {0,1,...,n-1}. 
 * You can use it to iterate through all subsets of this set, or all
 * subsets of a fixed size. 
 */
class REGINA_API NPartition {
        NBitmask part;
        unsigned long setSize;
        unsigned long subSetSize;
        bool beforeStart, afterEnd; 
        bool fixedSize;
public:
        /**
         * Constructor. Ambient set size will be SetSize, subsets of cardinality
         * SubSetSize to be iterated through (*). Setting fixedSubsetSize to false
         * will cause iterators to go through all subsets, starting at SubSetSize
         */
        NPartition(unsigned long SetSize, unsigned long SubSetSize=0, bool fixedSubsetSize=true);
        /**
         * Copy constructor. 
         */        
        NPartition(const NPartition& copyMe);
        /**
         * Increment operator
         */
        NPartition operator++();
        /**
         *  Reset the partition, as in the constructor with the same arguments.
         */
        void reset(unsigned long SetSize, unsigned long SubSetSize=0, bool fixedSubsetSize=true);
        /**
         * Has iteration passed the end of the list?
         */
        bool atEnd();
        /**
         * Are we before the start of the list?
         */
        bool atStart();
        /**
         * Request the current partition's NBitmask representation.
         */
        const NBitmask& partition() const;    
        /**
         * What is the order of set?
         */
        unsigned long sSize() const;
        /**
         * What is the order of the subsets we're iterating through? 
         */
        unsigned long ssSize() const;
        /**
         * A linear order on partitions -- the lexicographic order. 
         */
        bool operator<(const NPartition &that) const;
        /**
         * A vector description of the partition. 
         */    
        std::vector< unsigned long > vectorDesc() const;
        /**
         * Text output in form of a NBitMask
         */
        std::string textString() const;
        /**
         * Text output in form of a NBitMask.  If there are n elements in your
         * set your output will be a binary string of length n, with entries
         * either 0 or 1.  00...0 indicates the empty subset, 11...1 indicates 
         * the entire set. 
         */
        friend std::ostream& operator<<(std::ostream& output, 
                                        const NPartition& out);
};


inline NPartition::NPartition(unsigned long SetSize, unsigned long SubSetSize, bool fixedSubsetSize) : part(SetSize), 
  setSize(SetSize), subSetSize(SubSetSize), beforeStart(false), afterEnd(false), fixedSize(fixedSubsetSize)
{  for (unsigned long i=0; i<subSetSize; i++) part.set(i, true); 
   if (SubSetSize > SetSize) afterEnd=true;
}

inline void NPartition::reset(unsigned long SetSize, unsigned long SubSetSize, bool fixedSubsetSize) 
{
 setSize = SetSize; subSetSize = SubSetSize;  beforeStart = false; afterEnd = false; 
 fixedSize = fixedSubsetSize;
 for (unsigned long i=0; i<setSize; i++) part.set(i, (i<subSetSize) ? true : false); 
   if (SubSetSize > SetSize) afterEnd=true;
}

inline NPartition::NPartition(const NPartition& copyMe) : part(copyMe.part), setSize(copyMe.setSize), 
 subSetSize(copyMe.subSetSize), beforeStart(copyMe.beforeStart), afterEnd(copyMe.afterEnd), fixedSize(copyMe.fixedSize) {}

inline NPartition NPartition::operator++()
{
 //   (a) find first one from RHS with a zero to the right. Move it right. 
 //       if not the *last* entry, append the appropriate number of consecutive 1's.
 if (afterEnd) return (*this);
 unsigned long numOnesWithoutZerosToRight=0; // count this from right to left. 
 signed long indx;
 for (indx=setSize-1; (indx>=0) && ((indx==setSize-1)? true : !(part.get(indx) && !part.get(indx+1)) ); indx--) 
  { if ((indx<setSize-1)&&(part.get(indx+1))) { part.set(indx+1, false); }
    if (part.get(indx)) numOnesWithoutZerosToRight++; }
 // can terminate with indx==-1 or at first index of a 1 with a 0 to right
 if (indx != -1)
  {
   part.set(indx, false);  part.set(indx+1,true);
   for (unsigned long i=0; i<numOnesWithoutZerosToRight; i++) part.set(indx+2+i, true); 
  }
 else
  {
   //   (c) if you can't find such a 1 in (b), done with this set size. 
   //   *(d) move up to next set size *if* all set sizes requested...
   if (!fixedSize) 
    { 
      if (numOnesWithoutZerosToRight == setSize) afterEnd = true;
      else 
       {
        subSetSize++;
        part.reset(); 
        for (unsigned long i=0; i<numOnesWithoutZerosToRight+1; i++) part.set(i, true); 
       }
    } 
   else afterEnd=true;   // fixed set size    
  }
 return (*this);
}

inline bool NPartition::atEnd()
{ return afterEnd; }

inline bool NPartition::atStart()
{ return beforeStart; }

inline const NBitmask& NPartition::partition() const
{ return part; }      
 
inline bool NPartition::operator<(const NPartition &that) const
{ // 0 < 1 lexico order.
 for (unsigned long i=0; i<setSize; i++)
  { if ( (!part.get(i)) && (that.part.get(i)) ) return true; 
    if ( (part.get(i))  && (!that.part.get(i)) ) return false; }
 return false;
}

inline std::string NPartition::textString() const
{
 std::stringstream ss;
 for (unsigned long i=0; i<setSize; i++)
  ss<<( part.get(i) ? "1" : "0" );
 return ss.str();
}

inline unsigned long NPartition::sSize() const
{ return setSize; }      

inline unsigned long NPartition::ssSize() const
{ return subSetSize; }      

inline std::ostream& operator<<(std::ostream& output, const NPartition& out)
 { output<<out.textString(); return output; }

inline std::vector< unsigned long > NPartition::vectorDesc() const
{
 std::vector< unsigned long > retval;
 for (unsigned long i=0; i<setSize; i++)
  if (part.get(i)) retval.push_back(i);
 return retval;  
}

/*@}*/

}// end namespace regina

#endif

