// Created on: 2011-10-05
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OSD_MemInfo_H__
#define _OSD_MemInfo_H__

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
class OSD_MemInfo
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
    MemHeapUsage,      //!< Total space allocated from the heap
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
