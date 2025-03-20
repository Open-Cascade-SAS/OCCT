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

#ifndef OSD_MemInfo_HeaderFile
#define OSD_MemInfo_HeaderFile

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
//! about memory (over)usage. This is also preferred to use specialized
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
  //! Create and initialize. By default all countes are active
  Standard_EXPORT OSD_MemInfo(const Standard_Boolean theImmediateUpdate = Standard_True);

  //! Return true if the counter is active
  Standard_Boolean IsActive(const OSD_MemInfo::Counter theCounter) const
  {
    return myActiveCounters[theCounter];
  }

  //! Set all counters active. The information is collected for active counters.
  //! @param theActive state for counters
  Standard_EXPORT void SetActive(const Standard_Boolean theActive);

  //! Set the counter active. The information is collected for active counters.
  //! @param theCounter type of counter
  //! @param theActive state for the counter
  void SetActive(const OSD_MemInfo::Counter theCounter, const Standard_Boolean theActive)
  {
    myActiveCounters[theCounter] = theActive;
  }

  //! Clear counters
  Standard_EXPORT void Clear();

  //! Update counters
  Standard_EXPORT void Update();

  //! Return the string representation for all available counter.
  Standard_EXPORT TCollection_AsciiString ToString() const;

  //! Return value of specified counter in bytes.
  //! Notice that NOT all counters are available on various systems.
  //! Standard_Size(-1) means invalid (unavailable) value.
  Standard_EXPORT Standard_Size Value(const OSD_MemInfo::Counter theCounter) const;

  //! Return value of specified counter in MiB.
  //! Notice that NOT all counters are available on various systems.
  //! Standard_Size(-1) means invalid (unavailable) value.
  Standard_EXPORT Standard_Size ValueMiB(const OSD_MemInfo::Counter theCounter) const;

  //! Return floating value of specified counter in MiB.
  //! Notice that NOT all counters are available on various systems.
  //! Standard_Real(-1) means invalid (unavailable) value.
  Standard_EXPORT Standard_Real ValuePreciseMiB(const OSD_MemInfo::Counter theCounter) const;

public:
  //! Return the string representation for all available counter.
  Standard_EXPORT static TCollection_AsciiString PrintInfo();

protected:
  //! Return true if the counter is active and the value is valid
  Standard_Boolean hasValue(const OSD_MemInfo::Counter theCounter) const
  {
    return IsActive(theCounter) && myCounters[theCounter] != Standard_Size(-1);
  }

private:
  Standard_Size    myCounters[MemCounter_NB];       //!< Counters' values, in bytes
  Standard_Boolean myActiveCounters[MemCounter_NB]; //!< container of active state for a counter
};

#endif // _OSD_MemInfo_H__
