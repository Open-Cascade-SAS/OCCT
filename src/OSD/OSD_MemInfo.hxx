// Created on: 2011-10-05
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _OSD_MemInfo_H__
#define _OSD_MemInfo_H__

#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

//! This class provide information about memory utilized by current process.
//! This information includes:
//!  - Private Memory - synthetic value that tries to filter out the memory
//!                     usage only by the process itself (allocated for data
//!                     and stack), excluding dynamic libraries.
//!                     These pages may be in RAM or in SWAP.
//!  - Virtual Memory - amount of reserved and committed memory in the
//!                     user-mode portion of the virtual address space.
//!                     Notice that this counter includes reserved memory
//!                     (not yet in used) and shared between processes memory (libraries).
//!  - Working Set    - set of memory pages in the virtual address space of the process
//!                     that are currently resident in physical memory (RAM).
//!                     These pages are available for an application to use
//!                     without triggering a page fault.
//!  - Pagefile Usage - space allocated for the pagefile, in bytes.
//!                     Those pages may or may not be in memory (RAM)
//!                     thus this counter couldn't be used to estimate
//!                     how many active pages doesn't present in RAM.
//!
//! Notice that none of these counters can be used as absolute measure of
//! application memory consumption!
//!
//! User should analyze all values in specific case to make correct decision
//! about memory (over)usage. This is also prefferred to use specialized
//! tools to detect memory leaks.
//!
//! This also means that these values should not be used for intellectual
//! memory management by application itself.
class OSD_MemInfo : public Standard_Transient
{

public:

  enum Counter
  {
    MemPrivate = 0,    //!< Virtual memory allocated for data and stack excluding libraries
    MemVirtual,        //!< Reserved and committed memory of the virtual address space
    MemWorkingSet,     //!< Memory pages that are currently resident in physical memory
    MemWorkingSetPeak, //!< Peak working set size
    MemSwapUsage,      //!< Space allocated for the pagefile
    MemSwapUsagePeak,  //!< Peak space allocated for the pagefile
    MemCounter_NB      //!< Indicates total counters number
  };

public:

  //! Create and initialize
  Standard_EXPORT OSD_MemInfo();

  //! Update counters
  Standard_EXPORT void Update();

  //! Return the string representation for all available counter.
  Standard_EXPORT TCollection_AsciiString ToString() const;

  //! Return value or specified counter in bytes.
  //! Notice that NOT all counters are available on various systems.
  //! Standard_Size(-1) means invalid (unavailable) value.
  Standard_EXPORT Standard_Size Value (const OSD_MemInfo::Counter theCounter) const;

  //! Return value or specified counter in MiB.
  //! Notice that NOT all counters are available on various systems.
  //! Standard_Size(-1) means invalid (unavailable) value.
  Standard_EXPORT Standard_Size ValueMiB (const OSD_MemInfo::Counter theCounter) const;

public:

  //! Return the string representation for all available counter.
  Standard_EXPORT static TCollection_AsciiString PrintInfo();

private:

  Standard_Size myCounters[MemCounter_NB]; //!< Counters' values, in bytes

};

#endif // _OSD_MemInfo_H__
